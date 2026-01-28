# Test LNS Operations: Comprehensive test of all LNS instructions
# Tests: ladd, lsub, lmul, ldiv, lsqrt
# All values in 16-bit LNS format (1 sign + 8 int + 7 frac)

.data
    # Test values in LNS format
    val_2_0: .half 0x0100      # 2.0 (Lx = 1.0)
    val_4_0: .half 0x0200      # 4.0 (Lx = 2.0)
    val_8_0: .half 0x0300      # 8.0 (Lx = 3.0)
    val_0_5: .half 0xFF00      # 0.5 (Lx = -1.0)
    val_1_0: .half 0x0000      # 1.0 (Lx = 0.0)
    val_3_0: .half 0x0198      # 3.0 (Lx ≈ 1.585)
    val_neg2: .half 0x8100     # -2.0 (sign=1, Lx = 1.0)
    val_16_0: .half 0x0400     # 16.0 (Lx = 4.0)
    
    # Result storage
    result_add: .half 0
    result_sub: .half 0
    result_mul: .half 0
    result_div: .half 0
    result_sqrt: .half 0
    
    msg_add: .string "Testing LADD\n"
    msg_sub: .string "Testing LSUB\n"
    msg_mul: .string "Testing LMUL\n"
    msg_div: .string "Testing LDIV\n"
    msg_sqrt: .string "Testing LSQRT\n"
    msg_done: .string "All LNS operations tested\n"

.text
main:
    # Test 1: LADD - Addition
    # 2.0 + 3.0 = 5.0
    li a7, 4
    la a0, msg_add
    ecall
    
    la t0, val_2_0
    la t1, val_3_0
    lhu a0, 0(t0)
    lhu a1, 0(t1)
    ladd a2, a0, a1
    la t2, result_add
    sh a2, 0(t2)
    
    # Test 2: LSUB - Subtraction
    # 4.0 - 2.0 = 2.0
    li a7, 4
    la a0, msg_sub
    ecall
    
    la t0, val_4_0
    la t1, val_2_0
    lhu a0, 0(t0)
    lhu a1, 0(t1)
    lsub a2, a0, a1
    la t2, result_sub
    sh a2, 0(t2)
    
    # Test 3: LMUL - Multiplication
    # 2.0 * 4.0 = 8.0
    # In LNS: Lx + Ly = 1.0 + 2.0 = 3.0
    li a7, 4
    la a0, msg_mul
    ecall
    
    la t0, val_2_0
    la t1, val_4_0
    lhu a0, 0(t0)
    lhu a1, 0(t1)
    lmul a2, a0, a1
    la t2, result_mul
    sh a2, 0(t2)
    
    # Test 4: LDIV - Division
    # 8.0 / 2.0 = 4.0
    # In LNS: Lx - Ly = 3.0 - 1.0 = 2.0
    li a7, 4
    la a0, msg_div
    ecall
    
    la t0, val_8_0
    la t1, val_2_0
    lhu a0, 0(t0)
    lhu a1, 0(t1)
    ldiv a2, a0, a1
    la t2, result_div
    sh a2, 0(t2)
    
    # Test 5: LSQRT - Square Root
    # sqrt(16.0) = 4.0
    # In LNS: Lx >> 1 = 4.0 >> 1 = 2.0
    li a7, 4
    la a0, msg_sqrt
    ecall
    
    la t0, val_16_0
    lhu a0, 0(t0)
    lsqrt a1, a0
    la t2, result_sqrt
    sh a1, 0(t2)
    
    # Additional complex tests
    # Test 6: Chain operations: (2.0 * 3.0) + 4.0 = 10.0
    la t0, val_2_0
    la t1, val_3_0
    lhu a0, 0(t0)
    lhu a1, 0(t1)
    lmul a2, a0, a1     # 2.0 * 3.0 = 6.0
    
    la t0, val_4_0
    lhu a0, 0(t0)
    ladd a2, a2, a0     # 6.0 + 4.0 = 10.0
    
    # Test 7: Division by result: 10.0 / 2.0 = 5.0
    la t0, val_2_0
    lhu a0, 0(t0)
    ldiv a2, a2, a0     # 10.0 / 2.0 = 5.0
    
    # Test 8: Square root of result: sqrt(4.0) = 2.0
    la t0, val_4_0
    lhu a0, 0(t0)
    lsqrt a1, a0
    
    # Test 9: Negative number handling: -2.0 + 3.0 = 1.0
    la t0, val_neg2
    la t1, val_3_0
    lhu a0, 0(t0)
    lhu a1, 0(t1)
    ladd a2, a0, a1
    
    # Test 10: Multiplication with 0.5: 4.0 * 0.5 = 2.0
    la t0, val_4_0
    la t1, val_0_5
    lhu a0, 0(t0)
    lhu a1, 0(t1)
    lmul a2, a0, a1
    
    # Print completion
    li a7, 4
    la a0, msg_done
    ecall
    
    # Exit
    li a7, 10
    ecall
