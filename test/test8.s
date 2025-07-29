# Simple Calculator Program
# Performs basic arithmetic operations based on user input

.data
    welcome_msg: .string "=== Simple RISC-V Calculator ===\n"
    menu_msg: .string "Choose operation:\n1. Addition\n2. Subtraction\n3. Multiplication\n4. Division\n5. Exit\n"
    choice_prompt: .string "Enter choice (1-5): "
    num1_prompt: .string "Enter first number: "
    num2_prompt: .string "Enter second number: "
    result_msg: .string "Result: "
    division_error: .string "Error: Division by zero!\n"
    invalid_choice: .string "Invalid choice! Please try again.\n"
    goodbye_msg: .string "Thank you for using the calculator!\n"
    newline: .string "\n"

.text
main:
    # Print welcome message
    li a7, 4
    la a0, welcome_msg
    ecall
    
calculator_loop:
    # Print menu
    li a7, 4
    la a0, menu_msg
    ecall
    
    # Get user choice
    li a7, 4
    la a0, choice_prompt
    ecall
    
    li a7, 5            # Read integer
    ecall
    mv s0, a0           # Store choice in s0
    
    # Check for exit
    li t0, 5
    sub t0, s0, t0
    beqz t0, exit_program
    
    # Validate choice (1-4)
    li t0, 1
    bgt t0, s0, invalid_input
    li t0, 4
    bgt s0, t0, invalid_input
    
    # Get first number
    li a7, 4
    la a0, num1_prompt
    ecall
    
    li a7, 5
    ecall
    mv s1, a0           # Store first number in s1
    
    # Get second number
    li a7, 4
    la a0, num2_prompt
    ecall
    
    li a7, 5
    ecall
    mv s2, a0           # Store second number in s2
    
    # Branch to appropriate operation
    addi t0, s0, -1
    beqz t0, do_addition
    addi t0, s0, -2
    beqz t0, do_subtraction
    addi t0, s0, -3
    beqz t0, do_multiplication
    addi t0, s0, -4
    beqz t0, do_division
    
do_addition:
    add s3, s1, s2      # s3 = s1 + s2
    j print_result
    
do_subtraction:
    sub s3, s1, s2      # s3 = s1 - s2
    j print_result
    
do_multiplication:
    mul s3, s1, s2      # s3 = s1 * s2
    j print_result
    
do_division:
    # Check for division by zero
    beqz s2, division_by_zero
    div s3, s1, s2      # s3 = s1 / s2
    j print_result
    
division_by_zero:
    li a7, 4
    la a0, division_error
    ecall
    j calculator_loop
    
print_result:
    # Print result message
    li a7, 4
    la a0, result_msg
    ecall
    
    # Print the result
    li a7, 1
    mv a0, s3
    ecall
    
    # Print two newlines
    li a7, 4
    la a0, newline
    ecall
    ecall
    
    j calculator_loop
    
invalid_input:
    li a7, 4
    la a0, invalid_choice
    ecall
    j calculator_loop
    
exit_program:
    # Print goodbye message
    li a7, 4
    la a0, goodbye_msg
    ecall
    
    # Exit
    li a7, 10
    ecall
