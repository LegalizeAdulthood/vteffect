#include <cassert>
#include <iomanip>
#include <iostream>

namespace {

struct mnemonic {
    char prefix;
    const char *mnemonic;
};

class dumper
{
public:
    dumper(std::istream &str) : m_str(str) {}

    void dump();

private:
    void dump(char c);
    void dump_string(char c);
    void dump_control(char c);
    void dump_esc();
    void dump_csi();
    void dump_csi_function(const std::string &args, char function);
    char get();
    void unget(char c);
    bool eof();

    std::istream &m_str;
    bool m_has_buffer = false;
    char m_buffer = 0;
};

bool dumper::eof()
{
    return !m_has_buffer && !m_str;
}

char dumper::get()
{
    if (m_has_buffer)
    {
        m_has_buffer = false;
        return m_buffer;
    }

    char c;
    m_str >> std::noskipws >> c;
    return c;
}

void dumper::unget(char c)
{
    assert(!m_has_buffer);
    m_has_buffer = true;
    m_buffer = c;
}

void dumper::dump()
{
    while (!eof())
    {
        dump(get());
    }
}

void dumper::dump_string(char c)
{
    int column = 0;
    std::cout << '"';
    while (std::isprint(static_cast<unsigned char>(c)))
    {
        ++column;
        if (column > 40)
        {
            std::cout << "\"\n\"";
            column = 1;
        }
        std::cout << c;
        if (eof())
        {
            break;
        }
        c = get();
    }
    std::cout << "\"\n";
    if (!std::isprint(static_cast<unsigned char>(c)))
    {
        unget(c);
    }
}

void dumper::dump(char c)
{
    if (std::isprint(static_cast<unsigned char>(c)))
    {
        dump_string(c);
    }
    else
    {
        dump_control(c);
    }
}

void dumper::dump_csi_function(const std::string &args, char function)
{
    if (!std::isalpha(function))
    {
        assert(false);
    }

    mnemonic mnemonics[] = {
        { 'A', "cuu" },
        { 'B', "cud" },
        { 'C', "cuf" },
        { 'D', "cub" },
        { 'H', "cup" },
        { 'J', "ed" },
        { 'K', "el" },
        { 'R', "cpr" },
        { 'c', "da" },
        { 'f', "hvp" },
        { 'g', "tbc" },
        { 'h', "sm" },
        { 'l', "rm" },
        { 'm', "sgr" },
        { 'n', "dsr" },
        { 'q', "decll" },
        { 'r', "decstbm" },
        { 'x', "decreqtparm" },
        { 'y', "dectst" }
    };
    for (const auto &entry : mnemonics)
    {
        if (function == entry.prefix)
        {
            std::cout << entry.mnemonic;
            if (!args.empty())
            {
                std::cout << " \"" << args << '"';
            }
            std::cout << '\n';
            return;
        }
    }
    std::cout << "esc\n\"[" << args << function << "\"\n";
}

void dumper::dump_csi()
{
    std::string args;
    while (!eof())
    {
        const char c = get();
        if (std::isdigit(c) || c == ';'
            || c == '<' || c == '=' || c == '>' || c == '?')
        {
            args += c;
        }
        else
        {
            unget(c);
            break;
        }
    }
    if (!eof())
    {
        dump_csi_function(args, get());
    }
    else
    {
        std::cout << "esc\n\"[" << args << "\"\n";
    }
}

void dumper::dump_esc()
{
    if (eof())
    {
        std::cout << "esc\n";
        return;
    }

    char c = get();
    switch (c)
    {
    case '(':
        std::cout << "scs g0," << get() << '\n';
        return;

    case ')':
        std::cout << "scs g1," << get() << '\n';
        return;

    case '#':
        {
            mnemonic mnemonics[] = {
                { '3', "decdhltop" },
                { '4', "decdhlbot" },
                { '5', "decswl" },
                { '6', "decdwl" },
                { '7', "dechcp" },
                { '8', "decaln" }
            };
            c = get();

            for (const auto &entry : mnemonics)
            {
                if (c == entry.prefix)
                {
                    std::cout << entry.mnemonic << '\n';
                    return;
                }
            }

            std::cout << "\"#" << c << "\"\n";
            return;
        }

    case '[':
        dump_csi();
        return;
    }

    mnemonic mnemonics[] = {
        { '1', "decgon" },
        { '2', "decgoff" },
        { '7', "decsc" },
        { '8', "decrc" },
        { '=', "deckpam" },
        { '>', "deckpnm" },
        { 'D', "ind" },
        { 'E', "nel" },
        { 'H', "hts" },
        { 'M', "ri" },
        { 'z', "decid" },
        { 'c', "ris" }
    };
    for (const auto &entry : mnemonics)
    {
        if (c == entry.prefix)
        {
            std::cout << entry.mnemonic << '\n';
            return;
        }
    }

    std::cout << "esc\n";
    unget(c);
}

void dumper::dump_control(char c)
{
    if (c == 27)
    {
        dump_esc();
        return;
    }

    const char *mnemonics[] = {
        "nul", "soh", "stx", "etx", "eot", "enq", "ack", "bel",
        "bs", "ht", "lf", "vt", "ff", "cr", "so", "si",
        "dle", "dc1", "dc2", "dc3", "dc4", "naq", "syn", "etb",
        "can", "em", "sub", "esc", "is4", "is3", "is2", "is1"
    };
    if (c < 32)
    {
        std::cout << mnemonics[static_cast<int>(c)] << '\n';
        return;
    }

    if (c == 127)
    {
        std::cout << "del\n";
        return;
    }

    assert(c < 0);
    std::cout << "\\0x" << std::setw(2) << std::hex
        << static_cast<unsigned char>(c) << '\n';
}

} // namespace

void dump(std::istream &str)
{
    dumper(str).dump();
}
