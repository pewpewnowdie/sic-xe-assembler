#include<iostream>
#include<vector>
#include<string>
#include<unordered_map>
#include<stdexcept>

struct SymData {
    uint32_t address;
    bool flag;
};

class Optab {
    std::unordered_map<std::string, uint8_t> OPTAB;
public:
    Optab() {
        OPTAB = {
            {"ADD", 0x18}, {"ADDF", 0x58}, {"ADDR", 0x90}, {"AND", 0x40}, {"CLEAR", 0xB4},
            {"COMP", 0x28}, {"COMPF", 0x88}, {"COMPR", 0xA0}, {"DIV", 0x24}, {"DIVF", 0x64},
            {"DIVR", 0x9C}, {"FIX", 0xC4}, {"FLOAT", 0xC0}, {"HIO", 0xF4}, {"J", 0x3C},
            {"JEQ", 0x30}, {"JGT", 0x34}, {"JLT", 0x38}, {"JSUB", 0x48}, {"LDA", 0x00},
            {"LDB", 0x68}, {"LDCH", 0x50}, {"LDF", 0x70}, {"LDL", 0x08}, {"LDS", 0x6C},
            {"LDT", 0x74}, {"LDX", 0x04}, {"LPS", 0xD0}, {"MUL", 0x20}, {"MULF", 0x60},
            {"MULR", 0x98}, {"NORM", 0xC8}, {"OR", 0x44}, {"RD", 0xD8}, {"RMO", 0xAC},
            {"RSUB", 0x4C}, {"SHIFTL", 0xA4}, {"SHIFTR", 0xA8}, {"SIO", 0xF0}, {"SSK", 0xEC},
            {"STA", 0x0C}, {"STB", 0x78}, {"STCH", 0x54}, {"STF", 0x80}, {"STI", 0xD4},
            {"STL", 0x14}, {"STS", 0x7C}, {"STSW", 0xE8}, {"STT", 0x84}, {"STX", 0x10},
            {"SUB", 0x1C}, {"SUBF", 0x5C}, {"SUBR", 0x94}, {"SVC", 0xB0}, {"TD", 0xE0},
            {"TIO", 0xF8}, {"TIX", 0x2C}, {"TIXR", 0xB8}, {"WD", 0xDC}
        };
    }
    uint8_t lookup(const std::string &instruction) {
        if(OPTAB.find(instruction) == OPTAB.end()) return 0x01;
        return OPTAB[instruction];
    }
};

class Symtab {
    std::unordered_map<std::string, SymData> SYMTAB;
public:
    bool present(const std::string &symbol) {
        if(SYMTAB.find(symbol) == SYMTAB.end()) return false;
        return true;
    }
    SymData lookup(const std::string &symbol) {
        if(!present(symbol)) throw std::invalid_argument("Symtab.lookup : Symbol is not present in SYMTAB");
        return SYMTAB[symbol];
    }
    void write(Optab *optab, const std::string &symbol, const uint32_t address, bool flag = false) {
        if(present(symbol)) throw std::invalid_argument("Symtab.write : Symbol is already present in SYMTAB");
        SYMTAB[symbol] = {address, flag};
    }
};

uint32_t LOCCTR;