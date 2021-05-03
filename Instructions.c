#include <stdbool.h>
#include "Operands.h"
#include "Assembler.h"

typedef enum InstrucitonKind {
	INST_NONE,
	INST_MOVE,
	INST_CMP,
	INST_ADD,
	INST_SUB,
	INST_LEA,
	INST_CLR,
	INST_NOT,
	INST_INC,
	INST_DEC,
	INST_JMP,
	INST_BNE,
	INST_JSR,
	INST_RED,
	INST_PRN,
	INST_RTS,
	INST_STOP
} InstrucitonKind;

/*Mapping of opcode by functions*/
typedef enum Ops {
	OP_MOV = 0,
	OP_CMP = 1,
	OP_ADD = 2,
	OP_SUB = 2,
	OP_LEA = 4,
	OP_CLR = 5,
	OP_NOT = 5,
	OP_INC = 5,
	OP_DEC = 5,
	OP_JMP = 9,
	OP_BNE = 9,
	OP_JSR = 9,
	OP_RED = 12,
	OP_PRN = 13,
	OP_RTS = 14,
	OP_STOP = 15
} Ops;

/*Mapping of functs by functions*/
typedef enum Functs {
	FUN_ADD = 10,
	FUN_SUB = 11,
	FUN_CLR = 10,
	FUN_NOT = 11,
	FUN_INC = 12,
	FUN_DEC = 13,
	FUN_JMP = 10,
	FUN_BNE = 11,
	FUN_JSR = 12
} Functs;

/*The function verifies the word is instruction
Gets kind of the word and returns true\false */
bool is_inst(TokenKind k) {
	return k >= TOKEN_FIRST_INST && k <= TOKEN_LAST_INST;
}

/*The function parsed the instruction word
Gets the details of word*/
void out_instruction(Assembler* as, int op, int funct, int am_src, int am_dst) {
	assert(op >= 0 && op < 16);
	assert(funct >= 0 && funct < 16);
	assert(am_src >= 0 && am_src < 4);
	assert(am_dst >= 0 && am_dst < 4);
	output_word(as, (op << 8) | (funct << 4) | (am_src << 2) | (am_dst << 0), 'A');
}	

/*The function parsed the instruction mov line
Gets struct that contains the first word of the instruction line*/
void parse_mov(Assembler* as) {
	Operand op1;
	Operand op2;
	assert(token_is(as, TOKEN_INST_MOVE));
	next_token(as);
	op1 = parse_operand(as, (as->ic) + 1);
	if(op1.kind == OPERAND_REL){
		error_here(as, "The first operand cannot be relative");	
	}
	if (!token_match(as, TOKEN_COMMA)) {
		error_here(as, "Missing comma between 2 operands");
	}
	op2 = parse_operand( as, (as->ic) + 2);
	if(op2.kind == OPERAND_IMM || op2.kind == OPERAND_REL){
		error_here(as, "The second operand cannot be immediate/relative");	
	}
	out_instruction(as, OP_MOV, 0, op_kind_to_am[op1.kind], op_kind_to_am[op2.kind]);
	output_word(as, op1.value, op1.are);
	output_word(as, op2.value, op2.are);
}

/*The function parsed the instruction cmp line
Gets struct that contains the first word of the instruction line*/
void parse_cmp(Assembler* as){
	Operand op1;
	Operand op2;
	assert(token_is(as, TOKEN_INST_CMP));
	next_token(as);
	op1 = parse_operand( as, (as->ic) + 1);
	if(op1.kind == OPERAND_REL){
		error_here(as, "The first operand cannot be relative");	
	}
	if (!token_match(as, TOKEN_COMMA)) {
		error_here(as, "Missing comma between 2 operands");
	}
	op2 = parse_operand(as, (as->ic) + 2);
	if(op2.kind == OPERAND_REL){
		error_here(as, "The second operand cannot be relative");	
	}
	out_instruction(as, OP_CMP, 0, op_kind_to_am[op1.kind], op_kind_to_am[op2.kind]);
	output_word(as, op1.value, op1.are);
	output_word(as, op2.value, op2.are);
}

/*The function parsed the instruction add line
Gets struct that contains the first word of the instruction line*/
void parse_add(Assembler* as){
	Operand op1;
	Operand op2;
	assert(token_is(as, TOKEN_INST_ADD));
	next_token(as);
	op1 = parse_operand(as, (as->ic) + 1);
	if(op1.kind == OPERAND_REL){
		error_here(as, "The first operand cannot be relative");	
	}
	if (!token_match(as, TOKEN_COMMA)) {
		error_here(as, "Missing comma between 2 operands");
	}
	op2 = parse_operand( as, (as->ic) + 2);
	if(op2.kind == OPERAND_REL || op2.kind == OPERAND_IMM){
		error_here(as, "The second operand cannot be immediate/relative");	
	}
	out_instruction(as, OP_ADD, FUN_ADD, op_kind_to_am[op1.kind], op_kind_to_am[op2.kind]);
	output_word(as, op1.value, op1.are);
	output_word(as, op2.value, op2.are);
}

/*The function parsed the instruction sub line
Gets struct that contains the first word of the instruction line*/
void parse_sub(Assembler* as){
	Operand op1;
	Operand op2;
	assert(token_is(as, TOKEN_INST_SUB));
	next_token(as);
	op1 = parse_operand(as, (as->ic) + 1);
	if(op1.kind == OPERAND_REL){
		error_here(as, "The first operand cannot be relative");	
	}
	if (!token_match(as, TOKEN_COMMA)) {
		error_here(as, "Missing comma between 2 operands");
	}
	op2 = parse_operand(as, (as->ic) + 2);
	if(op2.kind == OPERAND_REL || op2.kind == OPERAND_IMM){
		error_here(as, "The second operand cannot be immediate/relative");	
	}
	out_instruction(as, OP_SUB, FUN_SUB, op_kind_to_am[op1.kind], op_kind_to_am[op2.kind]);
	output_word(as, op1.value, op1.are);
	output_word(as, op2.value, op2.are);
}

/*The function parsed the instruction lea line
Gets struct that contains the first word of the instruction line*/
void parse_lea(Assembler* as){
	Operand op1;
	Operand op2;
	assert(token_is(as, TOKEN_INST_LEA));
	next_token(as);
	op1 = parse_operand(as, (as->ic) + 1);
	if(op1.kind != OPERAND_DIR){
		error_here(as, "The first operand should be a lable");	
	}
	if (!token_match(as, TOKEN_COMMA)) {
		error_here(as, "Missing comma between 2 operands");
	}
	op2 = parse_operand(as, (as->ic) + 2);
	if(op2.kind == OPERAND_REL || op2.kind == OPERAND_IMM){
		error_here(as, "The second operand cannot be immediate/relative");	
	}
	out_instruction(as, OP_LEA, 0, op_kind_to_am[op1.kind], op_kind_to_am[op2.kind]);
	output_word(as, op1.value, op1.are);
	output_word(as, op2.value, op2.are);
}

/*The function parsed the instruction clr line
Gets struct that contains the first word of the instruction line*/
void parse_clr(Assembler* as){
	Operand op1;
	assert(token_is(as, TOKEN_INST_CLR));
	next_token(as);
	op1 = parse_operand(as, (as->ic) + 1);
	if(op1.kind != OPERAND_DIR && op1.kind != OPERAND_REG){
		error_here(as, "The operand cannot be immediate/relative");	
	}
	out_instruction(as, OP_CLR, FUN_CLR, 0, op_kind_to_am[op1.kind]);
	output_word(as, op1.value, op1.are);
}

/*The function parsed the instruction not line
Gets struct that contains the first word of the instruction line*/
void parse_not(Assembler* as){
	Operand op1;
	assert(token_is(as, TOKEN_INST_NOT));
	next_token(as);
	op1 = parse_operand(as, (as->ic) + 1);
	if(op1.kind != OPERAND_DIR && op1.kind != OPERAND_REG){
		error_here(as, "The operand cannot be immediate/relative");	
	}
	out_instruction(as, OP_NOT, FUN_NOT, 0, op_kind_to_am[op1.kind]);
	output_word(as, op1.value, op1.are);
}

/*The function parsed the instruction inc line
Gets struct that contains the first word of the instruction line*/
void parse_inc(Assembler* as){
	Operand op1;
	assert(token_is(as, TOKEN_INST_INC));
	next_token(as);
	op1 = parse_operand(as, (as->ic) + 1);
	if(op1.kind != OPERAND_DIR && op1.kind != OPERAND_REG){
		error_here(as, "The operand cannot be immediate/relative");	
	}
	out_instruction(as, OP_INC, FUN_INC, 0, op_kind_to_am[op1.kind]);
	output_word(as, op1.value, op1.are);
}

/*The function parsed the instruction dec line
Gets struct that contains the first word of the instruction line*/
void parse_dec(Assembler* as){
	Operand op1;
	assert(token_is(as, TOKEN_INST_DEC));
	next_token(as);
	op1 = parse_operand(as, (as->ic) + 1);
	if(op1.kind != OPERAND_DIR && op1.kind != OPERAND_REG){
		error_here(as, "The operand cannot be immediate/relative");		
	}
	out_instruction(as, OP_DEC, FUN_DEC, 0, op_kind_to_am[op1.kind]);
	output_word(as, op1.value, op1.are);
}

/*The function parsed the instruction jmp line
Gets struct that contains the first word of the instruction line*/
void parse_jmp(Assembler* as){
	Operand op1;
	assert(token_is(as, TOKEN_INST_JMP));
	next_token(as);
	op1 = parse_jmp_operand(as, (as->ic) + 1);
	if(op1.kind != OPERAND_REL && op1.kind != OPERAND_DIR){
		error_here(as, "The operand cannot be immediate/register");	
	}
	out_instruction(as, OP_JMP, FUN_JMP, 0, op_kind_to_am[op1.kind]);
	output_word(as, op1.value, op1.are);	
}

/*The function parsed the instruction bne line
Gets struct that contains the first word of the instruction line*/
void parse_bne(Assembler* as){
	Operand op1;
	assert(token_is(as, TOKEN_INST_BNE));
	next_token(as);
	op1 = parse_jmp_operand(as, (as->ic) + 1);
	if(op1.kind != OPERAND_REL && op1.kind != OPERAND_DIR){
		error_here(as, "The operand cannot be immediate/register");	
	}
	out_instruction(as, OP_BNE, FUN_BNE, 0, op_kind_to_am[op1.kind]);
	output_word(as, op1.value, op1.are);
}

/*The function parsed the instruction jsr line
Gets struct that contains the first word of the instruction line*/
void parse_jsr(Assembler* as){
	Operand op1;
	assert(token_is(as, TOKEN_INST_JSR));
	next_token(as);
	op1 = parse_jmp_operand(as, (as->ic) + 1);
	if(op1.kind != OPERAND_REL && op1.kind != OPERAND_DIR){
		error_here(as, "The operand cannot be immediate/register");	
	}
	out_instruction(as, OP_JSR, FUN_JSR, 0, op_kind_to_am[op1.kind]);
	output_word(as, op1.value, op1.are);
}

/*The function parsed the instruction red line
Gets struct that contains the first word of the instruction line*/
void parse_red(Assembler* as){
	Operand op1;
	assert(token_is(as, TOKEN_INST_RED));
	next_token(as);
	op1 = parse_operand(as, (as->ic) + 1);
	if(op1.kind != OPERAND_REG && op1.kind != OPERAND_DIR){
		error_here(as, "The operand cannot be immediate/relative");	
	}
	out_instruction(as, OP_RED, 0, 0, op_kind_to_am[op1.kind]);
	output_word(as, op1.value, op1.are);
}

/*The function parsed the instruction prn line
Gets struct that contains the first word of the instruction line*/
void parse_prn(Assembler* as){
	Operand op1;
	assert(token_is(as, TOKEN_INST_PRN));
	next_token(as);
	op1 = parse_operand(as, (as->ic) + 1);
	if(op1.kind == OPERAND_REL){
		error_here(as, "The operand cannot be relative");	
	}
	out_instruction(as, OP_PRN, 0, 0, op_kind_to_am[op1.kind]);
	output_word(as, op1.value, op1.are);
}

/*The function parsed the instruction rts line
Gets struct that contains the first word of the instruction line*/
void parse_rts(Assembler* as){
	assert(token_is(as, TOKEN_INST_RTS));
	next_token(as);
	out_instruction(as, OP_RTS, 0, 0, 0);
}

/*The function parsed the instruction stop line
Gets struct that contains the first word of the instruction line*/
void parse_stop(Assembler* as){
	assert(token_is(as, TOKEN_INST_STOP));
	next_token(as);
	out_instruction(as, OP_STOP, 0, 0, 0);
}
