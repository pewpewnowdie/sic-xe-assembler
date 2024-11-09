# SIC/XE Assembler

This project is a **SIC/XE assembler** implemented in C++. It translates assembly language programs written for the **Simplified Instructional Computer (SIC) and its extended version (SIC/XE)** into machine code. The assembler processes SIC/XE assembly code, generating object code that can be executed by a SIC/XE simulator.

## Features

- **Supports SIC and SIC/XE instructions** – Provides full support for standard and extended instructions.
- **Two-pass assembly process** – First pass builds the symbol table, while the second pass generates object code.
- **Handles directives** – Supports SIC/XE assembler directives such as `START`, `END`, `BYTE`, `WORD`, `RESB`, and `RESW`.
- **Error reporting** – Identifies and reports syntax errors, undefined symbols, and other common assembly issues.
- **Object code generation** – Produces a well-formatted object file suitable for SIC/XE loaders.

## Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/pewpewnowdie/sic-xe-assembler.git
   ```
2. **Navigate to project directory**
   ```bash
   cd sic-xe-assembler
   ```
3. **Compile the assmbler**
   ```bash
   g++ assembler.cpp -o assembler
   ```

## Usage

1. **Prepare an assembly file** – Create a `.txt` file with SIC/XE assembly code.
2. **Run the assembler**:
   ```bash
   ./assembler input.txt
   ```
3. **Check the output** – The assembler will produce `object.txt` file as output.

## Example

  Given an assembly file (`example.txt`):
  ```asm
  PROG:   START   1000         ; Set the starting address
          LDA     NUM1         ; Load the first number into the accumulator
          +ADD    NUM2         ; Add the second number to the accumulator
          STA     RESULT       ; Store the result in the RESULT memory location
  NUM1:   BYTE    x'5'         ; Define NUM1 as 5
  NUM2:   WORD    10           ; Define NUM2 as 10
  RESULT: WORD    0            ; Define RESULT to store the output
          END     PROG         ; Indicate the end of the program
  ```

  Run the assembler as follows:
  ```bash
  ./assembler example.txt
  ```

  The output file `object.txt` will contain the assembled machine code, along with the intermediate file `intermediate.txt`.

  `object.txt`:
  ```txt
  H^PROG^001000^000011
  T^1000^0A^002007^18300004^0C2004
  E^1000
  ```
  `intermediate.txt`:
  ```txt
  PROG: START 1000
  1000 LDA NUM1
  1003 ADD NUM2
  1007 STA RESULT
  100A NUM1: BYTE x'5'
  100B NUM2: WORD 10
  100E RESULT: WORD 0
  1011 END PROG
  ```
