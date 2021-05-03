#include "Operands.h"
#include "Symbols.h"
#include "Assembler.h"

/*The function verifies the word is register
Gets kind of the word and returns true\false */
bool is_reg(TokenKind k) {
	return k >= TOKEN_FIRST_REG && k <= TOKEN_LAST_REG;
}

/*The function parsed the operand word
Gets the details of word and return start of machine code*/
Operand parse_jmp_operand(Assembler* as, int target_address) {
	Operand op = { 0 };
	Symbol* symbol = NULL;
	
	switch (as->current_token.kind) {
	case TOKEN_IMMEDIATE:
		op.kind = OPERAND_IMM;
		op.value = as->current_token.int_value;
		op.are = 'A';
		next_token(as);
		break;
	case TOKEN_LABEL: {
		op.kind = OPERAND_DIR;
		symbol = symbol_get(as);
		next_token(as);
		if(as->is_second_pass && symbol->kind == SYM_EXTERN) {
			add_extern_refernce(symbol, target_address);
		}
		op.value = symbol->value;
		op.are = symbol->kind == SYM_EXTERN ? 'E' : 'R';
		break;
	}
	case TOKEN_RELATIVE:
		op.kind = OPERAND_REL;
		op.are = 'A';
		next_token(as);
		if(!token_is(as, TOKEN_LABEL)) {
			error_here(as, "The operand isn't a lable");
		}
		symbol = symbol_get(as);
		next_token(as);
		if(symbol->kind == SYM_EXTERN){
			error_here(as, "A relative operand cannot be external");
		}
		op.value = symbol->value - target_address;
		break;
	default:
		if(is_reg(as->current_token.kind)) {
			op.kind = OPERAND_REG;
			op.value = 1 << (as->current_token.kind - TOKEN_FIRST_REG);
			op.are = 'A';
		} else {
			error_here(as, "This is an illegal operand");
		}
		next_token(as);
		break;
	}
	return op;
}

/*The function parsed the operand word that is not of ramification function
Gets the details of word and return start of machine code*/
Operand parse_operand(Assembler* as, int target_address) {
	Operand op = parse_jmp_operand(as, target_address);
	if (op.kind == OPERAND_REL) {
		error_here(as, "The operand cannot be relative");
	}
	return op;
}

int op_kind_to_am[OPERAND_MAX] = {0};
