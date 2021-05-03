#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Symbols.h"
#include "Assembler.h"

/*The function adds new symbols to the dynamic array
Gets struct that contains the symbol and return the new symbol*/
Symbol* symbol_add_unchecked(Assembler* as) {
	int i;
	if((as->symbol_count) + 1 >= as->symbol_allocated){
		Symbol* new_symbol;
		as->symbol_allocated = (as->symbol_allocated)*2 > 8 ? (as->symbol_allocated)*2 : 8;
		new_symbol = calloc(sizeof(Symbol), as->symbol_allocated);
		for(i = 0; i < as->symbol_count; i++){
			new_symbol[i] = as->symbol_table[i];
		}
			free(as->symbol_table);
			as->symbol_table = new_symbol;
	}
	(as->symbol_table)[as->symbol_count].name = as->current_token.str;
	(as->symbol_table)[as->symbol_count].len = as->current_token.len;
	(as->symbol_table)[as->symbol_count].kind = SYM_UNRESOLVED;
	(as->symbol_table)[as->symbol_count].value = 0;
	(as->symbol_table)[as->symbol_count].entry = false;
	return &(as->symbol_table[(as->symbol_count)++]);
}

/*The function adds symbols to the dynamic array just if they are not exists
Gets struct that contains the symbol and return the symbol
if is already exists print error*/
Symbol* symbol_add(Assembler* as) {
	Symbol* sym;
	for(sym = as->symbol_table; sym != (as->symbol_table) + (as->symbol_count); ++sym) {
		if(sym->len == as->current_token.len && strncmp(sym->name, as->current_token.str, as->current_token.len) == 0){
			if(sym->kind != SYM_UNRESOLVED){
				error_here(as, "The symbol %.*s already exists\n", as->current_token.len, as->current_token.str);
			}
			return sym;
		}
	}
	return symbol_add_unchecked(as);
}

/*The function gets symbols
Gets struct that contains the symbol and return the symbol from symbols array
if he is not exists it will be add
if he is exists but unknown what he is kind and it is in second pass print error*/
Symbol* symbol_get(Assembler* as){
	Symbol* sym;
	for(sym = as->symbol_table; sym != (as->symbol_table) + (as->symbol_count); ++sym) {
		if(sym->len == as->current_token.len && strncmp(sym->name, as->current_token.str, as->current_token.len) == 0){
			if(as->is_second_pass && sym->kind == SYM_UNRESOLVED){
				printf("%s:%d error: unresolved symbol %.*s\n", as->file_name, as->line, as->current_token.len, as->current_token.str);
				as->have_error = true;
			}
			return sym;
		}
	}
	return symbol_add_unchecked(as);
}

/*The function parsed the directive string line
Gets struct that contains the first word of the directive line*/
void parse_string(Assembler* as, Symbol* symbol) {
	Token str_token;
	int i;	
	next_token(as);
	if(symbol && !as->is_second_pass) {
		symbol->kind = SYM_DATA;
		symbol->value = as->dc;
	}
	str_token = as->current_token;
	if(token_expect(as, TOKEN_STRING)) {
		for(i = 0; i < str_token.len; ++i) {
			output_data(as, as->current_token.str[i], 'A');
		}
		output_data(as, '\0', 'A');
	}
}

/*The function parsed the directive data line
Gets struct that contains the first word of the directive line*/
void parse_data(Assembler* as, Symbol* symbol) {
	Token int_token;
	next_token(as);
	if(symbol && !as->is_second_pass) {
		symbol->kind = SYM_DATA;
		symbol->value = as->dc;
	}
	do {
		int_token = as->current_token;
		if(!token_expect(as, TOKEN_INT)) {
			break;
		}
		output_data(as, int_token.int_value, 'A');
	} while(token_match(as, TOKEN_COMMA));
}

/*The function gets the symbol of directive line
Gets struct that contains the name's symbol and return the symbol from symbol array*/
Symbol* parse_symbol(Assembler* as) {
	Symbol* sym;
	if(token_is(as, TOKEN_LABEL)) {
		sym = symbol_get(as);
		next_token(as);
		return sym;
	}
	return NULL;
}

/*The function parsed the directive entry line
Gets struct that contains the first word of the directive line and the symbol
Changes the entry flag's symbol to true*/
void parse_entry(Assembler* as, Symbol* symbol) {
	assert(token_is(as, TOKEN_DIR_ENTRY));
	as->have_entries = true;
	next_token(as);
	if(symbol) {
		error_here(as, "Symbol before 'entry' directive");
	}
	symbol = parse_symbol(as);
	symbol->entry = true;
}

/*The function parsed the directive extern line
Gets struct that contains the first word of the directive line and the symbol
Changes the kind's symbol to extern*/
void parse_extern(Assembler* as, Symbol* symbol) {
	assert(token_is(as, TOKEN_DIR_EXTERN));
	as->have_extern = true;
	next_token(as);
	if(symbol) {
		error_here(as, "Symbol before 'extern' directive");
	}

	symbol = parse_symbol(as);
	if(!symbol) {
		error_here(as, "No has Symbol after 'extern' directive");
	}
	symbol->kind = SYM_EXTERN;
}

void fix_symbol_value(Assembler* as) {
	Symbol* sym;
	for(sym = as->symbol_table; sym != (as->symbol_table) + (as->symbol_count); ++sym){
		if(sym->kind == SYM_DATA){
			sym->value = (sym->value) + (as->ic);
		}
	}
}

/*The function adds to the extern symbol the address where he is written
Gets symbol and adress memmory*/
void add_extern_refernce(Symbol* symbol, int address) {
	if(symbol->extern_references == NULL) {
		symbol->extern_references = malloc(sizeof(ExternReferenceNode));
		symbol->extern_references->address = address;
		symbol->current_extern_references = symbol->extern_references;
	} else {
		symbol->current_extern_references->next = malloc(sizeof(ExternReferenceNode));
		symbol->current_extern_references->next->address = address;
		symbol->current_extern_references = symbol->current_extern_references->next;
	}
	symbol->current_extern_references->next = NULL;
}
