# RISC-V Assembler with LNSU Instructions

A RISC-V assembly language assembler written in C++. This project provides a complete toolchain for parsing and processing RISC-V assembly code, including support for custom LNSU (Logarithmic Number System Unit) instructions.

This assembler was developed to test and validate the LNSU implementation during an AI Accelerator internship at INESC TEC, as part of a Masters Program at FEUP focusing on RISC-V CPU development.

## Features

- RISC-V 32IM instruction set support
- Custom LNSU (Logarithmic Number System Unit) instructions
- Lexical analysis and tokenization
- Modular library architecture
- Comprehensive test suite

## Building

To build the assembler, run:

```bash
make
```

This will create the `riscv` executable in the `build/` directory.

## Usage

To assemble a RISC-V assembly file:

```bash
./build/riscv <assembly_file.s>
```

Example:
```bash
./build/riscv test/test1.s
```

## Testing

The project includes a comprehensive test suite. To run all tests:

```bash
make test
```

This will run the assembler against all test files in the `test/` directory and report the results.

## Cleaning

To clean build artifacts:

```bash
make clean
```

## Project Structure

The project is organized into several modules:

- **lexer**: Tokenizes assembly source code
- **parser**: Parses tokens into an abstract syntax tree
- **mapper**: Maps parsed instructions (planned feature)
- **error**: Error handling utilities

## Binary File Layout

The custom RISC-V binary format used by this simulator consists of a **fixed-size header** followed by the **memory contents** for the text and data segments. The stack is **not stored in the binary**; its location and size are only specified in the header.

### File Structure

| Offset (bytes) | Size (bytes)               | Description                                                             |
| -------------- | -------------------------- | ----------------------------------------------------------------------- |
| 0              | 4                          | Number of instructions in the text segment (`s_insts`)                  |
| 4              | 4                          | Number of words in the data segment (`s_data`)                          |
| 8              | 4                          | Stack size in words (`s_stack`)                                         |
| 12             | 4                          | Base address of text segment (`text_addr`)                              |
| 16             | 4                          | Base address of data segment (`data_addr`)                              |
| 20             | 4                          | Base address of stack segment (`stack_addr`)                            |
| 24             | 4 × (`s_insts` + `s_data`) | Memory words (`uint32_t`) containing the **text** and **data** segments |

> **Note:** All integers are stored in **little-endian** format.

---

### Segment Layout in Memory

1. **Text Segment (`.text`)**

   * Start: `text_addr`
   * Size: `s_insts * 4` bytes
   * Contains instructions encoded as 32-bit words.

2. **Data Segment (`.data`)**

   * Start: `data_addr`
   * Size: `s_data * 4` bytes
   * Contains initialized data (words).

3. **Stack Segment (`.stack`)**

   * Start: `stack_addr`
   * Size: `s_stack * 4` bytes
   * **Not included in the binary**; reserved in memory by the simulator at runtime.
   * Stack grows downward from `stack_addr + s_stack * 4`.

---

### Reading Instructions and Data

To access a **virtual address `addr`** in memory:

* **Text segment:**

  ```cpp
  index = (addr - text_addr) >> 2;  // word index in mem[]
  instruction = mem[index];
  ```

* **Data segment:**

  ```cpp
  index = ((addr - data_addr) + s_insts * 4) >> 2;
  word = mem[index];
  ```

* **Stack segment:**

  * Reserved in memory; the simulator maps it starting at `stack_addr`, with no corresponding binary data.
  * Access uses `index = ((addr - stack_addr) + s_insts*4 + s_data*4) >> 2`.

---

### Memory Layout Diagram

```text
Binary File Header
+-------------------+
| s_insts           | 0x00
| s_data            | 0x04
| s_stack           | 0x08
| text_addr         | 0x0C
| data_addr         | 0x10
| stack_addr        | 0x14
+-------------------+

Memory Array (mem[])
+-------------------+ <- mem[0]
| Text Segment      | text_addr .. data_addr-1
| instructions      |
+-------------------+ <- mem[s_insts]
| Data Segment      | data_addr .. stack_addr-1
| initialized data  |
+-------------------+ <- mem[s_insts + s_data - 1]
| Stack Segment     | stack_addr .. stack_addr + s_stack*4 - 1
| (reserved, zeroed)| 
+-------------------+ <- mem[s_insts + s_data + s_stack - 1]
```

**Example:**

```text
Header:
0x00: 0x00000020  ; 32 instructions
0x04: 0x00000010  ; 16 words of data
0x08: 0x00000400  ; 1024-word stack
0x0C: 0x80000000  ; text segment base
0x10: 0x80000200  ; data segment base
0x14: 0x80000300  ; stack base

Memory array (mem[]):
mem[0..31]       -> instructions (.text)
mem[32..47]      -> data (.data)
mem[48..1071]    -> stack (.stack, reserved at runtime)
```

> This ensures that **text** and **data** are read from the binary, while **stack memory** is allocated by the simulator at runtime according to the header.

## Requirements

- C++17 compatible compiler (g++)
- Make build system

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

**Henrique Teixeira**
