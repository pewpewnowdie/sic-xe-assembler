#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<fstream>

class Reader {
public:
    struct Instruction {
        std::string label;
        char prefix;
        std::string operation;
        std::vector<std::string> operands;
    };

    uint32_t wordToNum(std::string word) {
        if(word[0] == '#') {
            return std::stoi(word.substr(1));
        }
        int len = word.size(); 
        int base = 1; 
        uint32_t dec_val = 0; 
        for (int i = len - 1; i >= 0; i--) { 
            if (word[i] >= '0' && word[i] <= '9') { 
                dec_val += (int(word[i]) - 48) * base; 
                base = base * 16; 
            } else if (word[i] >= 'A' && word[i] <= 'F') { 
                dec_val += (int(word[i]) - 55) * base; 
                base = base * 16; 
            } 
        } 
        return dec_val;
    }

    std::string removeExtras(const std::string& str) {
        std::string res = "";
        for(auto c : str) {
            if(c == ';' || c == '/') return res;
            if(res.back() == ' ' && c == ' ') continue;
            res.push_back(c);
        }
        return res;
    }

    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(' ');
        if (std::string::npos == first) {
            return str;
        }
        size_t last = str.find_last_not_of(' ');
        return removeExtras(str.substr(first, (last - first + 1)));
    }

    bool addressBoundCheck(uint32_t address) {
        return address <= (1 << 20);
    }

    bool valueBoundCheck(uint16_t value) {
        return value <= (1 << 15);
    }

    Instruction inputSplit(std::string instruction) {
        Instruction result;
        instruction = trim(instruction);
        std::istringstream iss(instruction);
        std::string firstPart;
        getline(iss, firstPart, ' ');
        if (firstPart.back() == ':') {
            result.label = firstPart.substr(0, firstPart.size() - 1);
            getline(iss, result.operation, ' ');
        } else {
            result.operation = firstPart;
        }
        if(result.operation[0] == '+' || result.operation[0] == '@') {
            result.prefix = result.operation[0];
            result.operation = result.operation.substr(1);
        } else {
            result.prefix = ' ';
        }
        std::string operand;
        while (getline(iss, operand, ',')) {
            result.operands.push_back(trim(operand));
        }
        return result;
    }

    // std::ifstream loadFile(const std::string &path) {
    //     std::ifstream file(path);
    //     if (!file.is_open()) {
    //         throw std::invalid_argument("Reader.loadFile : File not found");
    //     }
    //     return file;
    // }   

    // std::vector<Instruction> readFile(std::ifstream &file) {
    //     std::vector<Instruction> instructions;
    //     std::string line;
    //     while (std::getline(file, line)) {
    //         if (!line.empty()) {
    //             instructions.push_back(splitInstruction(line));
    //         }
    //     }
    //     return instructions;
    // }

    // std::vector<Instruction> readFile(const std::string &path) {
    //     std::ifstream file(path);
    //     if (!file) {
    //         std::cerr << "Error opening file: " << path << std::endl;
    //         exit(1);
    //     }

    //     std::vector<Instruction> instructions;
    //     std::string line;
    //     while (std::getline(file, line)) {
    //         if (!line.empty()) {
    //             instructions.push_back(splitInstruction(line));
    //         }
    //     }
    //     file.close();
    //     return instructions;
    // }
};