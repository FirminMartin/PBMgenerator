#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "pbm.h"

char rand_create_byte(){
	int i;
    int ratio = (int)(100 * arguments.bp_ratio);
    char byte[8] = {0};

	for(i = 0;i < 8;i++){
		byte[i] = (rand() % 10001 >= ratio) ? 0 : 1 ;
    }
	return make_byte(byte);
}

void rand_name(char* filename, char* new_filename, int file_counter, dim_t dim){
    
    if (filename != NULL) {
        if (file_counter == 0) sprintf(new_filename, "%s.pbm", filename);
        else sprintf(new_filename, "%s_%d.pbm", filename, file_counter);
    }
	else sprintf(new_filename, "%ldx%ld-%.2f%%_%d.pbm",dim.width, dim.height, arguments.bp_ratio, file_counter);
}
