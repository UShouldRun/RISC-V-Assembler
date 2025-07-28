# 3x3 Matrix Multiplication Program
# Multiplies two 3x3 matrices and displays the result

.data
    # Matrix A (3x3)
    matrix_a: .word 1, 2, 3,
                    4, 5, 6,
                    7, 8, 9
    
    # Matrix B (3x3)
    matrix_b: .word 9, 8, 7,
                    6, 5, 4,
                    3, 2, 1
    
    # Result matrix C (3x3) - initialized to zeros
    matrix_c: .word 0, 0, 0,
                    0, 0, 0,
                    0, 0, 0
    
    matrix_a_msg: .string "Matrix A:\n"
    matrix_b_msg: .string "Matrix B:\n"
    result_msg: .string "Result (A * B):\n"
    space: .string "  "
    newline: .string "\n"

.text
main:
    # Print Matrix A
    li a7, 4
    la a0, matrix_a_msg
    ecall
    
    la a0, matrix_a
    jal print_matrix
    
    # Print Matrix B
    li a7, 4
    la a0, matrix_b_msg
    ecall
    
    la a0, matrix_b
    jal print_matrix
    
    # Perform matrix multiplication
    la a0, matrix_a     # A matrix address
    la a1, matrix_b     # B matrix address
    la a2, matrix_c     # Result matrix address
    jal matrix_multiply_3x3
    
    # Print result
    li a7, 4
    la a0, result_msg
    ecall
    
    la a0, matrix_c
    jal print_matrix
    
    # Exit program
    li a7, 10
    ecall

# Function: matrix_multiply_3x3
# Input: a0 = matrix A address, a1 = matrix B address, a2 = result matrix address
# Performs C = A * B for 3x3 matrices
matrix_multiply_3x3:
    # Save registers
    addi sp, sp, -32
    sw ra, 28(sp)
    sw s0, 24(sp)
    sw s1, 20(sp)
    sw s2, 16(sp)
    sw s3, 12(sp)
    sw s4, 8(sp)
    sw s5, 4(sp)
    sw s6, 0(sp)
    
    mv s0, a0           # s0 = matrix A
    mv s1, a1           # s1 = matrix B
    mv s2, a2           # s2 = result matrix C
    
    li s3, 0            # s3 = i (row counter)
    
row_loop:
    li t0, 3
    bge s3, t0, multiply_done   # if i >= 3, done
    
    li s4, 0            # s4 = j (column counter)
    
col_loop:
    li t0, 3
    bge s4, t0, next_row        # if j >= 3, next row
    
    # Calculate C[i][j] = sum of A[i][k] * B[k][j] for k = 0 to 2
    li s5, 0            # s5 = k (inner loop counter)
    li s6, 0            # s6 = sum accumulator
    
inner_loop:
    li t0, 3
    bge s5, t0, store_result    # if k >= 3, store result
    
    # Calculate address of A[i][k]
    li t0, 3
    mul t0, s3, t0      # t0 = i * 3
    add t0, t0, s5      # t0 = i * 3 + k
    slli t0, t0, 2      # t0 = (i * 3 + k) * 4
    add t0, s0, t0      # t0 = address of A[i][k]
    lw t1, 0(t0)        # t1 = A[i][k]
    
    # Calculate address of B[k][j]
    li t0, 3
    mul t0, s5, t0      # t0 = k * 3
    add t0, t0, s4      # t0 = k * 3 + j
    slli t0, t0, 2      # t0 = (k * 3 + j) * 4
    add t0, s1, t0      # t0 = address of B[k][j]
    lw t2, 0(t0)        # t2 = B[k][j]
    
    # Multiply and accumulate
    mul t3, t1, t2      # t3 = A[i][k] * B[k][j]
    add s6, s6, t3      # sum += A[i][k] * B[k][j]
    
    addi s5, s5, 1      # k++
    j inner_loop
    
store_result:
    # Calculate address of C[i][j]
    li t0, 3
    mul t0, s3, t0      # t0 = i * 3
    add t0, t0, s4      # t0 = i * 3 + j
    slli t0, t0, 2      # t0 = (i * 3 + j) * 4
    add t0, s2, t0      # t0 = address of C[i][j]
    sw s6, 0(t0)        # C[i][j] = sum
    
    addi s4, s4, 1      # j++
    j col_loop
    
next_row:
    addi s3, s3, 1      # i++
    j row_loop
    
multiply_done:
    # Restore registers
    lw ra, 28(sp)
    lw s0, 24(sp)
    lw s1, 20(sp)
    lw s2, 16(sp)
    lw s3, 12(sp)
    lw s4, 8(sp)
    lw s5, 4(sp)
    lw s6, 0(sp)
    addi sp, sp, 32
    ret

# Function: print_matrix
# Input: a0 = matrix address
# Prints a 3x3 matrix
print_matrix:
    # Save registers
    addi sp, sp, -16
    sw ra, 12(sp)
