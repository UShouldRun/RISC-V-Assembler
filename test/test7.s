# Bubble Sort Algorithm
# Sorts an array of integers in ascending order

.data
    array: .word 64, 34, 25, 12, 22, 11, 90, 88, 76, 50
    array_size: .word 10
    original_msg: .string "Original array: "
    sorted_msg: .string "Sorted array: "
    space: .string " "
    newline: .string "\n"

.text
main:
    # Print original array
    la a0, original_msg
    li a7, 4
    ecall
    
    la a0, array
    lw a1, array_size
    jal print_array
    
    # Sort the array
    la a0, array
    lw a1, array_size
    jal bubble_sort
    
    # Print sorted array
    la a0, sorted_msg
    li a7, 4
    ecall
    
    la a0, array
    lw a1, array_size
    jal print_array
    
    # Exit program
    li a7, 10
    ecall

# Function: bubble_sort
# Input: a0 = array address, a1 = array size
# Sorts array in-place
bubble_sort:
    # Save registers
    addi sp, sp, -20
    sw ra, 16(sp)
    sw s0, 12(sp)
    sw s1, 8(sp)
    sw s2, 4(sp)
    sw s3, 0(sp)
    
    mv s0, a0           # s0 = array address
    mv s1, a1           # s1 = array size
    li s2, 0            # s2 = i (outer loop counter)
    
outer_loop:
    ble s1, s2, sort_done    # if size <= i, done
    
    li s3, 0            # s3 = j (inner loop counter)
    sub t0, s1, s2      # t0 = size - i
    addi t0, t0, -1     # t0 = size - i - 1
    
inner_loop:
    ble t0, s3, outer_next   # if size - i - 1 <= j, next outer iteration
    
    # Calculate addresses of array[j] and array[j+1]
    slli t1, s3, 2      # t1 = j * 4
    add t1, s0, t1      # t1 = address of array[j]
    addi t2, t1, 4      # t2 = address of array[j+1]
    
    # Load values
    lw t3, 0(t1)        # t3 = array[j]
    lw t4, 0(t2)        # t4 = array[j+1]
    
    # Compare and swap if necessary
    ble t3, t4, no_swap # if array[j] <= array[j+1], no swap needed
    
    # Swap elements
    sw t4, 0(t1)        # array[j] = array[j+1]
    sw t3, 0(t2)        # array[j+1] = array[j]
    
no_swap:
    addi s3, s3, 1      # j++
    j inner_loop
    
outer_next:
    addi s2, s2, 1      # i++
    j outer_loop
    
sort_done:
    # Restore registers
    lw ra, 16(sp)
    lw s0, 12(sp)
    lw s1, 8(sp)
    lw s2, 4(sp)
    lw s3, 0(sp)
    addi sp, sp, 20
    ret

# Function: print_array
# Input: a0 = array address, a1 = array size
print_array:
    # Save registers
    addi sp, sp, -16
    sw ra, 12(sp)
    sw s0, 8(sp)
    sw s1, 4(sp)
    sw s2, 0(sp)
    
    mv s0, a0           # s0 = array address
    mv s1, a1           # s1 = array size
    li s2, 0            # s2 = index counter
    
print_loop:
    ble s1, s2, print_done   # if size <= index, done
    
    # Calculate address of current element
    slli t0, s2, 2      # t0 = index * 4
    add t0, s0, t0      # t0 = base_address + offset
    
    # Print current element
    lw a0, 0(t0)        # Load array element
    li a7, 1            # System call for print integer
    ecall
    
    # Print space
    la a0, space
    li a7, 4            # System call for print string
    ecall
    
    # Increment index
    addi s2, s2, 1
    j print_loop
    
print_done:
    # Print newline
    la a0, newline
    li a7, 4
    ecall
    
    # Restore registers
    lw ra, 12(sp)
    lw s0, 8(sp)
    lw s1, 4(sp)
    lw s2, 0(sp)
    addi sp, sp, 16
    ret
