#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <argp.h>

#include "pbm.h"
#include "method.h"
#include "common.h"

/*** argp related structure/function ***/

const char *argp_program_version = "pbm-generator 1.0";
static char doc[] = "PBMgenerator -- A program able to generate pbm image randomly with given"
" dimensions and black pixels ratio.";
static char args_doc[] = "width height";

static struct argp_option options[] = {
  {"verbose", 'v', 0, 0, "Produce verbose output"},
  {"bpratio", 'r', "bp_ratio", 0, "The black pixels ratio that the image created will have"},
  {"output",  'o', "filename", 0,"Output to filename.pbm"},
  {"quantity", 'n',"quantity", 0, "The quantity of image requested"},
  {0}
};

static error_t parse_opt (int key, char *arg, struct argp_state *state) {

  arg_t *arguments = state->input;
    	
  switch (key) {
    case 'v': arguments->verbose = 1; break;
    case 'r': arguments->bp_ratio = strtod(arg,NULL); break; 
    case 'o': arguments->outfile = arg; break;
    case 'n': arguments->quantity = atoi(arg);break;
    case ARGP_KEY_ARG:
      if (state->arg_num >= 2) argp_usage(state);
	  arguments->args[state->arg_num] = arg;
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 2) argp_usage (state);
      break;
    default: return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};


arg_t arguments;


funCrtByte fun_create_byte;

int main(int argc, char** argv){
	srand(time(NULL));
    dim_t dim;
    char filename [BUFF_LEN] = {0};
    int file_counter = 0;

	/* options default value */

	arguments.verbose = 1;
	arguments.quantity = 1;
	arguments.outfile = NULL;
    arguments.bp_ratio = 50.0;

	/* arguments parsing */

	argp_parse (&argp, argc, argv, 0, 0, &arguments);

    /* Reading dimensions */

	dim.width = atol(arguments.args[0]);
    dim.height = atol(arguments.args[1]);

	if (!dim.width || !dim.height) {
		fprintf(stderr, "Error : dimensions incorrect\n" );
		exit(EXIT_FAILURE);
	}

    /* chose create byte method 
     * TODO: add more methods */

    fun_create_byte = &rand_create_byte;

    for (file_counter = 0; file_counter < arguments.quantity; file_counter++){

        rand_name(arguments.outfile, filename, file_counter, dim);    
        write_pbm(filename, dim);

    }
	return 0;
}
