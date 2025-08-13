/**
 * Name: emulator.cpp
 * Author: Jesse Flores
 * Purpouse: The program was written with efforts to get a better understanidng
 *           of how a CPU works, how an assembler works, and how a compiler works.
 *           What follows is a simple CPU emulator with an assembler and a Micro-C
 *           to bytecode compiler. The emulator supports basic instructions, system
 *           calls, and a simple OS kernel. The assembler translates assembly language
 *           into bytecode, and the compiler translates a subset of C-like language
 *           (Micro-C) into bytecode. The emulator provides a command-line interface
 *           for loading, assembling, compiling, and executing programs.
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include <fstream>
#include <stdexcept>
#include <cctype>
#include <unordered_map>
#include <queue>

using namespace std;

// Define opcodes for a fictional CPU
enum OpCode : uint8_t {
    // Data Movement Instructions
    PUSH_B = 0x01,
    POP_B = 0x02,
    LOAD_A = 0x03,
    LOAD_B = 0x04,
    STORE_A = 0x05,
    // Arithmetic Instructions
    ADD_A_B = 0x10,
    SUB_A_B = 0x11,
    // Control Flow Instructions
    JMP = 0x20,
    HALT = 0xFF,
    // System Call Instruction (for OS)
    SYSCALL = 0x30
};

// Converter from string to OpCode
const map<string, OpCode> opcodeMap = {
    {"PUSH_B", PUSH_B},
    {"POP_B", POP_B},
    {"LOAD_A", LOAD_A},
    {"LOAD_B", LOAD_B},
    {"STORE_A", STORE_A},
    {"ADD_A_B", ADD_A_B},
    {"SUB_A_B", SUB_A_B},
    {"JMP", JMP},
    {"HALT", HALT},
    {"SYSCALL", SYSCALL}
};

// OS Kernel definitions
const uint16_t KERNEL_START_ADDRESS = 0x1000;
const uint16_t USER_PROGRAM_START_ADDRESS = 0x0000;
enum class SyscallNumber : uint8_t {
    PRINT_CHAR = 1,
    READ_CHAR = 2
};

class CPU {
public:
    uint8_t reg_A = 0;
    uint8_t reg_B = 0;
    uint16_t pc = 0;
    uint16_t sp = 0;
    bool privileged = false; // New: Privileged mode flag

    vector<uint8_t> memory;
    vector<uint8_t> stack;

    // Constructor
    CPU() {
        memory.resize(65536, 0);
        stack.resize(256, 0);
    }

    /**
     * Name: loadProgram
     * Purpouse: Load a program into memory at a specified start address.
     * Inputs:
     *   - program: A vector of bytes representing the program to load.
     *  - startAddress: The memory address where the program should be loaded.
     * Outputs: None
     * Effects: The program is copied into the CPU's memory starting at the specified address
     *         and an error message is printed if the program exceeds memory bounds.
     */
    void loadProgram(const vector<uint8_t>& program, uint16_t startAddress) {
        if (startAddress + program.size() > memory.size()) {
            cerr << "Error: Program too large for memory at address 0x" << hex << startAddress << dec << endl;
            return;
        }
        copy(program.begin(), program.end(), memory.begin() + startAddress);
    }

    /**
     * Name: syscallHandler
     * Purpouse: Handle system calls made by user programs.
     * Inputs: None (uses CPU registers)
     * Outputs: None (modifies CPU registers and may perform I/O)
     * Effects: Executes the system call specified in reg_A, using reg_B as an argument or return value.
     */
    void syscallHandler() {
        privileged = true;
        SyscallNumber syscallNum = static_cast<SyscallNumber>(reg_A);
        switch (syscallNum) {
            case SyscallNumber::PRINT_CHAR: {
                cout << static_cast<char>(reg_B) <<endl;
                break;
            }
            case SyscallNumber::READ_CHAR: {
                char inputChar;
                cin >> inputChar;
                reg_B = static_cast<uint8_t>(inputChar);
                break;
            }
            default: {
                cerr << "Error: Unknown syscall number: " << (int)reg_A << endl;
                break;
            }
        }
        privileged = false;
    }

    /**
     * Name: step
     * Purpouse: Execute a single instruction at the current program counter (pc).
     * Inputs: None (uses CPU registers and memory)
     * Outputs: Returns true if execution should continue, false if HALT is encountered or an error occurs.
     * Effects: Modifies CPU registers and memory based on the executed instruction.
     */
    bool step() {
        if (pc >= memory.size()) {
            cerr << "Error: Program Counter out of bounds. Halting." << endl;
            return false;
        }
        
        uint8_t instruction = memory[pc];
        pc++;
        cout << "[PC: 0x" << hex << (pc - 1) << "] ";

        switch (instruction) {
            case LOAD_A: {
                uint8_t value = memory[pc++];
                reg_A = value;
                cout << "LOAD_A " << (int)value << endl;
                break;
            }
            case LOAD_B: {
                uint8_t value = memory[pc++];
                reg_B = value;
                cout << "LOAD_B " << (int)value << endl;
                break;
            }
            case STORE_A: {
                uint16_t address = memory[pc++];
                memory[address] = reg_A;
                cout << "STORE_A at 0x" << hex << address << dec << endl;
                break;
            }
            case ADD_A_B: {
                reg_A = reg_A + reg_B;
                cout << "ADD_A_B -> A=" << (int)reg_A << endl;
                break;
            }
            case SUB_A_B: {
                reg_A = reg_A - reg_B;
                cout << "SUB_A_B -> A=" << (int)reg_A << endl;
                break;
            }
            case PUSH_B: {
                if (sp < stack.size()) {
                    stack[sp++] = reg_B;
                    cout << "PUSH_B" << endl;
                }
                break;
            }
            case POP_B: {
                if (sp > 0) {
                    reg_B = stack[--sp];
                    cout << "POP_B" << endl;
                }
                break;
            }
            case JMP: {
                uint16_t address = memory[pc++];
                pc = address;
                cout << "JMP to 0x" << hex << address << dec << endl;
                break;
            }
            case SYSCALL: {
                cout << "SYSCALL" << endl;
                syscallHandler();
                break;
            }
            case HALT: {
                cout << "HALT" << endl;
                return false;
            }
            default: {
                cerr << "Unknown instruction: 0x" << hex << (int)instruction << dec << endl;
                return false;
            }
        }
        return true;
    }

    /**
     * Name: dumpState
     * Purpouse: Print the current state of the CPU registers and flags.
     * Inputs: None
     * Outputs: None (prints to standard output)
     * Effects: Displays the values of registers A, B, PC, SP, and privileged mode status.
     */
    void dumpState() {
        cout << "--- CPU State ---" << endl;
        cout << "A: " << (int)reg_A << ", B: " << (int)reg_B << endl;
        cout << "PC: 0x" << hex << pc << dec << ", SP: 0x" << hex << sp << dec << endl;
        cout << "Privileged: " << (privileged ? "Yes" : "No") << endl;
        cout << "-----------------" << endl;
    }
};

// Simple assembler for the fictional CPU
/**
 * Name: parseHexProgram
 * Purpouse: Convert a string of hex values into a vector of bytes.
 * Inputs:
 *   - hexString: A string containing hex values separated by spaces.
 * Outputs: A vector of uint8_t representing the bytecode.
 * Effects: Parses the input string and converts each hex value to a byte, printing errors for invalid values.
 */
vector<uint8_t> parseHexProgram(const string& hexString) {
    stringstream ss(hexString);
    string hexByte;
    vector<uint8_t> program;
    while (ss >> hexByte) {
        try {
            int byteValue = stoi(hexByte, nullptr, 16);
            if (byteValue >= 0 && byteValue <= 255) {
                program.push_back(static_cast<uint8_t>(byteValue));
            } else {
                cerr << "Invalid hex byte value: " << hexByte << endl;
            }
        } catch (const exception& e) {
            cerr << "Error parsing hex byte: " << hexByte << " - " << e.what() << endl;
        }
    }
    return program;
}

/**
 * Name: assemble
 * Purpouse: Assemble a simple assembly language file into bytecode.
 * Inputs:
 *   - filename: The path to the assembly file.
 * Outputs: A vector of uint8_t representing the assembled bytecode.
 * Effects: Reads the assembly file, translates instructions to opcodes, and handles labels.
 */
vector<uint8_t> assemble(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open assembly file " << filename << endl;
        return {};
    }
    string line;
    vector<string> lines;
    while (getline(file, line)) {
        lines.push_back(line);
    }
    file.close();
    vector<uint8_t> bytecode;
    map<string, uint16_t> labels;
    uint16_t address = USER_PROGRAM_START_ADDRESS;
    for (const auto& l : lines) {
        stringstream ss(l);
        string token;
        ss >> token;
        if (token.empty() || token[0] == ';') continue;
        if (token.back() == ':') {
            labels[token.substr(0, token.size() - 1)] = address;
            ss >> token;
        }
        if (opcodeMap.count(token)) {
            address++;
            if (token == "LOAD_A" || token == "LOAD_B" || token == "JMP" || token == "STORE_A") {
                address++;
            }
        }
    }
    for (const auto& l : lines) {
        stringstream ss(l);
        string token;
        ss >> token;
        if (token.empty() || token[0] == ';') continue;
        if (token.back() == ':') {
            ss >> token;
        }
        if (opcodeMap.count(token)) {
            bytecode.push_back(opcodeMap.at(token));
            if (token == "LOAD_A" || token == "LOAD_B" || token == "JMP" || token == "STORE_A") {
                string operand;
                ss >> operand;
                if (labels.count(operand)) {
                    bytecode.push_back(static_cast<uint8_t>(labels.at(operand)));
                } else {
                    try {
                        bytecode.push_back(static_cast<uint8_t>(stoi(operand, nullptr, 10)));
                    } catch (...) {
                        cerr << "Error: Invalid operand '" << operand << "'" << endl;
                        return {};
                    }
                }
            }
        }
    }
    return bytecode;
}
// Simple Micro-C to bytecode compiler
/**
 * Name: compile
 * Purpouse: Compile a simple Micro-C source file into bytecode.
 * Inputs:
 *   - filename: The path to the Micro-C source file.
 * Outputs: A vector of uint8_t representing the compiled bytecode.
 * Effects: Reads the source file, translates variable declarations and assignments to bytecode.
 */
vector<uint8_t> compile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open source file " << filename << endl;
        return {};
    }

    vector<uint8_t> assemblyOutput;
    unordered_map<string, uint8_t> variables;
    uint8_t next_var_addr = 0x10;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        ss >> token;

        if (token.empty() || (token.size() >= 2 && token[0] == '/' && token[1] == '/')) continue;

        if (token == "int") {
            string varName;
            ss >> varName;
            if (varName.back() == ';') {
                varName.pop_back();
            }
            if (variables.count(varName)) {
                cerr << "Error: Variable '" << varName << "' already declared." << endl;
                return {};
            }
            variables[varName] = next_var_addr++;
            cout << "Compiling: Declared variable '" << varName << "' at address " << (int)variables[varName] << endl;
        } else {
            string varName = token;
            string equals, val1;
            ss >> equals >> val1;
            
            if (equals != "=") {
                cerr << "Error: Expected '=' in assignment statement." << endl;
                return {};
            }

            if (!variables.count(varName)) {
                cerr << "Error: Undefined variable '" << varName << "'" << endl;
                return {};
            }

            string op;
            ss >> op;

            if (op.empty() || op.back() == ';') { // Simple assignment (e.g., a = 10; or a = b;)
                if (op.back() == ';') {
                    op.pop_back();
                }
                int value_or_var;
                if (variables.count(val1)) { // The operand is a variable
                    value_or_var = variables.at(val1);
                    assemblyOutput.push_back(LOAD_A);
                    assemblyOutput.push_back(value_or_var);
                } else { // The operand is a literal number
                    try {
                        value_or_var = stoi(val1);
                        assemblyOutput.push_back(LOAD_A);
                        assemblyOutput.push_back(static_cast<uint8_t>(value_or_var));
                    } catch (...) {
                        cerr << "Error: Invalid operand '" << val1 << "'" << endl;
                        return {};
                    }
                }
                assemblyOutput.push_back(STORE_A);
                assemblyOutput.push_back(variables.at(varName));
            } else { // Arithmetic assignment (e.g., a = a + b;)
                string val2_str;
                ss >> val2_str;
                if (val2_str.back() == ';') {
                    val2_str.pop_back();
                }

                if (variables.count(val1)) {
                    assemblyOutput.push_back(LOAD_A);
                    assemblyOutput.push_back(variables.at(val1));
                } else {
                    try {
                        assemblyOutput.push_back(LOAD_A);
                        assemblyOutput.push_back(static_cast<uint8_t>(stoi(val1)));
                    } catch (...) {
                        cerr << "Error: Invalid operand '" << val1 << "'" << endl;
                        return {};
                    }
                }

                if (variables.count(val2_str)) {
                    assemblyOutput.push_back(LOAD_B);
                    assemblyOutput.push_back(variables.at(val2_str));
                } else {
                    try {
                        assemblyOutput.push_back(LOAD_B);
                        assemblyOutput.push_back(static_cast<uint8_t>(stoi(val2_str)));
                    } catch (...) {
                        cerr << "Error: Invalid operand '" << val2_str << "'" << endl;
                        return {};
                    }
                }

                if (op == "+") {
                    assemblyOutput.push_back(ADD_A_B);
                } else if (op == "-") {
                    assemblyOutput.push_back(SUB_A_B);
                } else {
                    cerr << "Error: Unknown operator '" << op << "'" << endl;
                    return {};
                }
                assemblyOutput.push_back(STORE_A);
                assemblyOutput.push_back(variables.at(varName));
            }
        }
    }
    assemblyOutput.push_back(HALT);
    return assemblyOutput;
}

/**
 * Name: main
 * Purpouse: Provide a command-line interface for loading, assembling, compiling, and executing
 *         programs on the fictional CPU.
 * Inputs: None (reads commands from standard input)
 * Outputs: None (prints to standard output)
 * Effects: Allows users to interactively load programs, step through execution, and view CPU state.
 */
int main() {
    CPU cpu;
    bool running = false;
    cout << "CPU Emulator Ready. Type 'help' for a list of commands." << endl;

    while (true) {
        cout << "> ";
        string line;
        getline(cin, line);
        stringstream ss(line);
        string command;
        ss >> command;

        if (command == "help") {
            cout << "Available commands:" << endl;
            cout << "  load <hex codes>   - Loads a program from a string of hex values" << endl;
            cout << "  asm <filename.asm> - Assembles and loads a program from an assembly file" << endl;
            cout << "  compile <filename.mc>- Compiles and loads a program from a Micro-C file" << endl;
            cout << "  run                - Executes the entire program until a HALT" << endl;
            cout << "  step               - Executes a single instruction" << endl;
            cout << "  dump               - Prints the current state of the CPU" << endl;
            cout << "  mem <address>      - Displays the value at a specific memory address" << endl;
            cout << "  reset              - Resets the CPU state" << endl;
            cout << "  quit               - Exits the emulator" << endl;
        } else if (command == "load") {
            string hexString;
            getline(ss, hexString);
            vector<uint8_t> program = parseHexProgram(hexString);
            if (!program.empty()) {
                cpu.loadProgram(program, USER_PROGRAM_START_ADDRESS);
                cpu.pc = USER_PROGRAM_START_ADDRESS;
                running = true;
                cout << "Program loaded and PC reset to " << USER_PROGRAM_START_ADDRESS << "." << endl;
            }
        } else if (command == "asm") {
            string filename;
            ss >> filename;
            if (!filename.empty()) {
                vector<uint8_t> program = assemble(filename);
                if (!program.empty()) {
                    cpu.loadProgram(program, USER_PROGRAM_START_ADDRESS);
                    cpu.pc = USER_PROGRAM_START_ADDRESS;
                    running = true;
                    cout << "Assembly program '" << filename << "' loaded and PC reset." << endl;
                } else {
                    cout << "Failed to assemble program." << endl;
                }
            } else {
                cout << "Usage: asm <filename.asm>" << endl;
            }
        } else if (command == "compile") {
            string filename;
            ss >> filename;
            if (!filename.empty()) {
                vector<uint8_t> bytecode = compile(filename);
                if (!bytecode.empty()) {
                    cpu.loadProgram(bytecode, USER_PROGRAM_START_ADDRESS);
                    cpu.pc = USER_PROGRAM_START_ADDRESS;
                    running = true;
                    cout << "Compiled program '" << filename << "' loaded and PC reset." << endl;
                } else {
                    cout << "Failed to compile program." << endl;
                }
            } else {
                cout << "Usage: compile <filename.mc>" << endl;
            }
        } else if (command == "run") {
            if (running) {
                while (cpu.step()) {}
                running = false;
                cout << "Program finished." << endl;
            } else {
                cout << "No program loaded. Use 'load', 'asm', or 'compile' first." << endl;
            }
        } else if (command == "step") {
            if (running) {
                if (!cpu.step()) {
                    running = false;
                    cout << "Program finished." << endl;
                }
            } else {
                cout << "No program loaded or program has halted. Use 'load', 'asm', or 'compile' first." << endl;
            }
        } else if (command == "dump") {
            cpu.dumpState();
        } else if (command == "mem") {
            uint16_t address;
            ss >> hex >> address;
            if (address < cpu.memory.size()) {
                cout << "Memory at 0x" << hex << address << ": 0x" << (int)cpu.memory[address] << dec << " (" << (int)cpu.memory[address] << ")" << endl;
            } else {
                cout << "Invalid memory address." << endl;
            }
        } else if (command == "reset") {
            cpu.pc = 0;
            cpu.reg_A = 0;
            cpu.reg_B = 0;
            running = false;
            cout << "CPU state reset." << endl;
        } else if (command == "quit") {
            cout << "Exiting emulator." << endl;
            break;
        } else {
            cout << "Unknown command. Type 'help' for a list of commands." << endl;
        }
    }

    return 0;
}