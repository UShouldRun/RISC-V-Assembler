# Edge Cases and Potential Error Conditions Test File
# Tests boundary conditions and potential parsing challenges

.text
    # Instructions with minimal spacing
    add t0,t1,t2
    addi a0,a1,100
    
    # Instructions with extra spacing
    add    t0  ,   t1   ,    t2
    addi   a0  ,   a1   ,    200
    
    # Mixed case (should be handled by case-sensitive lexer)
    # These might be errors depending on your lexer design
    
    # Numbers at boundaries
    li t0, 0
    li t1, 2147483647    # Max 32-bit signed
    li t2, -2147483648   # Min 32-bit signed
    li t3, 0xFFFFFFFF    # Max 32-bit unsigned
    
    # Very long hex numbers
    li t4, 0x00000001
    li t5, 0x80000000
    
    # Binary numbers with leading zeros
    li t6, 0b00000001
    li s0, 0b10000000
    
    # Registers with different formats
    add x0, x1, x2
    add x10, x11, x12
    add x31, x30, x29
    
    # Memory operations with edge offsets
    lw t0, 0(sp)         # Zero offset
    lw t1, -2048(sp)     # Negative boundary
    lw t2, 2047(sp)      # Positive boundary
    
    # Labels at boundaries
label_start:
    nop
    
very_long_label_name_that_tests_symbol_parsing:
    nop
    
_underscore_label:
    nop
    
label123:
    nop
    
    # Comments with various formats
    nop                  # End of line comment
    nop # Comment immediately after instruction
    # Full line comment
        # Indented comment
    
    # Empty lines and whitespace handling
    
    
    nop
    
    # String edge cases
    .string ""           # Empty string
    .string " "          # Single space
    .string "\n"         # Just newline
    .string "\t"         # Just tab
    
    # Directive edge cases
    .byte 0, 1, 255      # Multiple values
    .half 0, 65535       # Multiple halfwords
    .word 0, 0xFFFFFFFF  # Multiple words

.data
    # Data section edge cases
    single_byte: .byte 0x42
    
    # Potential symbol conflicts (testing lexer symbol recognition)
    x0_label: .word 0    # Label that looks like register
    add_data: .word 1    # Label that looks like instruction
    
    # Very long string
    long_string: .string "This is a very long string that tests the lexer's ability to handle extended text content with various characters: !@#$%^&*()_+-=[]{}|;:,.<>?/~`"
