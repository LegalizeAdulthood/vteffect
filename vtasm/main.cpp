#include <fstream>
#include <iostream>
#include <string>

#include <stdout_control.h>
#include "vtasm.h"

namespace
{

void usage()
{
    std::cerr << "Usage: vtasm [-o output-file|--output output-file] [file]\n";
}

enum class parse_result
{
    ok,
    help,
    error
};

parse_result parse_args(int argc, char *argv[], std::string &input_file, std::string &output_file)
{
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-o" || arg == "--output")
        {
            if (++i == argc)
            {
                std::cerr << arg << " requires an output file\n";
                usage();
                return parse_result::error;
            }
            output_file = argv[i];
        }
        else if (arg.rfind("--output=", 0) == 0)
        {
            output_file = arg.substr(9);
            if (output_file.empty())
            {
                std::cerr << "--output requires an output file\n";
                usage();
                return parse_result::error;
            }
        }
        else if (arg == "-h" || arg == "--help")
        {
            usage();
            return parse_result::help;
        }
        else if (!arg.empty() && arg[0] == '-')
        {
            std::cerr << "Unknown option: " << arg << '\n';
            usage();
            return parse_result::error;
        }
        else if (!input_file.empty())
        {
            std::cerr << "Only one input file may be specified\n";
            usage();
            return parse_result::error;
        }
        else
        {
            input_file = arg;
        }
    }

    return parse_result::ok;
}

} // namespace

int main(int argc, char *argv[])
{
    std::string input_file_name;
    std::string output_file_name;
    switch (parse_args(argc, argv, input_file_name, output_file_name))
    {
    case parse_result::ok:
        break;
    case parse_result::help:
        return 0;
    case parse_result::error:
        return 2;
    }

    std::ifstream input_file;
    std::istream *input = &std::cin;
    if (!input_file_name.empty())
    {
        input_file.open(input_file_name);
        if (!input_file)
        {
            std::cerr << "Cannot open input file: " << input_file_name << '\n';
            return 1;
        }
        input = &input_file;
    }

    std::ofstream output_file;
    std::streambuf *previous_output = nullptr;
    if (!output_file_name.empty())
    {
        output_file.open(output_file_name, std::ios::binary);
        if (!output_file)
        {
            std::cerr << "Cannot open output file: " << output_file_name << '\n';
            return 1;
        }
        previous_output = std::cout.rdbuf(output_file.rdbuf());
    }
    else
    {
        set_binary_stdout();
    }

    assemble(*input);

    if (previous_output != nullptr)
    {
        std::cout.rdbuf(previous_output);
    }

    return 0;
}
