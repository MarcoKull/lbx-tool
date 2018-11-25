#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#ifdef WITH_QT
#include "gui/Gui.h"
#endif

#include <lbx.h>

char** args;
int argsc;

void help() {
    std::cout
            << "usage: lbx-tool <option> [parameter]\n\n"
            << "   option                    parameter            description\n"
            << " --------------------------------------------------------------------------------------------------\n"
            << "  -a  | --add               <lbx> <file>         Add a <file> to <lbx>.\n"
            << "  -c  | --create            <path>               Create a new empty lbx file at <path>.\n"
            << "  -d  | --delete            <lbx> <nr>           Delete file at index <nr> in <lbx>.\n"
            << "  -f  | --files             <lbx>                Print number of files contained in <lbx>.\n"
            << "  -h  | --help                                   Show this help.\n"
            << "  -l  | --list              <lbx>                List contents of <lbx>.\n"
            << "  -p  | --print             <lbx>                Print a technical overview of <lbx>.\n"
            << "  -r  | --replace           <lbx> <nr> <file>    Replace file at index <nr> with <file> in <lbx>.\n"
            << "  -u0 | --unknown0          <lbx> <nr>           Set first unknown area of <lbx> to <nr>.\n"
            << "  -u1 | --unknown1          <lbx> <file>         Set second unknown area of <lbx> to <file>.\n"
            << "  -x  | --extract           <lbx> <nr> <path>    Extract file with index <nr> from <lbx> to <path>.\n"
            << "  -xu | --extract-unknown   <lbx> <path>         Extract second unknown area of <lbx> to <path>.\n";
}

void argError(std::string msg) {
    throw std::runtime_error("invalid command line - " + msg);
}

uint16_t str2uint16(std::string str) {
    char* v = (char*) str.c_str();
    errno = 0;
    char* end = v;
    long unsigned result = strtoul(v, &end, 10);
    if (errno == ERANGE || *end) {
        argError("'" + str + "' is not a valid number");
    }

    if (result > 65535) {
        argError("'" + str + "' is not a number between 0 and 65535");
    }

    return (uint16_t) result;
}

void checkArg(size_t ac) {
    if (argsc < (int) ac) {
        argError("too few arguments for '" + std::string(args[1]) + "'");
    } else if (argsc > (int) ac) {
        argError("too many arguments for '" + std::string(args[1]) + "'");
    }
}

std::pair<char*, uint32_t> fileRead(std::string path) {
    std::ifstream file(path, std::ifstream::binary | std::ifstream::ate);
    if (!file.good()) {
        throw std::runtime_error("could not open '" + path + "'");
    }

    std::pair<char*, uint32_t> p;
    p.second = (uint32_t) file.tellg();
    p.first = new char[p.second];
    file.seekg(0, std::ifstream::beg);
    file.read(p.first, p.second);

    return p;
}

void fileWrite(std::pair<char*, uint32_t>& content, std::string path) {
    std::ofstream o(path, std::ifstream::binary);
    if (!o.good()) {
        throw std::runtime_error("could not write to '" + path + "'");
    }
    o.write(content.first, content.second);
}

void add() {
    checkArg(4);
    LbxFile lbx(args[2]);
    lbx.add(fileRead(args[3]));
    lbx.save();
}

void create() {
    checkArg(3);
    LbxFile lbx;
    lbx.save();
}

void del() {
    checkArg(4);
    LbxFile lbx(args[2]);
    lbx.remove(str2uint16(args[3]));
    lbx.save();
}

void files() {
    checkArg(3);
    LbxFile lbx(args[2]);
    std::cout << lbx.size() << "\n";
}

void list() {
    checkArg(3);
    LbxFile lbx(args[2]);
    std::cout << "File '" << args[2] << "' contains " << lbx.size() << " files:\n";
    for (uint16_t i = 0; i < lbx.size(); ++i) {
        std::cout << std::setw(4) << i + 1 << " - " << lbx[i].second << " byte\n";
    }
}

void replace() {
    checkArg(5);
    LbxFile lbx(args[2]);
    uint16_t i = str2uint16(args[4]);
    delete[] lbx[i].first;
    lbx[i] = fileRead(args[3]);
    lbx.save();
}

void unknown0() {
    checkArg(4);
    LbxFile lbx(args[2]);
    lbx.unknown0() = str2uint16(args[3]);
    lbx.save();
}

void unknown1() {
    checkArg(4);
    LbxFile lbx(args[2]);
    std::pair<char*, uint32_t> p = fileRead(args[3]);
    delete[] lbx.unknown1().first;
    lbx.unknown1() = p;
    lbx.save();
}

void extract() {
    checkArg(5);
    LbxFile lbx(args[2]);
    uint16_t index = str2uint16(args[3]);
    if (index < 0 || index >= lbx.size()) {
        argError("invalid index '" + std::string(args[3]) + "'");
    }
    fileWrite(lbx[index], args[4]);
}

void extractUnknown() {
    checkArg(4);
    LbxFile lbx(args[2]);
    fileWrite(lbx.unknown1(), args[3]);
}

#define LBXTOOL_PRINT_LINE(x, y) std::cout << std::setw(10) << x << " - " << y << "\n";
#define LBXTOOL_PRINT_SEP(x) {\
    std::cout << " --------------------------------------- " << count << "\n";\
    count += x;\
}

void print() {
    checkArg(3);
    LbxFile lbx(args[2]);
    uint32_t count = 0;
    std::cout << "  file '" << args[2] << "':\n\n";
    LBXTOOL_PRINT_SEP(2);
    LBXTOOL_PRINT_LINE(lbx.size(), "number of files");
    LBXTOOL_PRINT_SEP(4);
    LBXTOOL_PRINT_LINE(LbxFile::SIGNATURE, "signature");
    LBXTOOL_PRINT_SEP(2);
    LBXTOOL_PRINT_LINE(lbx.unknown0(), "unknown area of two byte");
    for (uint16_t i = 0; i <= lbx.size(); ++i) {
        LBXTOOL_PRINT_SEP(4);
        LBXTOOL_PRINT_LINE(lbx.offset(i), "offset " << i);
    }
    if (lbx.unknown1().second > 0) {
        LBXTOOL_PRINT_SEP(lbx.unknown1().second);
        LBXTOOL_PRINT_LINE("?", "unknown area of " << lbx.unknown1().second << " byte");
    }

    for (uint16_t i = 0; i < lbx.size(); ++i) {
        LBXTOOL_PRINT_SEP(lbx[i].second);
        std::stringstream s;
        s << "data[" << i << "]";
        LBXTOOL_PRINT_LINE(s.str(), "file data of " << lbx[i].second << " byte");
    }
    LBXTOOL_PRINT_SEP(0) // print last seperator
}

void tool() {
    if (argsc < 2) {
        argError("too few arguments");
    }

    std::string o = args[1];
    if (o == "-a" || o == "--add") {
        add();
    } else if (o == "-c" || o == "--create") {
        create();
    } else if (o == "-d" || o == "--delete") {
        del();
    } else if (o == "-f" || o == "--files") {
        files();
    } else if (o == "-h" || o == "--help") {
        help();
    } else if (o == "-l" || o == "--list") {
        list();
    } else if (o == "-p" || o == "--print") {
        print();
    } else if (o == "-r" || o == "--replace") {
        replace();
    } else if (o == "-u0" || o == "--unknown0") {
        unknown0();
    } else if (o == "-u1" || o == "--unknown1") {
        unknown1();
    } else if (o == "-x" || o == "--extract") {
        extract();
    } else if (o == "-xu" || o == "--extract-unknown") {
        extractUnknown();
    } else {
        argError("unknown parameter '" + o + "'");
    }
}

int main(int argc, char** argv) {
    args = argv;
    argsc = argc;

    try {
        tool();
    } catch (std::exception& e) {

#ifdef WITH_QT
        if (argc < 3) {
            // open gui window
            if (argc == 2) {
                return LbxToolGui::show(argv[1]);
            } else {
                return LbxToolGui::show();
            }
        }
#endif /* WITH_QT */

        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

