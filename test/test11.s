# Test LNS Neural Network: Simple single-layer forward pass
# Output = ReLU(Weights * Input + Bias)
# 3 inputs, 2 neurons
# All values in 16-bit LNS format

.data
    # Input vector (3 elements) - LNS format
    # [2.0, 3.0, 1.5]
    input: .half 0x0100, 0x0198, 0x00C0
    
    # Weight matrix (3x2: 3 inputs, 2 neurons) - LNS format
    # Neuron 0: [0.5, 0.8, 1.2]
    # Neuron 1: [1.5, 0.3, 2.0]
    weights: .half 0xFF00, 0xFFCC, 0x0026,  # Neuron 0 weights
                   0x00C0, 0xFEA3, 0x0100   # Neuron 1 weights
    
    # Bias vector (2 elements) - LNS format
    # [0.5, 1.0]
    bias: .half 0xFF00, 0x0000
    
    # Output vector (2 elements)
    output: .half 0, 0
    
    # Temporary storage for weighted sums
    temp: .half 0, 0
    
    msg_done: .string "NN forward pass complete\n"

.text
main:
    # Perform forward pass for neuron 0
    la a0, input
    la a1, weights
    lhu a2, 0(a0)       # input[0] = 2.0
    lhu a3, 0(a1)       # weight[0][0] = 0.5
    lmul t0, a2, a3     # input[0] * weight[0][0]
    
    lhu a2, 2(a0)       # input[1] = 3.0
    lhu a3, 2(a1)       # weight[0][1] = 0.8
    lmul t1, a2, a3     # input[1] * weight[0][1]
    ladd t0, t0, t1     # Accumulate
    
    lhu a2, 4(a0)       # input[2] = 1.5
    lhu a3, 4(a1)       # weight[0][2] = 1.2
    lmul t1, a2, a3     # input[2] * weight[0][2]
    ladd t0, t0, t1     # Accumulate
    
    # Add bias for neuron 0
    la a4, bias
    lhu t1, 0(a4)       # bias[0] = 0.5
    ladd t0, t0, t1     # Add bias
    
    # Apply ReLU (check sign bit, if negative set to 0x4000 which is -inf/0)
    li t2, 0x8000
    and t3, t0, t2      # Check sign bit
    beqz t3, neuron0_positive
    li t0, 0x4000       # Set to -inf (represents 0 in LNS)
neuron0_positive:
    la a5, output
    sh t0, 0(a5)        # Store output[0]
    
    # Perform forward pass for neuron 1
    la a0, input
    la a1, weights
    addi a1, a1, 6      # Point to neuron 1 weights
    
    lhu a2, 0(a0)       # input[0] = 2.0
    lhu a3, 0(a1)       # weight[1][0] = 1.5
    lmul t0, a2, a3     # input[0] * weight[1][0]
    
    lhu a2, 2(a0)       # input[1] = 3.0
    lhu a3, 2(a1)       # weight[1][1] = 0.3
    lmul t1, a2, a3     # input[1] * weight[1][1]
    ladd t0, t0, t1     # Accumulate
    
    lhu a2, 4(a0)       # input[2] = 1.5
    lhu a3, 4(a1)       # weight[1][2] = 2.0
    lmul t1, a2, a3     # input[2] * weight[1][2]
    ladd t0, t0, t1     # Accumulate
    
    # Add bias for neuron 1
    la a4, bias
    lhu t1, 2(a4)       # bias[1] = 1.0
    ladd t0, t0, t1     # Add bias
    
    # Apply ReLU
    li t2, 0x8000
    and t3, t0, t2      # Check sign bit
    beqz t3, neuron1_positive
    li t0, 0x4000       # Set to -inf (represents 0)
neuron1_positive:
    la a5, output
    sh t0, 2(a5)        # Store output[1]
    
    # Print completion message
    li a7, 4
    la a0, msg_done
    ecall
    
    # Exit
    li a7, 10
    ecall
