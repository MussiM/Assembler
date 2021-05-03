#pragma once
#include <stdbool.h>
#include <stdlib.h>

/*enum for symbol kinds*/
typedef enum SymbolKind{
	SYM_UNRESOLVED,
	SYM_DATA,
	SYM_CODE,
	SYM_EXTERN
}SymbolKind;

/*A linked list of addresses to extern symbols*/
typedef struct ExternReferenceNode {
	struct ExternReferenceNode* next;
	int address;
} ExternReferenceNode;

/*A structure containing the details of symbols*/
typedef struct Symbol{
	const char* name;
	int len;
	SymbolKind kind;
	int value;
	bool entry;
	ExternReferenceNode* extern_references;
	ExternReferenceNode* current_extern_references;
} Symbol;

struct Assembler;

void add_extern_refernce(Symbol* symbol, int address);

Symbol* symbol_add(struct Assembler* as);
Symbol* symbol_get(struct Assembler* as);
void parse_string(struct Assembler* as, Symbol* symbol);
void parse_data(struct Assembler* as, Symbol* symbol);
void parse_entry(struct Assembler* as, Symbol* symbol);
void parse_extern(struct Assembler* as, Symbol* symbol);
void fix_symbol_value(struct Assembler* as);
