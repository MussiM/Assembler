#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include "Tokens.h"

struct Symbol;

/*Used to store a single word machine code
value is code and are is A\R\E*/
typedef struct Codes {
	int value;
	char are;
} Codes;

/*Used to store a global variables*/
typedef struct Assembler {
	int dc;
	int ic;
	bool is_second_pass;
	const char* file_name;
	const char* stream;
	const char* start;
	int line;
	struct Symbol* symbol_table;
	int symbol_count;
	int symbol_allocated;
	bool have_entries;
	bool have_extern;
	bool have_error;
	Token current_token;
	Codes out_data[1024 * 4];
	Codes out_code[1024 * 4];
} Assembler;

void init_assembler(void);
void parse_statement(Assembler* as);
char* string_concat(const char* a, const char* b);
char* read_entire_file(const char* file_name);
void output_files(Assembler* as, const char* file_name);
void output_data(Assembler* as, int value, char are);
void output_word(Assembler* as, int value, char are);

