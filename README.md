# A Full-Stack Computing Environment: From High-Level Code to CPU Execution

This project is a comprehensive C++ implementation of a custom computing stack. It evolves from a **simple 8-bit CPU emulator** to a complete system featuring an **assembler**, a **minimal operating system**, and a **high-level compiler**. The goal is to provide a hands-on, educational platform that demonstrates the entire journey of a program, from source code to machine-level execution.

-----

## Project Architecture

The project is structured into four layers of increasing abstraction, each representing a crucial component of a modern computer system.

### **1. The Hardware: A Custom 8-bit CPU Emulator** At the foundation is a CPU emulator built from scratch. It models the core components of a computer's central processing unit.

  * **Processor:** A custom 8-bit processor with two general-purpose registers (`A` and `B`), a 16-bit program counter (`PC`), and a 16-bit stack pointer (`SP`). This architecture is simple enough to understand but complex enough to perform useful computation.
  * **Memory & Stack:** The CPU operates on a 64KB memory space and a dedicated 256-byte stack, demonstrating basic memory management and stack-based operations.
  * **Instruction Set Architecture (ISA):** A simple ISA with opcodes for data movement, arithmetic, and control flow. The `step()` function meticulously simulates the **fetch-decode-execute cycle**, a foundational concept of computer architecture.

-----

### **2. The Low-Level Software: An Assembler** Moving up the stack, an assembler is introduced to bridge the gap between human-readable code and machine instructions.

  * **Two-Pass Assembly:** The assembler reads `.asm` files and performs a **two-pass process**. The first pass builds a **symbol table** to map labels (like `loop:`) to memory addresses, while the second pass generates the final bytecode. This is the same process used by real-world assemblers.
  * **Mnemonic-to-Opcode Translation:** This layer demonstrates the concept of **lexical analysis**, converting instructions like `LOAD_A 10` into the raw hexadecimal bytes (`0x03 0x0A`) that the CPU understands.

-----

### **3. The System Layer: A Minimal Operating System (OS)**

To manage interaction between programs and the "hardware," a minimal operating system is implemented.

  * **Kernel-User Space:** The system introduces a **privileged execution mode** via a new `SYSCALL` instruction. When a user program needs a service (like printing to the console), it executes a `SYSCALL`, which transfers control to a secure, privileged kernel routine.
  * **Kernel Services:** The OS provides essential services, such as `PRINT_CHAR` and `READ_CHAR`, illustrating the role of an OS in abstracting hardware access and managing resources.
  * **Demonstrated Expertise:** This shows an understanding of OS fundamentals, including **interrupts**, **system calls**, and the crucial separation of kernel and user code for system stability and security.

-----

### **4. The High-Level Software: A Micro-C Compiler**

The pinnacle of the stack is a compiler for a simple, C-like language. This tool enables development in a high-level language without the need to write assembly code.

  * **Single-Pass Compilation:** The compiler translates high-level concepts like **variable declarations** (`int a;`) and **expressions** (`a = b + 5;`) directly into the emulator's assembly language, which the assembler then converts into bytecode.
  * **Variable Management:** It manages a simple **symbol table** to track variable names and their corresponding memory addresses.
  * **Demonstrated Expertise:** This layer showcases deep knowledge of **compilation theory**, including lexical analysis, parsing, and code generation, proving an ability to design and implement a complete programming language pipeline.

-----

## Getting Started

### **Setup & Compilation**

This project is compatible with any **C++17-compatible compiler**.

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/jesse-flores/Custom-Computing-Stack-Emulator-Assembler-and-Compiler.git
    ```
2.  **Navigate to the project directory:**
    ```bash
    cd Custom-Computing-Stack-Emulator-Assembler-and-Compiler
    ```
3.  **Compile the program:** For MinGW on Windows, use this command to ensure a console application is built correctly:
    ```bash
    g++ -std=c++17 -Wall -Wextra -O2 emulator.cpp -o emulator
    ```

### **Usage**

Launch the emulator from your terminal:

```bash
./emulator
```

| Command                     | Example                       | Description                                                                 |
| --------------------------- | ----------------------------- | --------------------------------------------------------------------------- |
| `help`                      | `help`                        | Displays all available commands.                                            |
| `load <hex codes>`          | `load 03 05 04 0A 10 FF`      | Loads a program from raw hexadecimal bytecode.                              |
| `asm <filename.asm>`        | `asm program.asm`             | Assembles and loads a program from a `.asm` file.                           |
| `compile <filename.mc>`     | `compile program.mc`          | Compiles and loads a program from a Micro-C file.                           |
| `run`                       | `run`                         | Executes the loaded program until a `HALT` instruction is reached.          |
| `step`                      | `step`                        | Executes one instruction at a time.                                         |
| `dump`                      | `dump`                        | Displays the current state of the CPU registers.                            |
| `mem <address>`             | `mem 0xFF`                    | Displays the value at a specific memory address.                            |
| `reset`                     | `reset`                       | Resets the CPU's state (registers and PC).                                  |
| `quit`                      | `quit`                        | Exits the emulator.                                                         |

-----

### **Demonstration Programs**

To test each layer, try running these example programs.

#### **Micro-C Program (`program.mc`)**

```c
// A simple arithmetic program
int a;
int b;
a = 10;
b = 5;
a = a + b;
```

#### **Assembly Program (`program.asm`)**

```asm
; A program that uses a syscall to print a character
start:
    LOAD_A 1        ; Syscall number for PRINT_CHAR
    LOAD_B 72       ; ASCII value for 'H'
    SYSCALL
    HALT
```

#### **Hexadecimal Program (load directly)**

```bash
# This program loads 5 and 10, adds them, and then halts.
load 03 05 04 0A 10 FF
```

## License

This project is open source and available under the [MIT License](LICENSE).