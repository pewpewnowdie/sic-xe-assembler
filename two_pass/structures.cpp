#include<iostream>
#include<vector>
#include<string>
#include<unordered_map>
#include<stdexcept>

#define GARBAGE 4294967295

struct SymData {
    uint32_t address;
};

class Optab {
    std::unordered_map<std::string, std::pair<uint8_t, int8_t>> OPTAB;
public:
    Optab() {
        OPTAB = {
            {"ADD", {0x18, 3}}, {"ADDF", {0x58, 3}}, {"ADDR", {0x90, 2}}, {"AND", {0x40, 3}}, {"CLEAR", {0xB4, 2}},
            {"COMP", {0x28, 3}}, {"COMPF", {0x88, 3}}, {"COMPR", {0xA0, 2}}, {"DIV", {0x24, 3}}, {"DIVF", {0x64, 3}},
            {"DIVR", {0x9C, 2}}, {"FIX", {0xC4, 1}}, {"FLOAT", {0xC0, 1}}, {"HIO", {0xF4, 1}}, {"J", {0x3C, 3}},
            {"JEQ", {0x30, 3}}, {"JGT", {0x34, 3}}, {"JLT", {0x38, 3}}, {"JSUB", {0x48, 3}}, {"LDA", {0x00, 3}},
            {"LDB", {0x68, 3}}, {"LDCH", {0x50, 3}}, {"LDF", {0x70, 3}}, {"LDL", {0x08, 3}}, {"LDS", {0x6C, 3}},
            {"LDT", {0x74, 3}}, {"LDX", {0x04, 3}}, {"LPS", {0xD0, 3}}, {"MUL", {0x20, 3}}, {"MULF", {0x60, 3}},
            {"MULR", {0x98, 2}}, {"NORM", {0xC8, 1}}, {"OR", {0x44, 3}}, {"RD", {0xD8, 3}}, {"RMO", {0xAC, 2}},
            {"RSUB", {0x4C, 3}}, {"SHIFTL", {0xA4, 2}}, {"SHIFTR", {0xA8, 2}}, {"SIO", {0xF0, 1}}, {"SSK", {0xEC, 3}},
            {"STA", {0x0C, 3}}, {"STB", {0x78, 3}}, {"STCH", {0x54, 3}}, {"STF", {0x80, 3}}, {"STI", {0xD4, 3}},
            {"STL", {0x14, 3}}, {"STS", {0x7C, 3}}, {"STSW", {0xE8, 3}}, {"STT", {0x84, 3}}, {"STX", {0x10, 3}},
            {"SUB", {0x1C, 3}}, {"SUBF", {0x5C, 3}}, {"SUBR", {0x94, 2}}, {"SVC", {0xB0, 2}}, {"TD", {0xE0, 3}},
            {"TIO", {0xF8, 1}}, {"TIX", {0x2C, 3}}, {"TIXR", {0xB8, 2}}, {"WD", {0xDC, 3}}
        };
    }

    // returns machine code, {0x01, 0} if invalid
    std::pair<uint8_t, uint8_t> lookup(const std::string &instruction) {
        if(OPTAB.find(instruction) == OPTAB.end()) return {0x01, 0};
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

    // use when symbol is being declared
    // returns true if successful
    bool writeSymbol(const std::string &symbol, const uint32_t address) {
        if(present(symbol)) {
            if(lookup(symbol).address != GARBAGE) throw std::invalid_argument("Symtab.writeSymbol : Redefinition of symbol is not allowed");
            return false;
        }
        SYMTAB[symbol] = {address};
        return true;
    }

    // use when symbol is an argument and not a declaration
    uint32_t getAddress(const std::string &symbol, const uint32_t callerAddress) {
        if(!present(symbol)) throw std::invalid_argument("Symtab.getAddress : No such symbol present in SYMTAB");
        return lookup(symbol).address;
    }

    void print() {
        std::cout << "SYMTAB:" << std::endl;
        for(auto &i : SYMTAB) {
            std::cout << i.first << " " << i.second.address << std::endl;
        }
    }
};