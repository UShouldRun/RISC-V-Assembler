# Fibonacci Sequence Calculator
# Calculates the nth Fibonacci number iteratively

.data
    prompt: .string "Enter n for Fibonacci sequence: "
    result_msg: .string "Fibonacci number is: "
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
    mv s0, a0           # Store n in s0
    
    # Handle base cases
    li t0, 2
    blt s0, t0, base_case
    
    # Initialize for iteration
    li s1, 0            # fib(0) = 0
    li s2, 1            # fib(1) = 1
    li s3, 2            # counter = 2
    
fibonacci_loop:
    bge s3, s0, done    # if counter >= n, done
    
    # Calculate next fibonacci number
    add s4, s1, s2      # next_fib = fib(n-2) + fib(n-1)
    mv s1, s2           # fib(n-2) = fib(n-1)
    mv s2, s4           # fib(n-1) = next_fib
    
    addi s3, s3, 1      # counter++
    j fibonacci_loop
    
base_case:
    beqz s0, zero_case
    li s2, 1            # fib(1) = 1
    j done
    
zero_case:
    li s2, 0            # fib(0) = 0
    
done:
    # Print result message
    li a7, 4
    la a0, result_msg
    ecall
    
    # Print fibonacci number
    li a7, 1            # System call for print integer
    mv a0, s2
    ecall
    
    # Print newline
    li a7, 4
    la a0, newline
    ecall
    
    # Exit program
    li a7, 10
    ecall
