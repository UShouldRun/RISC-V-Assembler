# Test LNS Matrix: 2x2 Matrix Multiplication using LNS
# Matrix A * Matrix B = Matrix C
# All values stored in 16-bit LNS format (sign + 8-bit integer + 7-bit fractional)
.data
    # Matrix A (2x2) - LNS format
    # [[2.0, 3.0],
    #  [1.5, 4.0]]
    matrix_a: .half 0x0100, 0x0198, 0x00C0, 0x0200  # LNS encoded values
    
    # Matrix B (2x2) - LNS format  
    # [[1.0, 2.0],
    #  [3.0, 0.5]]
    matrix_b: .half 0x0000, 0x0100, 0x0198, 0xFF00  # LNS encoded values
    
    # Result matrix C (2x2)
    matrix_c: .half 0, 0, 0, 0
    
    msg_result: .string "Matrix C compute\n"

.text
main:
    # Load matrix addresses
    la a0, matrix_a
    la a1, matrix_b
    la a2, matrix_c
    
    # Compute C = A * B (2x2 matrices)
    jal lns_matrix_mult_2x2
    
    # Print completion message
    li a7, 4
    la a0, msg_result
    ecall
    
    # Exit
    li a7, 10
    ecall

# Function: lns_matrix_mult_2x2
# Multiplies two 2x2 matrices in LNS format
# Args: a0 = matrix A, a1 = matrix B, a2 = result C
lns_matrix_mult_2x2:
    addi sp, sp, -32
    sw ra, 28(sp)
    sw s0, 24(sp)
    sw s1, 20(sp)
    sw s2, 16(sp)
    sw s3, 12(sp)
    sw s4, 8(sp)
    sw s5, 4(sp)
    
    mv s0, a0           # s0 = matrix A
    mv s1, a1           # s1 = matrix B
    mv s2, a2           # s2 = matrix C
    
    # Compute C[0][0] = A[0][0]*B[0][0] + A[0][1]*B[1][0]
    lhu t0, 0(s0)       # A[0][0]
    lhu t1, 0(s1)       # B[0][0]
    lmul t2, t0, t1     # A[0][0] * B[0][0]
    
    lhu t0, 2(s0)       # A[0][1]
    lhu t1, 4(s1)       # B[1][0]
    lmul t3, t0, t1     # A[0][1] * B[1][0]
    
    ladd t4, t2, t3     # Sum products
    sh t4, 0(s2)        # Store C[0][0]
    
    # Compute C[0][1] = A[0][0]*B[0][1] + A[0][1]*B[1][1]
    lhu t0, 0(s0)       # A[0][0]
    lhu t1, 2(s1)       # B[0][1]
    lmul t2, t0, t1     # A[0][0] * B[0][1]
    
    lhu t0, 2(s0)       # A[0][1]
    lhu t1, 6(s1)       # B[1][1]
    lmul t3, t0, t1     # A[0][1] * B[1][1]
    
    ladd t4, t2, t3     # Sum products
    sh t4, 2(s2)        # Store C[0][1]
    
    # Compute C[1][0] = A[1][0]*B[0][0] + A[1][1]*B[1][0]
    lhu t0, 4(s0)       # A[1][0]
    lhu t1, 0(s1)       # B[0][0]
    lmul t2, t0, t1     # A[1][0] * B[0][0]
    
    lhu t0, 6(s0)       # A[1][1]
    lhu t1, 4(s1)       # B[1][0]
    lmul t3, t0, t1     # A[1][1] * B[1][0]
    
    ladd t4, t2, t3     # Sum products
    sh t4, 4(s2)        # Store C[1][0]
    
    # Compute C[1][1] = A[1][0]*B[0][1] + A[1][1]*B[1][1]
    lhu t0, 4(s0)       # A[1][0]
    lhu t1, 2(s1)       # B[0][1]
    lmul t2, t0, t1     # A[1][0] * B[0][1]
    
    lhu t0, 6(s0)       # A[1][1]
    lhu t1, 6(s1)       # B[1][1]
    lmul t3, t0, t1     # A[1][1] * B[1][1]
    
    ladd t4, t2, t3     # Sum products
    sh t4, 6(s2)        # Store C[1][1]
    
    lw ra, 28(sp)
    lw s0, 24(sp)
    lw s1, 20(sp)
    lw s2, 16(sp)
    lw s3, 12(sp)
    lw s4, 8(sp)
    lw s5, 4(sp)
    addi sp, sp, 32
    ret
