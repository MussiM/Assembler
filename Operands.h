#pragma once
#include "Tokens.h"

typedef enum OperandKind {
	OPERAND_IMM,
	OPERAND_REG,
	OPERAND_DIR,
	OPERAND_REL,
	OPERAND_MAX = OPERAND_REL
} OperandKind;

/*A structure containing the details of the operand
Kind is Addressing modes
value is address memmory
are is A\R\E */
typedef struct Opearand {
	OperandKind kind;
	int value;
	char are;
} Operand;

struct Assembler;

extern int op_kind_to_am[OPERAND_MAX];

bool is_reg(TokenKind k);
Operand parse_jmp_operand(struct Assembler* as, int target_address);
Operand parse_operand(struct Assembler* as, int target_address);
