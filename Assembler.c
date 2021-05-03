#include <stdio.h>
#include <stdlib.h>
#include "Instructions.h"
#include "Symbols.h"
#include "Operands.h"
#include "Assembler.h"

/*This file contains functions that the Main uses*/


/*Define an array that refers to functions that parsed directives*/
typedef void(*directive_parser)(Assembler* as, Symbol* symbol);
directive_parser directives_parsers[TOKEN_LAST_DIR - TOKEN_FIRST_DIR + 1];

/*Define an array that refers to functions that parsed instructions*/
typedef void(*instruction_parser)(Assembler* as);
instruction_parser instructions_parsers[TOKEN_LAST_INST - TOKEN_FIRST_INST + 1] = {0};

/*The function receives the machine code of a word from the directive sentence and places it in the data array*/
void output_data(Assembler* as, int value, char are){
	as->out_data[as->dc].value = value;
	as->out_data[as->dc].are = are;
	++as->dc;
}

/*The function receives the machine code of a word from the instruction sentence and places it in the code array*/
void output_word(Assembler* as, int value, char are) {
	as->out_code[as->ic - MIN_IC_VALUE].value = value;
	as->out_code[as->ic - MIN_IC_VALUE].are = are;
	++as->ic;
}

/*The function verifies that the word received is a directive
get the kind of word and return true\false */
bool is_directive(TokenKind kind){
	if(kind >= TOKEN_FIRST_DIR && kind <= TOKEN_LAST_DIR){
			return true;
		}
	return false;
}

/*The function initializes the const arrays*/
void init_assembler(void) {
	instructions_names[TOKEN_INST_MOVE - TOKEN_FIRST_INST] = "mov";
	instructions_names[TOKEN_INST_CMP - TOKEN_FIRST_INST] = "cmp";
	instructions_names[TOKEN_INST_ADD - TOKEN_FIRST_INST] = "add";
	instructions_names[TOKEN_INST_SUB - TOKEN_FIRST_INST] = "sub";
	instructions_names[TOKEN_INST_LEA - TOKEN_FIRST_INST] = "lea";
	instructions_names[TOKEN_INST_CLR - TOKEN_FIRST_INST] = "clr";
	instructions_names[TOKEN_INST_NOT - TOKEN_FIRST_INST] = "not";
	instructions_names[TOKEN_INST_INC - TOKEN_FIRST_INST] = "inc";
	instructions_names[TOKEN_INST_DEC - TOKEN_FIRST_INST] = "dec";
	instructions_names[TOKEN_INST_JMP - TOKEN_FIRST_INST] = "jmp";
	instructions_names[TOKEN_INST_BNE - TOKEN_FIRST_INST] = "bne";
	instructions_names[TOKEN_INST_JSR - TOKEN_FIRST_INST] = "jsr";
	instructions_names[TOKEN_INST_RED - TOKEN_FIRST_INST] = "red";
	instructions_names[TOKEN_INST_PRN - TOKEN_FIRST_INST] = "prn";
	instructions_names[TOKEN_INST_RTS - TOKEN_FIRST_INST] = "rts";
	instructions_names[TOKEN_INST_STOP - TOKEN_FIRST_INST] = "stop";
	
	directives_names[TOKEN_DIR_DATA - TOKEN_FIRST_DIR] = ".data";
	directives_names[TOKEN_DIR_STRING - TOKEN_FIRST_DIR] = ".string";
	directives_names[TOKEN_DIR_EXTERN - TOKEN_FIRST_DIR] = ".extern";
	directives_names[TOKEN_DIR_ENTRY - TOKEN_FIRST_DIR] = ".entry";

	op_kind_to_am[OPERAND_IMM] = 0;
	op_kind_to_am[OPERAND_DIR] = 1;
	op_kind_to_am[OPERAND_REL] = 2;
	op_kind_to_am[OPERAND_REG] = 3;
	
	instructions_parsers[TOKEN_INST_MOVE - TOKEN_FIRST_INST] = parse_mov;
	instructions_parsers[TOKEN_INST_CMP - TOKEN_FIRST_INST] = parse_cmp;
	instructions_parsers[TOKEN_INST_ADD - TOKEN_FIRST_INST] = parse_add;
	instructions_parsers[TOKEN_INST_SUB - TOKEN_FIRST_INST] = parse_sub;
	instructions_parsers[TOKEN_INST_LEA - TOKEN_FIRST_INST] = parse_lea;
	instructions_parsers[TOKEN_INST_CLR - TOKEN_FIRST_INST] = parse_clr;
	instructions_parsers[TOKEN_INST_NOT - TOKEN_FIRST_INST] = parse_not;
	instructions_parsers[TOKEN_INST_INC - TOKEN_FIRST_INST] = parse_inc;
	instructions_parsers[TOKEN_INST_DEC - TOKEN_FIRST_INST] = parse_dec;
	instructions_parsers[TOKEN_INST_JMP - TOKEN_FIRST_INST] = parse_jmp;
	instructions_parsers[TOKEN_INST_BNE - TOKEN_FIRST_INST] = parse_bne;
	instructions_parsers[TOKEN_INST_JSR - TOKEN_FIRST_INST] = parse_jsr;
	instructions_parsers[TOKEN_INST_RED - TOKEN_FIRST_INST] = parse_red;
	instructions_parsers[TOKEN_INST_PRN - TOKEN_FIRST_INST] = parse_prn;
	instructions_parsers[TOKEN_INST_RTS - TOKEN_FIRST_INST] = parse_rts;
	instructions_parsers[TOKEN_INST_STOP - TOKEN_FIRST_INST] = parse_stop;
	
	directives_parsers[TOKEN_DIR_DATA - TOKEN_FIRST_DIR] = parse_data;
	directives_parsers[TOKEN_DIR_STRING - TOKEN_FIRST_DIR] = parse_string;
	directives_parsers[TOKEN_DIR_EXTERN - TOKEN_FIRST_DIR] = parse_extern;
	directives_parsers[TOKEN_DIR_ENTRY - TOKEN_FIRST_DIR] = parse_entry;
} 

/*The function parsed the line of code
get struct that contains first word of line*/
void parse_statement(Assembler* as) {
	Symbol* symbol = NULL;
	while(token_match(as, TOKEN_NEWLINE));
	if(token_is(as, TOKEN_LABEL_DEF)) {
		if(as->is_second_pass){
			symbol = symbol_get(as);
		} else {
			symbol = symbol_add(as);
		}
		next_token(as);
	}
	if(is_directive(as->current_token.kind)) {	
		directives_parsers[as->current_token.kind - TOKEN_FIRST_DIR](as, symbol);
	} else if (is_inst(as->current_token.kind)) {
		if(symbol != NULL){
			symbol->kind = SYM_CODE;
			symbol->value = as->ic;
		}
		instructions_parsers[as->current_token.kind - TOKEN_FIRST_INST](as);
	} else {
		error_here(as, "expected directive or instruction but got %s", token_to_string(as->current_token.kind));
		next_token(as);
		return;
	}
	if (!token_match(as, TOKEN_NEWLINE) && !token_is(as, TOKEN_EOF)) {
		error_here(as, "expected newline or end of file but got %s", token_to_string(as->current_token.kind));
		next_token(as);
	}
}

/*The function connects 2 strings to one string
get 2 string and return one unified string*/
char* string_concat(const char* a, const char* b) {
	int len1;
	int len2;
	char* new_string;
	len1 = strlen(a);
	len2 = strlen(b);
	new_string = malloc(len1 + len2 + 1);
	memcpy(new_string, a, len1*sizeof(char));
	memcpy(new_string + len1, b, len2*sizeof(char));
	new_string[len1 + len2] = 0;
	return new_string;
}

/*The function opens a readable file
Gets a file name and returns a pointer to the file*/
char* read_entire_file(const char* file_name) {
	FILE* file;
	char* file_content;
	long int len;
	file = fopen(file_name, "rb");
	if(!file)
		return NULL;

	if(fseek(file, 0, SEEK_END) != 0) {
		fclose(file);
		return NULL;
	}
	len = ftell(file);
	if(len == -1) {
		fclose(file);
		return NULL;
	}
	if(fseek(file, 0, SEEK_SET) != 0) {
		fclose(file);
		return NULL;
	}
	file_content = malloc(len + 1);
	if(file_content == NULL) {
		fclose(file);
		return NULL;
	}
	if(fread(file_content, len, 1, file) != 1) {
		fclose(file);
		free(file_content);
		return NULL;
	}
	file_content[len] = 0;
	return file_content;
}

/*The function creates the machine code files of the open file
Gets an object that contains the information for writing the file and the file name*/
void output_files(Assembler* as, const char* file_name) {
	int i;
	const char* ob_file_name = string_concat(file_name, ".ob");
	FILE* file = fopen(ob_file_name, "wb");
	if(!file) {
		error_here(as, "Unable to open object file");
	}
	fprintf(file, "%6d %d", as->ic - MIN_IC_VALUE, as->dc);
	for(i = MIN_IC_VALUE; i < as->ic; ++i) {
		fprintf(file, "\n%04d %03X %c", i, (as->out_code[i - MIN_IC_VALUE].value) & ((1 << 12) - 1), as->out_code[i - MIN_IC_VALUE].are);
	}
	for(i = 0; i < as->dc; ++i) {
		fprintf(file, "\n%04d %03X A", i + as->ic, (as->out_data[i].value) & ((1 << 12) - 1));
	}
	fclose(file);
	if(as->have_entries) {
		const char* ent_file_name = string_concat(file_name, ".ent");
		FILE* entry_file = fopen(ent_file_name, "wb");
		Symbol* it_entry;
		bool is_first_entry = true;
		if(!entry_file) {
		error_here(as, "Unable to open entry file");
		}
		for(it_entry = as->symbol_table + as->symbol_count; it_entry >= as->symbol_table; --it_entry) {
			if(it_entry->entry) {
				if(!is_first_entry) {
					fprintf(entry_file, "\n");
				}
				is_first_entry = false;
				fprintf(entry_file, "%.*s %04d", it_entry->len, it_entry->name, it_entry->value);
			}
		}
		fclose(entry_file);
	}
	if(as->have_extern) {
		const char* ext_file_name = string_concat(file_name, ".ext");
		FILE* extern_file = fopen(ext_file_name, "wb");
		Symbol* it;
		bool is_first_ext = true;
		if(!extern_file) {
		error_here(as, "Unable to open extern file");
		}
		for(it = as->symbol_table; it != as->symbol_table + as->symbol_count; ++it) {
			if(it->kind == SYM_EXTERN) {
				ExternReferenceNode* node = it->extern_references;
				while(node) {
					if(!is_first_ext) {
						fprintf(extern_file, "\n");
					}
					is_first_ext = false;
					fprintf(extern_file, "%.*s %04d", it->len, it->name, node->address);
					node = node->next;
				}
			}
		}
		fclose(extern_file);
	}
}
