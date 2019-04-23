/* This is main file for decaf compiler: dcc
 * Author: Tianyi Liu
 * Email: liuty10@gmail.com
*/

/*
 Goal : Design a scanner to deal with decaf source code into TOKON.
 Task1: read source code from files line by line.
 Task2: for each line, read characters one by one to split the whole line into pre-TOKONS.
 Task3: For each pre-token, you need further process to get the final TOKON.
 Task4: Write the corresponding tokon back to a result file.
*/

/*
 How to use dcc (decaf gcc)?
 Goal: it should be simallar to gcc.
 Example: dcc -t input -o [output]

*/

#include"parser.h"
extern void pre_processor(FILE* source, FILE* dest);

void print_usage(){
	printf("Usage: ./dcc -t -i inputfile -o [outputfile]\n");
	printf("\n");
	return;
}
struct option long_options[] = {
	{"tokon", 	no_argument, 	  0, 't'}, // generate token file
	{"input", 	required_argument, 0, 'i'}, // input file
	{"output",	required_argument, 0, 'o'}, // output file
	{"help",	no_argument, 	  0, 'h'}, // output file
	{0, 0, 0, 0}
};
int main(int argc, char* argv[]){
	int option;
	int token_flag = 0;
	char *source_file_name = "";
	char *processed_file_name = "tmp.out";
	char *output_file_name = "a.out";

	FILE *source_file = NULL;
	FILE *processed_file = NULL;
	FILE *output_file = NULL;

    	while(1){
		option = getopt_long(argc, argv, "ht:i:o:", long_options, NULL);
		if(option == -1) break;
		switch(option){
		case 'h':
			print_usage();
			exit(0);
		case 'i':
			source_file_name = strdup(optarg);
			break;
		case 'o': 
			output_file_name = strdup(optarg);
			break;
		case 't':
			token_flag = 1;
			printf("token_flag is 1\n");
			break;
		default:
			printf("Unknow argument: %d\n", option);
			print_usage();
			exit(0);
		}
    	}
	source_file 	= fopen(source_file_name, "r");
	processed_file 	= fopen(processed_file_name, "w+");
	pre_processor(source_file, processed_file);
	rewind(processed_file);

        Program *prog = (Program*)new Program();
        //prog->parseProgram(processed_file);

        if(!prog->parseProgram(processed_file)){
             printf("Parse Errors.\n");
        }else{
             prog->printAST();
        }
	fclose(source_file);
	fclose(processed_file);
    	return 0;
}

