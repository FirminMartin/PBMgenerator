#ifndef BYTE_H
#define BYTE_H

#include <stdio.h>
#include <stdlib.h>

#include "pbm.h"

char rand_create_byte();
void rand_name(char* filename, char* new_filename, int file_counter, dim_t dim);
#endif
