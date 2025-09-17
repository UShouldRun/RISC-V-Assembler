.text
    # Load immediate values into registers
    addi x5, x0, 10     # x5 = 10
    addi x6, x0, 20     # x6 = 20
    
    # Add them together
    add x7, x5, x6      # x7 = x5 + x6 = 30
    
    # Multiply by 2 (shift left by 1)
    slli x8, x7, 1      # x8 = x7 << 1 = 60
    
    # Subtract original value
    sub x9, x8, x5      # x9 = x8 - x5 = 50
    
    # Store result in x10 for system call
    add x10, x9, x0     # x10 = x9 (copy result)
    
    # Exit system call (Linux)
    addi x17, x0, 93    # System call number for exit (93)
    ecall               # Make system call
