import struct
import sys

# --- Registers ---
REGS = ['x'+str(i) for i in range(32)]

# --- Opcode -> type ---
OPCODES = {
    0b0110011: 'R',    # R-type
    0b0010011: 'I',    # I-type ALU
    0b0000011: 'I',    # Loads
    0b0100011: 'S',    # Stores
    0b1100011: 'B',    # Branches
    0b0110111: 'U',    # LUI
    0b0010111: 'U',    # AUIPC
    0b1101111: 'J',    # JAL
    0b1100111: 'I',    # JALR
    0b1110011: 'SYSTEM',
    0b0000000: 'LNS',  # Custom LNS
}

# --- R-type funct3, funct7 -> mnemonic ---
R_FUNCTS = {
    (0x0, 0x00): 'ADD', (0x0, 0x20): 'SUB',
    (0x1, 0x00): 'SLL',
    (0x2, 0x00): 'SLT',
    (0x3, 0x00): 'SLTU',
    (0x4, 0x00): 'XOR',
    (0x5, 0x00): 'SRL', (0x5, 0x20): 'SRA',
    (0x6, 0x00): 'OR',
    (0x7, 0x00): 'AND',
    (0x1, 0x01): 'MUL',
    (0x2, 0x01): 'MULSU',
    (0x3, 0x01): 'MULU',
    (0x4, 0x01): 'DIV',
    (0x5, 0x01): 'DIVU',
    (0x6, 0x01): 'REM',
    (0x7, 0x01): 'REMU',
}

# --- I-type funct3 -> mnemonic ---
I_FUNCTS = {
    0x0: 'ADDI',
    0x1: 'SLLI',
    0x2: 'SLTI',
    0x3: 'SLTIU',
    0x4: 'XORI',
    0x5: 'SRLI/SRAI',
    0x6: 'ORI',
    0x7: 'ANDI',
}

# Load instructions funct3 -> mnemonic
LOAD_FUNCTS = {0x0: 'LB', 0x1: 'LH', 0x2: 'LW', 0x4: 'LBU', 0x5: 'LHU'}

# Store instructions funct3 -> mnemonic
STORE_FUNCTS = {0x0: 'SB', 0x1: 'SH', 0x2: 'SW'}

# Branch instructions funct3 -> mnemonic
BRANCH_FUNCTS = {0x0: 'BEQ', 0x1: 'BNE', 0x4: 'BLT', 0x5: 'BGE', 0x6: 'BLTU', 0x7: 'BGEU'}

# SYSTEM instructions imm -> mnemonic
SYSTEM_FUNCTS = {0x0: 'ECALL', 0x1: 'EBREAK', 0x102: 'SRET'}

# LNS custom instructions (funct3, funct7) -> mnemonic
LNS_FUNCTS = {(0x0,0x00): 'LNS_ADD',
              (0x1,0x00): 'LNS_SUB',
              (0x2,0x00): 'LNS_MUL',
              (0x3,0x00): 'LNS_DIV',
              (0x4,0x00): 'LNS_SQT'}

# --- Helpers ---
def sign_extend(value, bits):
    mask = 1 << (bits-1)
    return (value & (mask-1)) - (value & mask)

def get_bits(value, start, end):
    """Return bits from start (inclusive) to end (exclusive)."""
    return (value >> start) & ((1 << (end - start)) - 1)

# --- B-type immediate extraction ---
def get_branch_imm(inst):
    imm_12   = get_bits(inst, 31, 32) << 12
    imm_10_5 = get_bits(inst, 25, 31) << 5
    imm_4_1  = get_bits(inst, 8, 12) << 1
    imm_11   = get_bits(inst, 7, 8) << 11
    imm = imm_12 | imm_11 | imm_10_5 | imm_4_1
    return sign_extend(imm, 13)

# --- J-type immediate extraction ---
def get_jal_imm(inst):
    imm_20   = get_bits(inst, 31, 32) << 20
    imm_10_1 = get_bits(inst, 21, 31) << 1
    imm_11   = get_bits(inst, 20, 21) << 11
    imm_19_12= get_bits(inst, 12, 20) << 12
    imm = imm_20 | imm_19_12 | imm_11 | imm_10_1
    return sign_extend(imm, 21)

# --- Disassembler ---
def disassemble(inst, pc):
    opcode = inst & 0x7f
    type_ = OPCODES.get(opcode)
    if type_ is None:
        return f".word 0x{inst:08x} 0b{inst:032b}"

    # --- R-type ---
    if type_ == 'R':
        rd = REGS[get_bits(inst, 7, 12)]
        funct3 = get_bits(inst, 12, 15)
        rs1 = REGS[get_bits(inst, 15, 20)]
        rs2 = REGS[get_bits(inst, 20, 25)]
        funct7 = get_bits(inst, 25, 32)
        mnemonic = R_FUNCTS.get((funct3, funct7), 'R_UNKNOWN')
        return f"{mnemonic} {rd}, {rs1}, {rs2}"

    # --- I-type ---
    elif type_ == 'I':
        rd = REGS[get_bits(inst, 7, 12)]
        funct3 = get_bits(inst, 12, 15)
        rs1 = REGS[get_bits(inst, 15, 20)]
        imm = sign_extend(get_bits(inst, 20, 32), 12)

        if opcode == 0b0000011:  # Loads
            mnemonic = LOAD_FUNCTS.get(funct3,'L_UNKNOWN')
            return f"{mnemonic} {rd}, {imm}({rs1})"

        elif opcode == 0b1100111:  # JALR
            return f"JALR {rd}, {rs1}, {imm}"

        elif opcode == 0b0010011:  # ALU immediate
            if funct3 == 0x5:
                shamt = get_bits(imm,0,5)
                funct7 = get_bits(imm,5,12)
                mnemonic = 'SRAI' if funct7==0x20 else 'SRLI'
                return f"{mnemonic} {rd}, {rs1}, {shamt}"

            mnemonic = I_FUNCTS.get(funct3,'I_UNKNOWN')
            return f"{mnemonic} {rd}, {rs1}, {imm}"

        else:
            return f"I_UNKNOWN {rd}, {rs1}, {imm}"

    # --- S-type ---
    elif type_ == 'S':
        funct3 = get_bits(inst, 12, 15)
        rs1 = REGS[get_bits(inst, 15, 20)]
        rs2 = REGS[get_bits(inst, 20, 25)]
        imm = sign_extend(get_bits(inst,7,12) | (get_bits(inst,25,32)<<5),12)
        mnemonic = STORE_FUNCTS.get(funct3,'S_UNKNOWN')
        return f"{mnemonic} {rs2}, {imm}({rs1})"

    # --- B-type ---
    elif type_ == 'B':
        funct3 = get_bits(inst, 12, 15)
        rs1 = REGS[get_bits(inst, 15, 20)]
        rs2 = REGS[get_bits(inst, 20, 25)]
        imm = get_branch_imm(inst)
        target = (pc + imm) & 0xFFFFFFFF
        mnemonic = BRANCH_FUNCTS.get(funct3,'B_UNKNOWN')
        return f"{mnemonic} {rs1}, {rs2}, {imm} (target=0x{target:08x})"

    # --- U-type ---
    elif type_ == 'U':
        rd = REGS[get_bits(inst,7,12)]
        imm = inst & 0xFFFFF000
        target = (pc + imm) & 0xFFFFFFFF
        if opcode == 0b0110111:  # LUI
            return f"LUI {rd}, {hex(imm)}"
        else:  # AUIPC
            return f"AUIPC {rd}, {hex(imm)} (target=0x{target:08x})"

    # --- J-type ---
    elif type_ == 'J':
        rd = REGS[get_bits(inst,7,12)]
        imm = get_jal_imm(inst)
        target = (pc + imm) & 0xFFFFFFFF
        return f"JAL {rd}, {imm} (target=0x{target:08x})"

    # --- SYSTEM ---
    elif type_ == 'SYSTEM':
        imm = get_bits(inst,20,32)
        mnemonic = SYSTEM_FUNCTS.get(imm,'SYSTEM_UNKNOWN')
        return f"{mnemonic}"

    # --- LNS ---
    elif type_ == 'LNS':
        rd = REGS[get_bits(inst,7,12)]
        funct3 = get_bits(inst,12,15)
        rs1 = REGS[get_bits(inst,15,20)]
        rs2 = REGS[get_bits(inst,20,25)]
        funct7 = get_bits(inst,25,32)
        mnemonic = LNS_FUNCTS.get((funct3,funct7),'LNS_UNKNOWN')
        return f"{mnemonic} {rd}, {rs1}, {rs2}"

    # --- fallback ---
    else:
        return f".word 0x{inst:08x}"

# --- Parse binary file ---
def parse_binary(filename):
    with open(filename,"rb") as f:
        # Read 64-bit headers
        s_data = struct.unpack("<Q", f.read(8))[0]
        s_insts = struct.unpack("<Q", f.read(8))[0]
        print(f"# Data words: {s_data}, Instructions: {s_insts}")

        # Skip data section
        f.seek(16 + s_data * 4)

        pc = 16 + s_data * 4  # first instruction after data
        for _ in range(s_insts):
            data = f.read(4)
            if len(data)<4:
                break
            inst = struct.unpack("<I", data)[0]
            print(f"0x{pc:08x} {disassemble(inst, pc)}")
            pc += 4

# --- Main ---
if __name__=="__main__":
    if len(sys.argv)<2:
        print("Usage: python disasm.py <binary_file>")
        sys.exit(1)
    parse_binary(sys.argv[1])
