#ifndef VOLEMACHINE_H 
#define VOLEMACHINE_H
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>
using namespace std;
class Register {
public:
    vector<int> memory;
    static const int size = 16;

    Register(){
        memory.resize(size, 0);
    }
    int getCell(int index) const{
     return memory[index];
        
    }
    void setCell(int index, int val){
        memory[index] = val &0xFF; 
    }
};
class Memory {
public:
    vector<string> memory;
    static const int size = 256;
    Memory() {
       memory.resize(size, "0000");
    }
    string getCell(int address) const {
    return memory[address];
    }
    void setCell(int address, const string& val) {
       memory[address] = val;
    }
};

class ALU {
public:
    int hexToDec(const string& hex) {
        return stoi(hex, nullptr, 16);
        
    }
    string decToHex(int dec){
        stringstream ss;
        ss << setw(2) <<setfill('0') << hex << dec;
    return ss.str();
    }

    void add(int idx1, int idx2, int idx3, Register& reg) {
        int result = reg.getCell(idx1) +reg.getCell(idx2);
        reg.setCell(idx3, result);
}

};

class CU {
private:
    string decToHex(int dec){
        stringstream ss;
        ss << setw(2) << setfill('0') << hex << dec;
        return ss.str();
    }
public:
    void load(int idxReg, int intMem, Register& reg, Memory& mem){
        string memValue =mem.getCell(intMem);
        int value = stoi(memValue, nullptr, 16);
        reg.setCell(idxReg, value);
        }

    void store(int idxReg,int idxMem, Register& reg, Memory& mem) {
        mem.setCell(idxMem, decToHex(reg.getCell(idxReg)));
        
     }
    void move(int idxReg1, int idxReg2, Register& reg) {
        reg.setCell(idxReg2, reg.getCell(idxReg1));

    }
    void jump(int idxMem, int& PC){
        PC = idxMem;
    }

    void compareJump(int idxReg1, int idxReg2, int idxMem, Register& reg, int& PC){
        if (reg.getCell(idxReg1) == reg.getCell(idxReg2)){
            PC = idxMem;
        }
    }
    void halt(bool& halted) {
        halted = true;
    }


};

class CPU {
public:
    int programCounter;
    int instructionRegister;
    Register reg;
    ALU alu;
    CU cu;
    bool halted;
    CPU() : programCounter(0), instructionRegister(0), halted(false) {}

    void runNextStep(Memory& mem) {
        
        instructionRegister = stoi(mem.getCell(programCounter), nullptr, 16);
        execute(mem);
        if (!halted) programCounter++;
    }

    void execute(Memory& mem) {
        int opcode =(instructionRegister &0xF000) >>12;
        int regIdx =(instructionRegister &0x0F00) >> 8;
        int operand = instructionRegister & 0x00FF;
        switch (opcode){
            case 0x1:
                cu.load(regIdx, operand, reg, mem);
                break;
            case 0x2:
                reg.setCell(regIdx, operand);
                break;
            case 0x3:
                cu.store(regIdx, operand,reg, mem);
                break;
            
            case 0x4: 
                cu.move(regIdx, operand& 0x0F, reg);
                break;
            case 0x5: 
                alu.add(regIdx, (operand >> 4) & 0x0F, operand & 0x0F, reg);
               break;
            case 0xB:
                
               if ((operand & 0xF0) ==0x00){
                    cu.jump(operand, programCounter);
                } 
               else {
                    cu.compareJump(regIdx, operand & 0x0F, operand & 0xF0, reg, programCounter);
                }
                break;
            case 0xC:
                cu.halt(halted);
                break;
            default:
                cu.halt(halted);
                break;
        }
    }
};

class Machine {
public:
    CPU processor;
    Memory memory;
    void loadProgramFile(const vector<string>& instructions) {
        for (size_t i = 0; i <instructions.size()&& i <memory.size; ++i) {
            memory.setCell(i, instructions[i]);
        }
        
        processor.programCounter = 0;
    }

    void outputState() const{
        
        cout<<"\n--- Machine State ---\n";
        cout<<"Program Counter: " << processor.programCounter << "\n";
        cout <<"Instruction Register: " << hex << processor.instructionRegister << "\n";
        cout <<"Registers: ";
        
        for (const auto& reg :processor.reg.memory) {
            cout << setw(2) << setfill('0') << hex << reg << " ";
        }
        
        cout << "\nMemory first 16 cells: ";
        for (int i = 0; i < 16; ++i) {
            cout << memory.getCell(i) << " ";
        }
        cout << dec << endl;
    }
};

class MainUI {
public:
    Machine machine;
    void displayMenu() const {
        cout << "1. Choose Input Method (File or Instructions)\n";
        cout << "2. Display State\n";
        cout << "3. Exit\n";
    }
    void inputChoice(char choice) {
        switch (choice){
            case '1':
                getFileOrInstructions();
                break;
            
            case '2':
                machine.outputState();
                break;
            case '3':
                exit(0);
            default:
                cerr << "Invalid choice!\n";
    }
    }

    void getFileOrInstructions(){
        int choice;
        cout << "\nChoose input method:\n";
        cout << "1. Input from file\n";
        cout << "2. Input from instructions\n";
        cout << "Enter your choice (1 or 2): ";
        cin >> choice;

        if (choice == 1) {
            inputFileName();
        } 
        else if (choice == 2){
             vector<string> instructions;
            string instruction;

            cout << "Enter instructions (type 'C000' to end):" << endl;
            while (true) {
                cout << "Instruction: ";
                cin >> instruction;
                if (instruction == "C000") {
                    break;
                }
            if (instruction == "0000") {
                continue;
            }
            instructions.push_back(instruction);
        }
        machine.loadProgramFile(instructions);
    } else {
        cerr << "Invalid choice" << endl;
        return;
    }
    runProgram();
}

    void inputFileName() {
        string filename;
        cout << "Enter filename: ";
        cin >> filename;
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Failed to open file: " << filename << endl;
            return;
        }
        vector<string> instructions;
        string instruction;
        
        while (getline(file, instruction)){
            if (!instruction.empty()) {
                instructions.push_back(instruction);
        }
        }
        file.close();
        machine.loadProgramFile(instructions);
    }
    void runProgram() {
        while (!machine.processor.halted) {
            machine.processor.runNextStep(machine.memory);
        }
    }
};

#endif
