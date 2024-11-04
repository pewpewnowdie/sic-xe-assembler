
#include<iostream>
#include<vector>
#include<string>
#include<unordered_map>
#include<stdexcept>
#include<algorithm>

#include "structures.cpp"
#include "reader.cpp"

class PassOne {
    Optab optab;
    Symtab symtab;
    uint32_t locctr;
    Reader reader;
    std::ofstream intermediateFile;
    uint32_t start;
    uint32_t length;
    std::string progName;
public:
    PassOne() {
        intermediateFile.open("intermediate.txt");
        if(!intermediateFile.is_open()) {
            std::cerr << "Error opening intermediate file!" << std::endl;
            exit(0);
        }
    }
    ~PassOne() {
        if(intermediateFile.is_open()) intermediateFile.close();
    }
    int initiate(std::string path) {
        std::ifstream file(path);
        if (!file) {
            std::cerr << "Error opening file: " << path << std::endl;
            exit(1);
        }
        std::string line;
        std::getline(file, line);
        while (line.empty()) std::getline(file, line);
        Reader::Instruction instruction = reader.inputSplit(line);
        if(instruction.operation != "START") {
            locctr = 0;
            start = 0;
        } else {
            locctr = reader.wordToNum(instruction.operands[0]);
            start = locctr;
            if(instruction.label != "") {
                progName = instruction.label;
                try {
                    symtab.writeSymbol(instruction.label, locctr);
                } catch(const std::invalid_argument &e) {
                    std::cerr << e.what() << std::endl;
                    file.close();
                    return 1;
                }
                intermediateFile << instruction.label << ": "<< "START" << " " << locctr << "\n";
            } else intermediateFile << "START" << " " << locctr << "\n";
        }
        while (std::getline(file, line) && !line.empty()) {
            Reader::Instruction instruction = reader.inputSplit(line);
            if(instruction.operation == "END") {
                if(instruction.label != "") {
                    try {
                        symtab.writeSymbol(instruction.label, locctr);
                    } catch(const std::invalid_argument &e) {
                        std::cerr << e.what() << std::endl;
                        file.close();
                        return 1;
                    }
                }
                if(instruction.label != "")
                    intermediateFile << locctr << " " << instruction.label << ": " << "END" << " " << progName << "\n";
                else
                    intermediateFile << locctr << " " << "END" << " " << progName << "\n";
                length = locctr - start;
                break;
            }
            if(instruction.label != "") {
                try {
                    symtab.writeSymbol(instruction.label, locctr);
                } catch(const std::invalid_argument &e) {
                    std::cerr << e.what() << std::endl;
                    file.close();
                    return 1;
                }
            }
            auto optabLookup = optab.lookup(instruction.operation);
            uint8_t opcode = optabLookup.first;
            uint8_t size = optabLookup.second;
            size = instruction.prefix == '+' ? 4 : size;
            if(opcode != 0x01) {
                if(instruction.label != "")
                    intermediateFile << locctr << " " << instruction.label << ": " << instruction.operation << " " << instruction.operands[0] << "\n";
                else
                    intermediateFile << locctr << " " << instruction.operation << " " << instruction.operands[0] << "\n";
                locctr += size;
            } else if(instruction.operation == "WORD") {
                if(instruction.label != "")
                    intermediateFile << locctr << " " << instruction.label << ": " << instruction.operation << " " << instruction.operands[0] << "\n";
                else
                    intermediateFile << locctr << " " << instruction.operation << " " << instruction.operands[0] << "\n";
                locctr += 3;
            } else if(instruction.operation == "RESW") {
                if(instruction.label != "")
                    intermediateFile << locctr << " " << instruction.label << ": " << instruction.operation << " " << instruction.operands[0] << "\n";
                else
                    intermediateFile << locctr << " " << instruction.operation << " " << instruction.operands[0] << "\n";
                locctr += 3 * reader.wordToNum(instruction.operands[0]);
            } else if(instruction.operation == "RESB") {
                if(instruction.label != "")
                    intermediateFile << locctr << " " << instruction.label << ": " << instruction.operation << " " << instruction.operands[0] << "\n";
                else
                    intermediateFile << locctr << " " << instruction.operation << " " << instruction.operands[0] << "\n";
                locctr += reader.wordToNum(instruction.operands[0]);
            } else if(instruction.operation == "BYTE") {
                if(instruction.label != "")
                    intermediateFile << locctr << " " << instruction.label << ": " << instruction.operation << " " << instruction.operands[0] << "\n";
                else
                    intermediateFile << locctr << " " << instruction.operation << " " << instruction.operands[0] << "\n";
                if(instruction.operands[0][0] == 'X' || instruction.operands[0][0] == 'x') {
                    locctr += (instruction.operands[0].size() - 3) / 2;
                } else if(instruction.operands[0][0] == 'C' || instruction.operands[0][0] == 'c') {
                    locctr += instruction.operands[0].size() - 3;
                }
            } else {
                std::cerr << "pass_one.initiate : Invalid operation: " << instruction.operation << std::endl;
                file.close();
                return 1;
            }
        }
        file.close();
    }
};

int main(int argc, char *argv[]) {
    if(argc < 2) return 1;
    std::string path = argv[1];
    PassOne passOne;
    passOne.initiate(path);
    return 0;
}