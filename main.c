#include <stdio.h>
#include <stdlib.h>
#include "Assembler.h"

/*The assembler interprets the code line by line.
First Passage: Means all code with unexplained symbols.
Second Passage: Arranging the address of the symbols.
Create the appropriate files for the input file*/

int main(int argc, char** argv) {
	int i;
	Assembler as = {0};  	
	if(argc < 2) {
		printf("no provided input files\n");
		return 0;
	}
	init_assembler(); 
	for(i = 1; i < argc; ++i) {
		const char* as_file = string_concat(argv[i], ".as");
		const char* code = read_entire_file(as_file);
		if(!code) {
			error_here(&as, "This file couldn't be read");
		}
		init_assemble_file(&as, code, as_file);
		next_token(&as);
		while (!token_is(&as, TOKEN_EOF)) {
			parse_statement(&as);	
		}
		init_second_pass(&as);
		next_token(&as);
		while (!token_is(&as, TOKEN_EOF)) {			
			parse_statement(&as);
		}
		if(!as.have_error){
			output_files(&as, argv[i]);
		}
	}
	return 0;
}
