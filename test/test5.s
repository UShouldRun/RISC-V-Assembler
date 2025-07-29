# Array Sum Calculator
# Calculates the sum of elements in an array

.data
    array: .word 10, 25, 30, 45, 50, 15, 20, 35, 40, 5
    array_size: .word 10
    sum_msg: .string "Sum of array elements: "
    avg_msg: .string "Average: "
    newline: .string "\n"

.text
main:
    # Load array address and size
    la s0, array        # s0 = array base address
    lw s1, array_size   # s1 = array size
    li s2, 0            # s2 = sum (accumulator)
    li s3, 0            # s3 = index counter
    
sum_loop:
    ble s1, s3, calculate_average  # if size <= index, done
    
    # Calculate address of current element
    slli t0, s3, 2      # t0 = index * 4 (word size)
    add t0, s0, t0      # t0 = base_address + offset
    
    # Load current element and add to sum
    lw t1, 0(t0)        # t1 = array[index]
    add s2, s2, t1      # sum += array[index]
    
    # Increment index
    addi s3, s3, 1
    j sum_loop
    
calculate_average:
    # Print sum message
    li a7, 4            # System call for print string
    la a0, sum_msg
    ecall
    
    # Print sum
    li a7, 1            # System call for print integer
    mv a0, s2
    ecall
    
    # Print newline
    li a7, 4
    la a0, newline
    ecall
    
    # Calculate and print average
    li a7, 4
    la a0, avg_msg
    ecall
    
    # Integer division for average
    div s4, s2, s1      # s4 = sum / size
    
    li a7, 1
    mv a0, s4
    ecall
    
    # Print newline
    li a7, 4
    la a0, newline
    ecall
    
    # Exit program
    li a7, 10
    ecall
