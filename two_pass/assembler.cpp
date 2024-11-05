#include<iostream>
#include<vector>
#include<string>
#include<unordered_map>
#include<stdexcept>
#include<algorithm>

#include "structures.cpp"
#include "reader.cpp"

class PassOne {
protected:
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
    int initiatePassOne(std::string path) {
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
            if(instruction.operands.size() > 2) {
                std::cerr << "PassOne.initiatePassOne : Operand overflow" << std::endl;
                exit(1);
            }
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
                std::cerr << "PassOne.initiatePassOne : Invalid operation: " << instruction.operation << std::endl;
                file.close();
                return 1;
            }
        }
        file.close();
        return 0;
    }

    void printSymtab() {
        symtab.print();
    }
};

class PassTwo : public PassOne {
    std::ofstream objectFile;
public:
    PassTwo() {
        objectFile.open("object.txt");
        if(!objectFile.is_open()) {
            std::cerr << "Error opening object file!" << std::endl;
            exit(0);
        }
    }
    ~PassTwo() {
        if(objectFile.is_open()) objectFile.close();
    }
    int initiatePassTwo() {
        std::ifstream file("intermediate.txt");
        if(!file.is_open()) {
            std::cerr << "Error opening intermediate file!" << std::endl;
            exit(0);
        }
        std::string line;
        Reader::Instruction instruction;
        std::getline(file, line);
        if(line[0] >= '0' && line[0] <= '9') {
            objectFile << "H^PROG^0000^" << reader.decToHex(length) <<"\n";
        } else {
            instruction = reader.inputSplit(line);
            if(instruction.label != "") {
                objectFile << "H^START^" << instruction.label << "^" << reader.decToHex(start) << "^" << reader.decToHex(length) <<"\n";
            } else {
                objectFile << "H^START^" << reader.decToHex(start) << "^" << reader.decToHex(length) <<"\n";
            }
            std::getline(file, line);
        }
        Reader::Instruction nextInstruction = reader.intermediateSplit(line);
        if(nextInstruction.operation == "END") {
            objectFile << "E^" << reader.decToHex(start) << '\n';
            return 0;
        }
        locctr = nextInstruction.address;
        std::stringstream record;
        uint32_t recordAddress = locctr;
        do {
            instruction = nextInstruction;
            if(!getline(file, line)) return 1;
            nextInstruction = reader.intermediateSplit(line);
            locctr = nextInstruction.address;
            if(nextInstruction.operation == "END") {
                return 0;
            }
            auto opcodeLookup = optab.lookup(instruction.operation);
            uint8_t opcode = opcodeLookup.first;

            if(opcode == 0x01) {    // assembler directive
                // write code for assembler directives
                continue;
            }

            // setting flags
            uint8_t flag = 0;

            if(locctr - instruction.address == 1) {     // format 1
                if(instruction.operands.size()) {
                    std::cerr << "PassTwo.initiatePassTwo : Incorrect arguments for format 2 instruction" << std::endl;
                    exit(1);
                }
                if(record.str().size() / 2 > 14) {
                    objectFile << "T^" << reader.decToHex(recordAddress) << "^" << reader.decToHex(record.str().size() / 2) << "^" << record.str() << "\n";
                    record.str("");
                    recordAddress = locctr;
                }
                if(opcode / 16 == 0) {
                    record << "0" << reader.decToHex(opcode);
                } else {
                    record << reader.decToHex(opcode);
                }
                locctr = nextInstruction.address;
            } else if(locctr - instruction.address == 2) {  // format 2
                if(record.str().size() / 2 > 13) {
                    objectFile << "T^" << reader.decToHex(recordAddress) << "^" << reader.decToHex(record.str().size() / 2) << "^" << record.str() << "\n";
                    record.str("");
                    recordAddress = locctr;
                }
                char reg1 = reader.getRegisterNumber(instruction.operands[0]);
                char reg2 = reader.getRegisterNumber(instruction.operands[1]);
                if(opcode / 16 == 0) {
                    record << "0" << reader.decToHex(opcode) << reg1 << reg2;
                } else {
                    record << reader.decToHex(opcode) << reg1 << reg2;
                }
                locctr = nextInstruction.address;
            } else if(locctr - instruction.address == 3) {  // format 3
                if(record.str().size() / 2 > 12) {
                    objectFile << "T^" << reader.decToHex(recordAddress) << "^" << reader.decToHex(record.str().size() / 2) << "^" << record.str() << "\n";
                    record.str("");
                    recordAddress = locctr;
                }
                // setting flags
                
            }
            // if(instruction.operands[0] == "X" || instruction.operands[0] == "x" || instruction.operands[1] == "X" || instruction.operands[1] == "x") flag | (1 << 3);
        } while(true);
        return 0;
    }
};

class Assembler : public PassTwo {
public:
    int initiate(std::string path) {
        return initiatePassOne(path) == 0 ? initiatePassTwo() : 1;
    }
};

int main(int argc, char *argv[]) {
    if(argc < 2) return 1;
    std::string path = argv[1];
    Assembler assembler;
    assembler.initiate(path);
    return 0;
}