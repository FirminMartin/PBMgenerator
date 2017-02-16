// Firmin Martin
// 2017.02.11
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
//#define NDEBUG
#include <assert.h>
#include <argp.h>
#define BUFF_LEN 300 

const char *argp_program_version = "PBMgenerator 1.0";

struct arguments {
  char *args[2];            /* arg[1]=width and arg[2]=height */
  int verbose;              /* The -v flag */
  int number;               /* The -n flag */
  char *outfile;            /* Argument for -o */
  double bp_ratio;            /* Arguments for black pixels ratio*/
};

static struct argp_option options[] = {
  {"verbose", 'v', 0, 0, "Produce verbose output"},
  {"bpratio", 'r', "bp_ratio", 0, "The black pixels ratio that the image created will have"},
  {"output",  'o', "filename", 0,"Output to filename.pbm "},
  {"number", 'n',"number",0, "The number of image requested"},
  {0}
};

static error_t parse_opt (int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;
    
	
  switch (key) {
    case 'v': arguments->verbose = 1; break;
    case 'r': arguments->bp_ratio = strtod(arg,NULL); break; //BUG : when bp_ratio >= 0.999, the image isn't correct
    case 'o': arguments->outfile = arg; break;
    case 'n': arguments->number = atoi(arg);break;
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

static char args_doc[] = "width height";

static char doc[] = "PBMgenerator -- A program able to generate pbm image randomly with given"
" dimensions and black pixels ratio.";

static struct argp argp = {options, parse_opt, args_doc, doc};

typedef struct{
    long width;  /** width in pixels (bits) */
    long height; /** height in pixels (bits) */
} dim_t;

typedef struct{
    long long bpixels;
    long long bits;
    double local_ratio;
}remains_t;

remains_t remains;

void init_args(struct arguments *args);
void write_pbm(const char* name, dim_t dim, double bpixels_ratio);
void init_remains(dim_t dim, double bpixels_ratio);
char random_make_byte(int nb_bit);
long long sum_first(char arr[], int n);
void random_make_buffer(char buff[BUFF_LEN], int n);
int  total_bits(char x);

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

int main(int argc, char **argv){
	struct arguments arguments;
	double bp_ratio;
	int i;
    clock_t start_clock, end_clock;

    srand(time(NULL));
	init_args(&arguments);
	
    argp_parse (&argp, argc, argv, 0, 0, &arguments);
    
    dim_t dim = {atol(arguments.args[0]), atol(arguments.args[1])};
	if (!dim.width || !dim.height) {
		fprintf(stderr, RED "Error : dimensions incorrect\n" RESET);
		exit(-2);
	}

	bp_ratio = arguments.bp_ratio;
	if (bp_ratio > 1 || bp_ratio < 0) {
		fprintf(stderr, RED "Error : black pixels ratio incorrect\n" RESET);
		exit(-3);
	}

    if (arguments.number > 0){
        for(i=0;i<arguments.number;i++)  {
            if (arguments.verbose) fprintf(stdout,WHT "%3d ... " RESET, i+1);
            write_pbm(arguments.outfile, dim, bp_ratio);
            if(arguments.verbose) {
                if (clock()/CLOCKS_PER_SEC == 0) fprintf(stdout, BLU "done " YEL "%12ld"  BLU " ms\n" RESET, (clock()*1000)/CLOCKS_PER_SEC);
                else fprintf(stdout, BLU "done " YEL "%5ld" BLU " s" YEL "%5ld" BLU " ms\n", clock()/CLOCKS_PER_SEC, (clock()*1000)/CLOCKS_PER_SEC%1000);
            }
        }
    } else {
        fprintf(stderr, RED "Error : requested number incorrect\n" RESET);
        exit(-4);
    }
    
	return 0;
}

/** 
  * INPUT : the arguments structure
  * OUTPUT : NULL
  * SIDE EFFECTS : initialize arguments by each default value 
  *  */

void init_args(struct arguments *args){
	args->outfile = NULL;
	args->verbose = 0;
	args->bp_ratio = 0.5;
    args->number = 1;
}



/** 
  * INPUT : the file name, dimension struct and the black pixel ratio
  * OUPUT : NULL
  * SIDE EFFECTS : create a .pbm image with a giving name, dimensions and black pixel ration.
  *  */

void write_pbm(const char* name, dim_t dim, double bpixels_ratio){
    static int file_nb = 0; /** count the request files number to name files*/
    int write_bytes = 0;
    long long remains_bytes=0;
    char buff[BUFF_LEN]={0};
    char file_name[255]={0};
    double tmp_local_ratio = 0;
    FILE* fp;
    
    init_remains(dim, bpixels_ratio);
    if (name != NULL) {
        if (file_nb == 0) sprintf(file_name, "%s.pbm", name);
        else sprintf(file_name, "%s_%d.pbm", name, file_nb);
    }
	else sprintf(file_name, "%ldx%ld-%.2f_%d.pbm",dim.width, dim.height, bpixels_ratio, file_nb);

    fp = fopen(file_name,"wb");
    if(!fp){
        perror("write_pbm() Error ");
        exit(-1);
    }
    /* write header*/
    sprintf(buff, "P4\n%ld %ld\n", dim.width, dim.height); 
    fwrite(buff, sizeof(char),strlen(buff), fp);
    do{
        /** Calculate exactly the right nb of bytes */
        remains_bytes = (remains.bits <= 0) ? 0 : ((remains.bits % 8 == 0) ? remains.bits/8 : remains.bits/8 + 1); 
        if (remains_bytes == 0) break;
        write_bytes = (remains_bytes > BUFF_LEN) ? BUFF_LEN : remains_bytes;
        random_make_buffer(buff, write_bytes);
        fwrite(buff, sizeof(char), write_bytes, fp);
        remains.bits -= 8*write_bytes;

        tmp_local_ratio = remains.local_ratio;
        if (remains.bpixels == 0) remains.local_ratio = 0;
        remains.local_ratio = remains.bpixels / (remains.bits + 0.0000000000000000001); /** prevent division by 0 */
        if (remains.local_ratio < tmp_local_ratio) remains.local_ratio *= (0.01*(rand()%5)); //TODO modify the local ratio algo 
    }while(remains_bytes > 0);
    fclose(fp);
    file_nb++;
}


/** 
  * INPUT : the buffer (array of char) and the number of byte we want to randomize
  * OUTPUT : NULL
  * SIDE EFFECTS : randomize the first n bytes of the buffer 
  *  */

void random_make_buffer(char buff[BUFF_LEN], int n){
    assert(BUFF_LEN >= n);
    assert(remains.bits != 0);
    long long tmp_v = 0; 
    int i, tmp_i = 0;
    long long sum = 0;
    for(i = 0; i < n; i++){ /** randomly init the first n bytes of the buffer */
        buff[i] = rand()%256;
        sum += total_bits(buff[i]);  /** calculate the number of '1' the first n bytes of the buffer containing */
    }
    if (sum > 8*n*remains.local_ratio ){
        do{ 
            /** randomly choose a non-null byte and decrease its number of '1' by 1*/
            tmp_i = rand()%n;
            tmp_v = total_bits(buff[tmp_i]);
            if (tmp_v == 0) continue;  /** is a null byte*/
            buff[tmp_i] = random_make_byte(tmp_v - 1);
            sum-=1;
        }while(sum > (int)8*n*remains.local_ratio);
    }
    else {
        do{
            tmp_i = rand()%n;
            tmp_v = total_bits(buff[tmp_i]);
            if (tmp_v == 8) continue;
            buff[tmp_i] = random_make_byte(tmp_v + 1);
            sum+=1;
        }while(sum < (int)8*n*remains.local_ratio);
    }
    remains.bpixels -= sum;
}

/** 
  * INPUT : number of '1' bit we want to the byte have
  * OUTPUT : a char (byte) constituted by nb_bit '1' and sizeof(char)-nb_bit '0'
  *
  *  */

char random_make_byte(int nb_bit){ 
    char bit[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
    int i;
    char arr[8]={0}, byte = 0;
    if (nb_bit >= 8) return 0x8F;  /** return 1111 1111 */
    while(sum_first(arr, 8)!=nb_bit){
        for (i=0;i<8;i++) arr[i] = rand()%2;
    }
    for(i=0;i<8;i++) byte += arr[i]*bit[i];
    return byte;
}

/** 
  * INPUT : an char array and the number of char we want to sum
  * OUTPUT : the sum of the first n char
  * 
  *  */

long long sum_first(char arr[], int n){
    int i = 0;
    long long sum = 0;
    for (i=0;i<n;i++) sum += arr[i];
    return sum;
}


/** 
  * INPUT : dimension structure, black pixel ratio
  * OUTPUT : NULL
  * SIDE EFFECTS : initialization of the global structure remains.
  *  */

void init_remains(dim_t dim, double bpixels_ratio){
    remains.bpixels = dim.width * dim.height * bpixels_ratio; /** init remains black pixels*/
    remains.bits = dim.width*dim.height; /** init remains bits */
    remains.local_ratio = bpixels_ratio;  /** init the local black pixels ratio */
}

/** 
  * INPUT : A character
  * OUTPUT : The sum of bit constituting the char
  *
  *  */

int total_bits(char x){
    int i,k=0x80, sum = 0;
    for (i=0;i<8;i++){
        sum += (x & k)/k;
        k = k >> 1;
    }
    return sum;
}


