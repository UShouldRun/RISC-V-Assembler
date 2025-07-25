# Basic RISC-V Assembly Test File
# Tests directives, registers, and basic instructions

.text
    # Basic arithmetic instructions
    add x1, x2, x3
    addi t0, t1, 100
    sub s0, s1, s2
    
    # Load immediate and upper immediate
    li a0, -42
    lui t2, 0x10000
    auipc gp, 0x1000
    
    # Move operations
    mv a1, a0
    
    # Logical operations
    and x10, x11, x12
    andi a2, a3, 0xFF
    or s3, s4, s5
    ori t3, t4, 0x0F
    xor a4, a5, a6
    xori s6, s7, 0xAA
    
    # Shift operations
    sll t5, t6, a7
    slli s8, s9, 5
    srl s10, s11, t0
    srli x28, x29, 3
    sra x30, x31, t1
    srai ra, sp, 7

.data
    # Data section with various data types
    my_byte: .byte 0x42
    my_half: .half 0B10010010
    my_word: .word 0xDEADBEEF
    my_string: .string "Hello RISC-V World!"
