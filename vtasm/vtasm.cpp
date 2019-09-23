#include <cassert>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <string>

#include "vtasm.h"

namespace
{

struct mnemonic
{
    char prefix;
    const char *mnemonic;
};

class assembler
{
public:
    assembler(std::istream &str) : m_str(str) {}

    void assemble();

private:
    void assemble(const std::string &line);
    void assemble_string(const std::string &line);
    void assemble_control(const std::string &line);
    void assemble_esc(const std::string &command, const std::string &args);
    std::string get();
    bool eof();

    std::istream &m_str;
    int m_line_number;
};

bool assembler::eof()
{
    return !m_str;
}

std::string assembler::get()
{
    ++m_line_number;
    std::string line;
    std::getline(m_str, line);
    return line;
}

void assembler::assemble()
{
    while (!eof())
    {
        assemble(get());
    }
}

void assembler::assemble_string(const std::string &line)
{
    if (line.back() != '"')
    {
        std::cerr << "Missing close quote on line " << m_line_number << ": " << line << '\n';
        return;
    }

    std::cout << line.substr(1, line.size()-2);
}

void assembler::assemble(const std::string &line)
{
    if (line.empty() || line[0] == '#')
    {
        return;
    }

    if (line[0] == '"')
    {
        assemble_string(line);
    }
    else
    {
        assemble_control(line);
    }
}

void assembler::assemble_esc(const std::string &command, const std::string &args)
{
    static const mnemonic simple_commands[] =
    {
        { '(', "scsg0" },
        { ')', "scsg1" },
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
        { 'Z', "decid" },
        { 'c', "ris" }
    };
    for (const auto &entry : simple_commands)
    {
        if (command == entry.mnemonic)
        {
            std::cout << '\033' << entry.prefix << args;
            return;
        }
    }

    static const mnemonic hash_commands[] =
    {
        { '3', "decdhltop" },
        { '4', "decdhlbot" },
        { '5', "decswl" },
        { '6', "decdwl" },
        { '7', "dechcp" },
        { '8', "decaln" }
    };
    for (const auto &entry : hash_commands)
    {
        if (command == entry.mnemonic)
        {
            std::cout << "\033#" << entry.prefix;
            return;
        }
    }

    static const mnemonic csi_commands[] =
    {
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
    for (const auto &entry : csi_commands)
    {
        if (command == entry.mnemonic)
        {
            std::cout << "\033[" << args << entry.prefix;
            return;
        }
    }

    std::cerr << "Unknown command on line " << m_line_number << ": " << command << " \"" << args << "\"\n";
}

void assembler::assemble_control(const std::string &line)
{
    std::string command;
    std::string args;
    if (auto pos = line.find(' '); pos != std::string::npos)
    {
        command = line.substr(0, pos);
        args = line.substr(pos + 1);
        args = args.substr(1, args.size()-2);
    }
    else
    {
        command = line;
    }

    static const mnemonic mnemonics[] =
    {
        { '\000', "nul" }, { '\001', "soh" }, { '\002', "stx" }, { '\003', "etx" },
        { '\004', "eot" }, { '\005', "enq" }, { '\006', "ack" }, { '\007', "bel" },
        { '\010', "bs" }, { '\011', "ht" }, { '\012', "lf" }, { '\013', "vt" },
        { '\014', "ff" }, { '\015', "cr" }, { '\016', "so" }, { '\017', "si" },
        { '\020', "dle" }, { '\021', "dc1" }, { '\022', "dc2" }, { '\023', "dc3" },
        { '\024', "dc4" }, { '\025', "naq" }, { '\026', "syn" }, { '\027', "etb" },
        { '\030', "can" }, { '\031', "em" }, { '\032', "sub" }, { '\033', "esc" },
        { '\034', "is4" }, { '\035', "is3" }, { '\036', "is2" }, { '\037', "is1" },
        { '\177', "del" }
    };
    for (auto item : mnemonics)
    {
        if (command == item.mnemonic)
        {
            std::cout << item.prefix;
            return;
        }
    }

    return assemble_esc(command, args);
}

} // namespace

void assemble(std::istream &str)
{
    assembler(str).assemble();
}
