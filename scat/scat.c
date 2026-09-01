#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <fcntl.h>
#include <io.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif

#include "port.h"

/* scat -- slow cat for playing back ESC animations */
#define US_PER_SECOND 1000000
#if defined(_WIN32)
#define MIN_DELAY_PER_CHUNK_US 10000
#else
#define MIN_DELAY_PER_CHUNK_US 100
#endif
#define XON 0x11
#define XOFF 0x13

#if defined(_WIN32)
typedef SOCKET socket_handle;
#define INVALID_SOCKET_HANDLE INVALID_SOCKET
#define CLOSE_SOCKET closesocket
#else
typedef int socket_handle;
#define INVALID_SOCKET_HANDLE (-1)
#define CLOSE_SOCKET close
#define SOCKET_ERROR (-1)
#endif

struct output
{
    int network;
    socket_handle socket;
};

static void usage(void)
{
    fprintf(stderr,
            "Usage: scat [--listen port|--connect host port] "
            "[--start-delay seconds] [--hold-open] <baud> [file]\n");
}

static int parse_baud(const char *arg)
{
    char *end = NULL;
    long baud = strtol(arg, &end, 10);
    if (*arg == '\0' || *end != '\0' || baud <= 0)
    {
        fprintf(stderr, "Invalid baud rate: %s\n", arg);
        return 0;
    }
    return (int) baud;
}

static int parse_start_delay(const char *arg, int *start_delay)
{
    char *end = NULL;
    long delay = strtol(arg, &end, 10);
    if (*arg == '\0' || *end != '\0' || delay < 0)
    {
        fprintf(stderr, "Invalid start delay: %s\n", arg);
        return 1;
    }
    if (delay > INT_MAX / US_PER_SECOND)
    {
        fprintf(stderr, "Start delay is too large: %s\n", arg);
        return 1;
    }
    *start_delay = (int) delay;
    return 0;
}

static void compute_delay(int baud, int *chunk_size, int *delay_per_chunk)
{
    *chunk_size = 1;
    /* Assume a 10-bit frame for each character: 1 start, 8 data, 1 stop. */
    *delay_per_chunk = (int) ((long long) US_PER_SECOND * 10 * *chunk_size / baud);
    while (*delay_per_chunk < MIN_DELAY_PER_CHUNK_US)
    {
        ++*chunk_size;
        *delay_per_chunk = (int) ((long long) US_PER_SECOND * 10 * *chunk_size / baud);
    }
}

static void set_binary_stdio(void)
{
#if defined(_WIN32)
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif
}

static int socket_startup(void)
{
#if defined(_WIN32)
    WSADATA data;
    int result = WSAStartup(MAKEWORD(2, 2), &data);
    if (result != 0)
    {
        fprintf(stderr, "WSAStartup failed: %d\n", result);
        return 1;
    }
#endif
    return 0;
}

static void socket_cleanup(void)
{
#if defined(_WIN32)
    WSACleanup();
#endif
}

static void socket_error(const char *operation)
{
#if defined(_WIN32)
    fprintf(stderr, "%s failed: %d\n", operation, WSAGetLastError());
#else
    fprintf(stderr, "%s failed: %s\n", operation, strerror(errno));
#endif
}

static void configure_stream_socket(socket_handle socket)
{
    int yes = 1;
    setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (const char *) &yes, sizeof(yes));
}

static socket_handle listen_for_connection(const char *port)
{
    struct addrinfo hints;
    struct addrinfo *addresses = NULL;
    struct addrinfo *address = NULL;
    socket_handle listener = INVALID_SOCKET_HANDLE;
    socket_handle client = INVALID_SOCKET_HANDLE;
    int yes = 1;
    int result = 0;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    result = getaddrinfo("127.0.0.1", port, &hints, &addresses);
    if (result != 0)
    {
        fprintf(stderr, "getaddrinfo failed for port %s: %d\n", port, result);
        return INVALID_SOCKET_HANDLE;
    }

    for (address = addresses; address != NULL; address = address->ai_next)
    {
        listener = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
        if (listener == INVALID_SOCKET_HANDLE)
        {
            continue;
        }

        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (const char *) &yes, sizeof(yes));
        if (bind(listener, address->ai_addr, (int) address->ai_addrlen) == 0 && listen(listener, 1) == 0)
        {
            break;
        }

        CLOSE_SOCKET(listener);
        listener = INVALID_SOCKET_HANDLE;
    }

    freeaddrinfo(addresses);
    if (listener == INVALID_SOCKET_HANDLE)
    {
        socket_error("listen");
        return INVALID_SOCKET_HANDLE;
    }

    fprintf(stderr, "scat: listening on 127.0.0.1:%s\n", port);
    client = accept(listener, NULL, NULL);
    if (client == INVALID_SOCKET_HANDLE)
    {
        socket_error("accept");
    }
    else
    {
        configure_stream_socket(client);
    }
    CLOSE_SOCKET(listener);
    return client;
}

static socket_handle connect_to_host(const char *host, const char *port)
{
    struct addrinfo hints;
    struct addrinfo *addresses = NULL;
    struct addrinfo *address = NULL;
    socket_handle client = INVALID_SOCKET_HANDLE;
    int result = 0;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    result = getaddrinfo(host, port, &hints, &addresses);
    if (result != 0)
    {
        fprintf(stderr, "getaddrinfo failed for %s:%s: %d\n", host, port, result);
        return INVALID_SOCKET_HANDLE;
    }

    for (address = addresses; address != NULL; address = address->ai_next)
    {
        client = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
        if (client == INVALID_SOCKET_HANDLE)
        {
            continue;
        }

        if (connect(client, address->ai_addr, (int) address->ai_addrlen) == 0)
        {
            break;
        }

        CLOSE_SOCKET(client);
        client = INVALID_SOCKET_HANDLE;
    }

    freeaddrinfo(addresses);
    if (client == INVALID_SOCKET_HANDLE)
    {
        socket_error("connect");
    }
    else
    {
        configure_stream_socket(client);
    }
    return client;
}

static int socket_readable(socket_handle socket, int wait)
{
    fd_set read_set;
    struct timeval timeout;
    struct timeval *timeout_ptr = NULL;
    int result = 0;

    FD_ZERO(&read_set);
    FD_SET(socket, &read_set);
    if (!wait)
    {
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        timeout_ptr = &timeout;
    }

#if defined(_WIN32)
    result = select(0, &read_set, NULL, NULL, timeout_ptr);
#else
    result = select(socket + 1, &read_set, NULL, NULL, timeout_ptr);
#endif
    if (result == SOCKET_ERROR)
    {
        socket_error("select");
        return -1;
    }
    return result > 0;
}

static int socket_readable_for(socket_handle socket, int timeout_us)
{
    fd_set read_set;
    struct timeval timeout;
    int result = 0;

    FD_ZERO(&read_set);
    FD_SET(socket, &read_set);
    timeout.tv_sec = timeout_us / US_PER_SECOND;
    timeout.tv_usec = timeout_us % US_PER_SECOND;

#if defined(_WIN32)
    result = select(0, &read_set, NULL, NULL, &timeout);
#else
    result = select(socket + 1, &read_set, NULL, NULL, &timeout);
#endif
    if (result == SOCKET_ERROR)
    {
        socket_error("select");
        return -1;
    }
    return result > 0;
}

static int receive_socket_byte(socket_handle socket, unsigned char *byte)
{
    int received = recv(socket, (char *) byte, 1, 0);
    if (received == 1)
    {
        return 1;
    }
    if (received == 0)
    {
        fprintf(stderr, "scat: peer closed the connection\n");
        return 0;
    }
    socket_error("recv");
    return -1;
}

static int apply_flow_control(socket_handle socket, int *paused)
{
    for (;;)
    {
        unsigned char byte = 0;
        int ready = socket_readable(socket, *paused);
        int received = 0;

        if (ready <= 0)
        {
            return ready;
        }

        received = receive_socket_byte(socket, &byte);
        if (received <= 0)
        {
            return -1;
        }

        if (byte == XOFF)
        {
            *paused = 1;
        }
        else if (byte == XON)
        {
            *paused = 0;
        }

        if (!*paused)
        {
            ready = socket_readable(socket, 0);
            if (ready <= 0)
            {
                return ready;
            }
        }
    }
}

static int poll_flow_control(socket_handle socket, int *paused)
{
    for (;;)
    {
        unsigned char byte = 0;
        int ready = socket_readable(socket, 0);
        int received = 0;

        if (ready <= 0)
        {
            return ready;
        }

        received = receive_socket_byte(socket, &byte);
        if (received <= 0)
        {
            return -1;
        }

        if (byte == XOFF)
        {
            *paused = 1;
        }
        else if (byte == XON)
        {
            *paused = 0;
        }
    }
}

static int write_socket_byte(socket_handle socket, unsigned char byte)
{
    const char *buffer = (const char *) &byte;
    int remaining = 1;

    while (remaining > 0)
    {
        int sent = send(socket, buffer, remaining, 0);
        if (sent == SOCKET_ERROR)
        {
            socket_error("send");
            return 1;
        }
        buffer += sent;
        remaining -= sent;
    }
    return 0;
}

static int write_byte(struct output *output, unsigned char byte)
{
    if (output->network)
    {
        return write_socket_byte(output->socket, byte);
    }
    if (putchar(byte) == EOF)
    {
        fprintf(stderr, "scat: write failed\n");
        return 1;
    }
    return 0;
}

static int flush_output(struct output *output)
{
    if (output->network)
    {
        return 0;
    }
    return fflush(stdout) == EOF;
}

static int delay_output(struct output *output, int *paused, int delay_us)
{
    int ready = 0;

    if (!output->network)
    {
        usleep(delay_us);
        return 0;
    }

    ready = socket_readable_for(output->socket, delay_us);
    if (ready < 0)
    {
        return -1;
    }
    if (ready == 0)
    {
        return 0;
    }

    return poll_flow_control(output->socket, paused);
}

static int send_file(FILE *file, struct output *output, int baud)
{
    int chunk_size = 1;
    int delay_per_chunk = 0;
    int char_count = 0;
    int paused = 0;
    int ch = 0;
    int next_ch = 0;

    compute_delay(baud, &chunk_size, &delay_per_chunk);

    ch = fgetc(file);
    while (ch != EOF)
    {
        next_ch = fgetc(file);
        if (output->network &&
            apply_flow_control(output->socket, &paused) < 0)
        {
            return 1;
        }

        if (write_byte(output, (unsigned char) ch) != 0)
        {
            return 1;
        }
        if (next_ch != EOF && output->network &&
            apply_flow_control(output->socket, &paused) < 0)
        {
            return 1;
        }

        ++char_count;
        if (next_ch != EOF && char_count % chunk_size == 0)
        {
            if (flush_output(output))
            {
                return 1;
            }
            if (delay_output(output, &paused, delay_per_chunk) < 0)
            {
                return 1;
            }
        }

        ch = next_ch;
    }

    if (ferror(file))
    {
        fprintf(stderr, "scat: read failed\n");
        return 1;
    }

    return flush_output(output) ? 1 : 0;
}

static int hold_socket_open(socket_handle socket)
{
    char buffer[256];

    for (;;)
    {
        int received = recv(socket, buffer, sizeof(buffer), 0);
        if (received > 0)
        {
            continue;
        }
        if (received == 0)
        {
            return 0;
        }

#if defined(_WIN32)
        if (WSAGetLastError() == WSAECONNRESET)
        {
            return 0;
        }
#else
        if (errno == ECONNRESET)
        {
            return 0;
        }
#endif

        socket_error("recv");
        return 1;
    }
}

int main(int argc, char *argv[])
{
    const char *listen_port = NULL;
    const char *connect_host = NULL;
    const char *connect_port = NULL;
    const char *file_name = NULL;
    int arg = 1;
    int start_delay = 0;
    int hold_open = 0;
    int baud = 0;
    FILE *file = stdin;
    struct output output;
    int result = 0;

    output.network = 0;
    output.socket = INVALID_SOCKET_HANDLE;

    if (arg < argc && strcmp(argv[arg], "--listen") == 0)
    {
        if (arg + 1 >= argc)
        {
            usage();
            return 1;
        }
        listen_port = argv[arg + 1];
        arg += 2;
    }
    else if (arg < argc && strcmp(argv[arg], "--connect") == 0)
    {
        if (arg + 2 >= argc)
        {
            usage();
            return 1;
        }
        connect_host = argv[arg + 1];
        connect_port = argv[arg + 2];
        arg += 3;
    }

    while (arg < argc && strncmp(argv[arg], "--", 2) == 0)
    {
        if (strcmp(argv[arg], "--start-delay") == 0)
        {
            if (arg + 1 >= argc || parse_start_delay(argv[arg + 1], &start_delay))
            {
                usage();
                return 1;
            }
            arg += 2;
        }
        else if (strcmp(argv[arg], "--hold-open") == 0)
        {
            hold_open = 1;
            ++arg;
        }
        else
        {
            usage();
            return 1;
        }
    }

    if (argc < arg + 1 || argc > arg + 2)
    {
        usage();
        return 1;
    }

    baud = parse_baud(argv[arg]);
    if (baud == 0)
    {
        return 1;
    }

    if (argc == arg + 2)
    {
        file_name = argv[arg + 1];
        file = fopen(file_name, "rb");
        if (file == NULL)
        {
            fprintf(stderr, "scat: cannot open %s: %s\n", file_name, strerror(errno));
            return 1;
        }
    }

    set_binary_stdio();

    if (listen_port != NULL || connect_host != NULL)
    {
        if (socket_startup() != 0)
        {
            result = 1;
            goto done;
        }
        output.network = 1;
    }
    if (listen_port != NULL)
    {
        output.socket = listen_for_connection(listen_port);
        if (output.socket == INVALID_SOCKET_HANDLE)
        {
            result = 1;
            goto done;
        }
        fprintf(stderr, "scat: client connected\n");
    }
    else if (connect_host != NULL)
    {
        output.socket = connect_to_host(connect_host, connect_port);
        if (output.socket == INVALID_SOCKET_HANDLE)
        {
            result = 1;
            goto done;
        }
        fprintf(stderr, "scat: connected to %s:%s\n", connect_host, connect_port);
    }

    if (start_delay > 0)
    {
        usleep(start_delay * US_PER_SECOND);
    }

    result = send_file(file, &output, baud);
    if (result == 0 && output.network && hold_open)
    {
        result = hold_socket_open(output.socket);
    }

done:
    if (output.socket != INVALID_SOCKET_HANDLE)
    {
        CLOSE_SOCKET(output.socket);
    }
    if (listen_port != NULL || connect_host != NULL)
    {
        socket_cleanup();
    }
    if (file != stdin)
    {
        fclose(file);
    }
    return result;
}
