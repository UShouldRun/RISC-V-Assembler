# Register Name Test File
# Tests all register aliases and numeric names

.text
    # Test zero register aliases
    add zero, zero, zero
    add x0, x0, x0
    
    # Test return address register
    add ra, ra, ra
    add x1, x1, x1
    
    # Test stack pointer
    add sp, sp, sp
    add x2, x2, x2
    
    # Test global pointer
    add gp, gp, gp
    add x3, x3, x3
    
    # Test thread pointer
    add tp, tp, tp
    add x4, x4, x4
    
    # Test temporary registers
    add t0, t0, t0
    add x5, x5, x5
    add t1, t1, t1
    add x6, x6, x6
    add t2, t2, t2
    add x7, x7, x7
    
    # Test saved/frame pointer
    add s0, s0, s0
    add x8, x8, x8
    add s1, s1, s1
    add x9, x9, x9
    
    # Test argument/return registers
    add a0, a0, a0
    add x10, x10, x10
    add a1, a1, a1
    add x11, x11, x11
    add a2, a2, a2
    add x12, x12, x12
    add a3, a3, a3
    add x13, x13, x13
    add a4, a4, a4
    add x14, x14, x14
    add a5, a5, a5
    add x15, x15, x15
    add a6, a6, a6
    add x16, x16, x16
    add a7, a7, a7
    add x17, x17, x17
    
    # Test saved registers
    add s2, s2, s2
    add x18, x18, x18
    add s3, s3, s3
    add x19, x19, x19
    add s4, s4, s4
    add x20, x20, x20
    add s5, s5, s5
    add x21, x21, x21
    add s6, s6, s6
    add x22, x22, x22
    add s7, s7, s7
    add x23, x23, x23
    add s8, s8, s8
    add x24, x24, x24
    add s9, s9, s9
    add x25, x25, x25
    add s10, s10, s10
    add x26, x26, x26
    add s11, s11, s11
    add x27, x27, x27
    
    # Test more temporary registers
    add t3, t3, t3
    add x28, x28, x28
    add t4, t4, t4
    add x29, x29, x29
    add t5, t5, t5
    add x30, x30, x30
    add t6, t6, t6
    add x31, x31, x31
