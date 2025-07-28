# Recursive Factorial Calculator
# Demonstrates function calls and recursion

.data
    prompt: .string "Enter a number for factorial: "
    result_msg: .string "Factorial is: "
    newline: .string "\n"

.text
main:
    # Print prompt
    li a7, 4            # System call for print string
    la a0, prompt
    ecall
    
    # Read integer input
    li a7, 5            # System call for read integer
    ecall
    mv s0, a0           # Store input in s0
    
    # Call factorial function
    mv a0, s0           # Set argument for factorial
    jal factorial   # Call factorial function
    mv s1, a0           # Store result in s1
    
    # Print result message
    li a7, 4
    la a0, result_msg
    ecall
    
    # Print factorial result
    li a7, 1            # System call for print integer
    mv a0, s1
    ecall
    
    # Print newline
    li a7, 4
    la a0, newline
    ecall
    
    # Exit program
    li a7, 10
    ecall

# Recursive factorial function
# Input: a0 = n
# Output: a0 = n!
factorial:
    # Save return address and argument on stack
    addi sp, sp, -8     # Allocate stack space
    sw ra, 4(sp)        # Save return address
    sw a0, 0(sp)        # Save argument (n)
    
    # Base case: if n <= 1, return 1
    li t0, 1
    ble a0, t0, base_case
    
    # Recursive case: factorial(n-1)
    addi a0, a0, -1     # n - 1
    jal factorial   # factorial(n-1)
    
    # Multiply n * factorial(n-1)
    lw t0, 0(sp)        # Load original n
    mul a0, t0, a0      # n * factorial(n-1)
    j factorial_return
    
base_case:
    li a0, 1            # Return 1 for base case
    
factorial_return:
    # Restore stack and return
    lw ra, 4(sp)        # Restore return address
    addi sp, sp, 8      # Deallocate stack space
    ret                 # Return to caller
