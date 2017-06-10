#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pbm.h"
#include "common.h"


int write_pbm (char* out_file, dim_t dim){
    int i;
    static int file_nb = 0; /** count remains files number to name files*/
    char buff[BUFF_LEN]	= {0};
    char byte;
    FILE* fp; 

    fp = fopen(out_file,"wb");
    if(!fp){
        perror("write_pbm() Error ");
        exit(EXIT_FAILURE);
    }

    /*write header*/
    sprintf(buff, "P4\n%ld %ld\n", dim.width, dim.height); 
    fwrite(buff, sizeof(char),strlen(buff), fp);

    for (i = 0; i < (dim.width*dim.height)/8; i++) { 
        byte = fun_create_byte();
        fwrite(&byte, sizeof(char), 1, fp);
    }
    fclose(fp);
	return 0;
}


char make_byte(char* byte){
	int i,tmp = 1;
	char b = 0;
	
	for(i=0;i<8;i++){
		b += tmp*byte[i];
		tmp = tmp << 1;
	}
	return b;
}
