#pragma once
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define MIN_IC_VALUE 100

/*enum to kind's words*/
typedef enum TokenKind {
	TOKEN_NONE,
	TOKEN_IMMEDIATE,
	TOKEN_INT,
	TOKEN_STRING,
	TOKEN_LABEL,
	TOKEN_LABEL_DEF,
	TOKEN_RELATIVE,
	TOKEN_COMMA,
	TOKEN_REG0,
	TOKEN_FIRST_REG = TOKEN_REG0,
	TOKEN_REG1,
	TOKEN_REG2,
	TOKEN_REG3,
	TOKEN_REG4,
	TOKEN_REG5,
	TOKEN_REG6,
	TOKEN_REG7,
	TOKEN_LAST_REG = TOKEN_REG7,
	TOKEN_INST_MOVE,
	TOKEN_FIRST_INST = TOKEN_INST_MOVE,
	TOKEN_INST_CMP,
	TOKEN_INST_ADD,
	TOKEN_INST_SUB,
	TOKEN_INST_LEA,
	TOKEN_INST_CLR,
	TOKEN_INST_NOT,
	TOKEN_INST_INC,
	TOKEN_INST_DEC,
	TOKEN_INST_JMP,
	TOKEN_INST_BNE,
	TOKEN_INST_JSR,
	TOKEN_INST_RED,
	TOKEN_INST_PRN,
	TOKEN_INST_RTS,
	TOKEN_INST_STOP,
	TOKEN_LAST_INST = TOKEN_INST_STOP,
	TOKEN_NEWLINE,
	TOKEN_DIR_DATA,
	TOKEN_FIRST_DIR = TOKEN_DIR_DATA,
	TOKEN_DIR_STRING,
	TOKEN_DIR_EXTERN,
	TOKEN_DIR_ENTRY,
	TOKEN_LAST_DIR = TOKEN_DIR_ENTRY,
	TOKEN_EOF
} TokenKind;

/*A struct that contains the current word:
kind's word
character of word
length of word
address in memmeory*/
typedef struct Token {
	TokenKind kind;
	const char* str;
	int len;
	int int_value;
} Token;

struct Assembler;

extern const char* instructions_names[TOKEN_LAST_INST - TOKEN_FIRST_INST + 1];
extern const char* directives_names[TOKEN_LAST_DIR - TOKEN_FIRST_DIR + 1];

void init_assemble_file(struct Assembler* as, const char* code, const char* file_name);
TokenKind string_to_token_kind(const char* str, int len);
char* token_to_string(TokenKind token);
void next_token(struct Assembler* as);
bool token_is(struct Assembler* as, TokenKind k);
bool token_match(struct Assembler* as, TokenKind k);
bool token_expect(struct Assembler* as, TokenKind kind);
void init_second_pass(struct Assembler* as);
void error_here(struct Assembler* as, const char* fmt, ...);
