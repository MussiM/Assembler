#pragma once
#include <stdbool.h>
#include "Tokens.h"

struct Assembler;

bool is_inst(TokenKind k);
void out_instruction(struct Assembler* as, int op, int funct, int am_src, int am_dst);
void parse_mov(struct Assembler* as);
void parse_cmp(struct Assembler* as);
void parse_add(struct Assembler* as);
void parse_sub(struct Assembler* as);
void parse_lea(struct Assembler* as);
void parse_clr(struct Assembler* as);
void parse_not(struct Assembler* as);
void parse_inc(struct Assembler* as);
void parse_dec(struct Assembler* as);
void parse_jmp(struct Assembler* as);
void parse_bne(struct Assembler* as);
void parse_jsr(struct Assembler* as);
void parse_red(struct Assembler* as);
void parse_prn(struct Assembler* as);
void parse_rts(struct Assembler* as);
void parse_stop(struct Assembler* as);
