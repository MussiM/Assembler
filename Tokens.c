#include <stdio.h>
#include <stdarg.h>
#include "Symbols.h"
#include "Assembler.h"

const char* instructions_names[TOKEN_LAST_INST - TOKEN_FIRST_INST + 1] = {0};

const char* register_names[] = {
	"r0",
	"r1",
	"r2",
	"r3",
	"r4",
	"r5",
	"r6",
	"r7"
};

const char* directives_names[TOKEN_LAST_DIR - TOKEN_FIRST_DIR + 1] = {0};

/*The function converts a string to a more specific word type
Gets first char of the word and length*/
TokenKind string_to_token_kind(const char* str, int len) {
	int i = 0;
	for (i = 0; i < sizeof(instructions_names) / sizeof(*instructions_names); ++i) {
		if (strlen(instructions_names[i]) == len && strncmp(str, instructions_names[i], len) == 0) {
			return i + TOKEN_FIRST_INST;
		}
	}
	
	for (i = 0; i < sizeof(register_names) / sizeof(*register_names); ++i) {
		if (strlen(register_names[i]) == len && strncmp(str, register_names[i], len) == 0) {
			return TOKEN_FIRST_REG + i;
		}
	}

	for (i = 0; i < sizeof(directives_names) / sizeof(*directives_names); ++i) {
		if (strlen(directives_names[i]) == len && strncmp(str, directives_names[i], len) == 0) {
			return TOKEN_FIRST_DIR + i;
		}
	}

	return TOKEN_LABEL;
}

/*The function converts a number kind to string
Gets number kind and return string*/
char* token_to_string(TokenKind token){
switch (token){
	case 0:
		return "none";
		break;
	case 1:
		return "int operand";
		break;
	case 2:
		return "integer";
		break;
	case 3:
		return "string";
		break;
	case 4:
	case 5:
		return "lable";
		break;
	case 6:
		return "lable operand";
		break;
	case 7:
		return "comma";
		break;
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
		return "register";
		break;
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
		return "instruction";
		break;
	case 32:
		return "newline";
		break;
	case 33:
	case 34:
	case 35:
	case 36:
		return "directive";
		break;
	case 37:
		return "end of file";
		break;
	default:
		return "unknown word";
		break;
}
}

/*The function converts a string to a integer
Gets struct tha contains the string and return integer*/
int parse_int_value(Assembler* as) {
	bool is_negative = *as->stream == '-';
	int value = 0;
	if(is_negative) {
		++as->stream;
	}
	while (isdigit(*as->stream)) {
		value *= 10;
		value += *as->stream - '0';
		++as->stream;
	}
	return is_negative ? -value : value;
}

/*The function reports errors during code
Gets struct that contains the file name and line and string to print*/
void error_here(Assembler* as, const char* fmt, ...) {
	if(!as->is_second_pass){
	va_list args;
	as->have_error = true;
	printf("%s:%d error: ", as->file_name, as->line);
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf("\n");
	}
}

/*The function resets the variables to the second pass
Gets struct that contains the variables*/
void init_second_pass(Assembler* as) {
	as->is_second_pass = true;
	fix_symbol_value(as);
	as->dc = 0;
	as->ic = MIN_IC_VALUE;
	as->stream = as->start;
	as->line = 1;
	as->current_token.kind = TOKEN_NONE;
}

/*The function resets the variables to the next input file
Gets struct that contains the variables, pointer to start of file and file name*/
void init_assemble_file(Assembler* as, const char* code, const char* file) {
	as->start = code;
	as->stream = code;
	as->file_name = file;
	as->line = 1;
	as->dc = 0;
	as->ic = MIN_IC_VALUE;
	as->have_extern = false;
	as->have_entries = false;
	as->have_error = false;
	as->is_second_pass = false;
	as->symbol_count = 0;
	as->symbol_allocated = 0;
	as->current_token.kind = TOKEN_NONE;
	if(as->symbol_table){
		free(as->symbol_table);
	}
	as->symbol_table = NULL;
}

/*The function searches for the next significant word
Gets a structure that contains the current location in the file*/
void next_token(Assembler* as) {
	repeat:
	switch (*as->stream) {
	case '\0':
		as->current_token.kind = TOKEN_EOF;
		break;
	case ';':
		if(as->current_token.kind != TOKEN_NEWLINE && as->current_token.kind != TOKEN_NONE) {
			error_here(as, "Comment must be in newline");
		}
		while(*as->stream != '\n' && *as->stream != 0){
			++as->stream;
		}
		goto repeat;
	case '\n':
	case ' ':
	case '\v':
	case '\t':
	case '\r': {
		bool is_newline = false;
		while (isspace(*as->stream)) {
			if(*as->stream == '\n'){
				is_newline = true;
				++as->line;
			}
			++as->stream;
		}
		if (!is_newline) {
			goto repeat;
		}
		as->current_token.kind = TOKEN_NEWLINE;
		break;
	}
	case '-':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9': {
		as->current_token.kind = TOKEN_INT;
		as->current_token.int_value = parse_int_value(as);
		break;
	}		
	case '#': {
		++as->stream;
		as->current_token.kind = TOKEN_IMMEDIATE;
		as->current_token.int_value = parse_int_value(as);
		break;
	}
	case '"': {
		const char* start = ++as->stream;
		while(*as->stream != '"' && *as->stream != 0) {
			++as->stream;
		}
		if(*as->stream == 0) {
			error_here(as, "The string need finished with \" ");
		}
		as->current_token.kind = TOKEN_STRING;
		as->current_token.str = start;
		as->current_token.len = as->stream - start;
		++as->stream;
		break;
	}
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':
	case '_':
	case '.':  {
		const char* start = as->stream;
		do {
			++as->stream;
		} while (isalnum(*as->stream) || *as->stream == '_') ;

		as->current_token.str = start;
		as->current_token.len = as->stream - start;
		as->current_token.kind = string_to_token_kind(as->current_token.str, as->current_token.len);
		
		if(*as->stream == ':')
		{
			++as->stream;
			if(as->current_token.kind != TOKEN_LABEL) {
				error_here(as, "didn't get lable but got %s", token_to_string(as->current_token.kind));
			}
			as->current_token.kind = TOKEN_LABEL_DEF;
		}	
		break;
	}
	case ',':
		++as->stream;
		as->current_token.kind = TOKEN_COMMA;
		break;
	case '%':
		++as->stream;
		as->current_token.kind = TOKEN_RELATIVE;
		break;
	default:
		printf("%s:%d error: unexpected character %c\n", as->file_name, as->line, *as->stream);
		as->have_error = true;
		++as->stream;
		goto repeat;
	}
}

/*The function checks if the word type matches the desired type
Gets a structure that contains the kind of current word and expected kind
return true\false */
bool token_is(Assembler* as, TokenKind k) {
	return as->current_token.kind == k;
}

/*The function checks if the word type matches the desired type
Gets a structure that contains the kind of current word and expected kind
if it's true Continue to the next word
return true\false */
bool token_match(Assembler* as, TokenKind k) {
	if (token_is(as, k)) {
		next_token(as);
		return true;
	}
	return false;
}

/*The function checks if the word type matches the desired type
Gets a structure that contains the kind of current word and expected kind
if it's false print error
if it's true continue to the next word
return true\false */
bool token_expect(Assembler* as, TokenKind kind) {
	if(as->current_token.kind != kind) {
		error_here(as, "didn't get %s but got %s", token_to_string(kind), token_to_string(as->current_token.kind));
		return false;
	} else {
		next_token(as);
		return true;
	}
}
