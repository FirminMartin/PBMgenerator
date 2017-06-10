#ifndef PBM_H
#define PBM_H

typedef struct{long width; long height;} dim_t;
typedef char (*funCrtByte)();

extern funCrtByte fun_create_byte;

char make_byte(char* byte);
int write_pbm (char* out_file, dim_t dim);

#endif
