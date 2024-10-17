#include<iostream>
#include<string>
#include<vector>
#include<sstream>

class Reader {
public:
    struct Instruction {
        std::string label;
        std::string operation;
        std::vector<std::string> operands;
    };

    uint32_t hexToDec(std::string hexVal) { 
        int len = hexVal.size(); 
        int base = 1; 
        uint32_t dec_val = 0; 
        for (int i = len - 1; i >= 0; i--) { 
            if (hexVal[i] >= '0' && hexVal[i] <= '9') { 
                dec_val += (int(hexVal[i]) - 48) * base; 
                base = base * 16; 
            } 
            else if (hexVal[i] >= 'A' && hexVal[i] <= 'F') { 
                dec_val += (int(hexVal[i]) - 55) * base; 
                base = base * 16; 
            } 
        } 
        return dec_val; 
    }

    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(' ');
        if (std::string::npos == first) {
            return str;
        }
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }

    Instruction splitIns(const std::string& instruction) {
        Instruction result;
        std::istringstream iss(instruction);
        std::string firstPart;
        getline(iss, firstPart, ' ');
        if (firstPart.back() == ':') {
            result.label = firstPart.substr(0, firstPart.size() - 1);
            getline(iss, result.operation, ' ');
        } else {
            result.operation = firstPart; 
        }
        std::string operand;
        while (getline(iss, operand, ',')) {
            result.operands.push_back(trim(operand));
        }
        return result;
    }
};