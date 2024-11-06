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
        std::cout << "PassOne called" << std::endl;
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
                    std::cerr << "PassOne.initiatePassOne : " << e.what() << std::endl;
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
                    std::cerr << "PassOne.initatePassOne : " << e.what() << std::endl;
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
        intermediateFile.close();
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
        std::cout << "PassTwo called" << std::endl;
        objectFile.open("object.txt");
        if(!objectFile.is_open()) {
            std::cerr << "Error opening object file!" << std::endl;
            exit(0);
        }
    }
    ~PassTwo() {
        std::cout << "Assembling completed" << std::endl;
        if(objectFile.is_open()) objectFile.close();
    }
    int initiatePassTwo() {
        printSymtab();
        std::ifstream file("intermediate.txt");
        if(!file.is_open()) {
            std::cerr << "Error opening intermediate file!" << std::endl;
            exit(0);
        }
        std::string line;
        Reader::Instruction instruction;
        getline(file, line);
        instruction = reader.inputSplit(line);
        std::string progStart = reader.decToHex(start);
        std::string progLength = reader.decToHex(length);
        if(progStart.size() < 6) progStart = std::string(6 - progStart.size(), '0') + progStart;
        if(progLength.size() < 6) progLength = std::string(6 - progLength.size(), '0') + progLength;
        if(instruction.label != "") {
            objectFile << "H^" << instruction.label << "^" << progStart << "^" << progLength << "\n";
        } else {
            objectFile << "H^" << progStart << "^" << progLength <<"\n";
        }
        getline(file, line);
        Reader::Instruction nextInstruction = reader.intermediateSplit(line);
        // if(nextInstruction.operation == "END") {
        //     std::cerr << "PassTwo.initiatePassTwo : No instructions to assemble" << std::endl;
        //     objectFile << "E^" << reader.decToHex(start) << '\n';
        //     return 0;
        // }
        std::string record = "";
        locctr = start;
        uint32_t recordAddress = locctr;
        int recordSize = 0;

        while(getline(file, line)) {
            instruction = nextInstruction;
            nextInstruction = reader.intermediateSplit(line);
            locctr = nextInstruction.address;
            auto opcodeLookup = optab.lookup(instruction.operation);
            uint8_t opcode = opcodeLookup.first;

            if(opcode == 0x01) {    // assembler directive
                // write code for assembler directives
                std::cerr << "PassTwo.initiatePassTwo : Assembler directives not supported" << std::endl;
                continue;
            }

            // setting flags
            uint8_t flag = 0;

            std::cout << "Instruction: " << locctr << " " << instruction.operation << " " << std::hex << unsigned(opcode) << std::endl;

            if(locctr - instruction.address == 1) {     // format 1
                if(instruction.operands.size()) {
                    std::cerr << "PassTwo.initiatePassTwo : Incorrect arguments for format 2 instruction" << std::endl;
                    exit(1);
                }
                if(recordSize > 14) {
                    objectFile << "T^" << reader.decToHex(recordAddress) << "^" << reader.decToHex(recordSize) << record << "\n";
                    record = "";
                    recordAddress = locctr;
                    recordSize = 0;
                }
                if(opcode / 16 == 0) {
                    record += "^0" + reader.decToHex(opcode);
                } else {
                    record += "^" + reader.decToHex(opcode);
                }
                recordSize += 1;
                locctr = nextInstruction.address;
            } else if(locctr - instruction.address == 2) {  // format 2
                if(recordSize > 13) {
                    objectFile << "T^" << reader.decToHex(recordAddress) << "^" << reader.decToHex(recordSize) << record << "\n";
                    record = "";
                    recordAddress = locctr;
                    recordSize = 0;
                }
                char reg1 = reader.getRegisterNumber(instruction.operands[0]);
                char reg2 = reader.getRegisterNumber(instruction.operands[1]);
                if(opcode / 16 == 0) {
                    record += "^0" + reader.decToHex(opcode) + reg1 + reg2;
                } else {
                    record += "^" + reader.decToHex(opcode) + reg1 + reg2;
                }
                recordSize += 2;
                locctr = nextInstruction.address;
            } else if(locctr - instruction.address == 3) {  // format 3
                if(recordSize > 12) {
                    objectFile << "T^" << reader.decToHex(recordAddress) << "^" << reader.decToHex(recordSize) << record << "\n";
                    record = "";
                    recordAddress = locctr;
                    recordSize = 0;
                }
                // setting flags
                if(((instruction.operands.size() > 0) && (instruction.operands[0] == "X" || instruction.operands[0] == "x")) ||
                   ((instruction.operands.size() > 1) && (instruction.operands[1] == "X" || instruction.operands[1] == "x")))
                    flag |= (1 << 4);
                if(instruction.operands.size() > 0 && !instruction.operands[0].empty() && instruction.operands[0][0] == '#')
                    flag |= (1 << 5);
                else if(instruction.operands.size() > 0 && !instruction.operands[0].empty() && instruction.operands[0][0] == '@')
                    flag |= (1 << 6);
                else
                    flag |= (1 << 2);
                uint32_t objectCode = (opcode << 4) | flag;
                objectCode <<= 12;
                if(instruction.operands.size() > 0 && !instruction.operands[0].empty() && instruction.operands[0][0] == '#') {
                    uint16_t disp = reader.wordToNum(instruction.operands[0].substr(1));
                    objectCode |= disp;
                } else if(instruction.operands.size() > 0 && !instruction.operands[0].empty()) {
                    try {
                        uint16_t disp = symtab.getAddress(instruction.operands[0]) - locctr;
                        objectCode |= disp;
                    } catch(const std::out_of_range &e) {
                        std::cerr << "PassTwo.initiatePassTwo : incorrect format" << std::endl;
                        exit(1);
                    } catch(const std::invalid_argument &e) {
                        std::cerr << "PassTwo.initiatePassTwo : " << e.what() << std::endl;
                        exit(1);
                    }
                } else {
                    std::cerr << "PassTwo.initiatePassTwo : missing operand" << std::endl;
                    exit(1);
                }
                std::string currCode = reader.decToHex(objectCode);
                if(currCode.size() < 6) {
                    currCode = std::string(6 - currCode.size(), '0') + currCode;
                }
                record += "^" + currCode;
                recordSize += 3;
                locctr = nextInstruction.address;
            } else if(locctr - instruction.address == 4) {  // format 4
                if(recordSize > 11) {
                    objectFile << "T^" << reader.decToHex(recordAddress) << "^" << reader.decToHex(recordSize) << record << "\n";
                    record = "";
                    recordAddress = locctr;
                    recordSize = 0;
                }
                // setting flags
                flag = 1; // e = 1
                if((instruction.operands.size() > 0 && (instruction.operands[0] == "X" || instruction.operands[0] == "x")) ||
                   (instruction.operands.size() > 1 && (instruction.operands[1] == "X" || instruction.operands[1] == "x")))
                    flag |= (1 << 4);
            
                if(instruction.operands.size() > 0 && !instruction.operands[0].empty() && instruction.operands[0][0] == '#')
                    flag |= (1 << 5);
                else if(instruction.operands.size() > 0 && !instruction.operands[0].empty() && instruction.operands[0][0] == '@')
                    flag |= (1 << 6);
                else
                    flag |= (1 << 2);
            
                uint32_t objectCode = (opcode << 4) | flag;
                objectCode <<= 20;
            
                if(instruction.operands.size() > 0 && !instruction.operands[0].empty() && instruction.operands[0][0] == '#') {
                    uint32_t disp = reader.wordToNum(instruction.operands[0].substr(1));
                    objectCode |= disp;
                } else if(instruction.operands.size() > 0 && !instruction.operands[0].empty() && instruction.operands[0][0] == '@') {
                    try {
                        uint32_t disp = symtab.getAddress(instruction.operands[0].substr(1)) - locctr;
                        objectCode |= disp;
                    } catch(const std::out_of_range &e) {
                        std::cerr << "PassTwo.initiatePassTwo : incorrect format" << std::endl;
                        exit(1);
                    } catch(const std::invalid_argument &e) {
                        std::cerr << "PassTwo.initiatePassTwo : " << e.what() << std::endl;
                        exit(1);
                    }
                } else if(instruction.operands.size() > 0) {
                    uint32_t disp = symtab.getAddress(instruction.operands[0]) - locctr;
                    objectCode |= disp;
                } else {
                    std::cerr << "PassTwo.initiatePassTwo : missing operand" << std::endl;
                    exit(1);
                }
                record += "^" +  reader.decToHex(objectCode);
                locctr = nextInstruction.address;
                recordSize += 4;
            }
            if(nextInstruction.operation == "END") {
                std::cout << "End of file" << std::endl;
                break;
            }
        }
        if(record.size()) {
            objectFile << "T^" << reader.decToHex(recordAddress) << "^" << reader.decToHex(recordSize) << record << "\n";
        }
        objectFile << "E^" << reader.decToHex(start) << '\n';
        if(file.is_open()) file.close();
        if(objectFile.is_open()) objectFile.close();
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
    // PassOne passOne;
    // passOne.initiatePassOne(path);
    // passOne.printSymtab();
    return 0;
}