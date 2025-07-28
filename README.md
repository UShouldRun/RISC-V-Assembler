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

## Requirements

- C++17 compatible compiler (g++)
- Make build system

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

**Henrique Teixeira**
