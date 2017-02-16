// Firmin Martin
// 2017.02.11
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
//#define NDEBUG
#include <assert.h>
#define BUFF_LEN 300 //SEGFAULT for -O3

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

void write_pbm(const char* name, dim_t* dim, double bpixels_ratio);
void init_remains(dim_t dim, double bpixels_ratio);
char random_make_byte(int nb_bit);
long long sum_first(char arr[], int n);
void print_byte(char x);
void random_make_buffer(char buff[BUFF_LEN], int n);
int  total_bits(char x);

int main(){
    srand(time(NULL));
    dim_t dim;
    int i;
    dim.width = 80;
    dim.height = 50;
    printf("start !\n");
    write_pbm("tst", &dim, 0.01 );
    return 0;
}

/** 
  * INPUT : the file name, dimension struct and the black pixel ratio
  * OUPUT : NULL
  * SIDE EFFECTS : create a .pbm image with the giving name, dimension and black pixel ration.
  *  */

void write_pbm(const char* name, dim_t* dim, double bpixels_ratio){
    static int file_nb = 0; /** count the request files number to name files*/
    int write_bytes = 0;
    long long remains_bytes=0;
    char buff[BUFF_LEN]={0};
    char file_name[255]={0};
    double tmp_local_ratio = 0;
    FILE* fp;
    
    init_remains(*dim, bpixels_ratio);
    
    sprintf(file_name, "%s-%ldx%ld-%.2f_%d.pbm", name,dim->width, dim->height, bpixels_ratio, file_nb);
    fp = fopen(file_name,"wb");
    if(!fp){
        perror("write_pbm() Error ");
        exit(-1);
    }
   
    /* write header*/
    sprintf(buff, "P4\n%ld %ld\n", dim->width, dim->height);
    fwrite(buff, sizeof(char),strlen(buff), fp);
printf("init:remains.bits = %lld, remains_bytes = %lld\n",remains.bits, remains_bytes);   
    do{
printf("before:remains.bits = %lld, remains_bytes = %lld\n",remains.bits, remains_bytes);
        remains_bytes = (remains.bits <= 0) ? 0 : ((remains.bits % 8 == 0) ? remains.bits/8 : remains.bits/8 + 1);
        if (remains_bytes == 0) break;
        write_bytes = (remains_bytes > BUFF_LEN) ? BUFF_LEN : remains_bytes;
        random_make_buffer(buff, write_bytes);
        assert(write_bytes == fwrite(buff, sizeof(char), write_bytes, fp));
        remains.bits -= 8*write_bytes;

        tmp_local_ratio = remains.local_ratio;
        if (remains.bpixels == 0) remains.local_ratio = 0;
        remains.local_ratio = remains.bpixels / (remains.bits + 0.0000000000000000001);
        if (remains.local_ratio < tmp_local_ratio) remains.local_ratio *= (0.01*(rand()%5)); //TODO
printf("after:remains.bits = %lld, remains_bytes = %lld\n",remains.bits, remains_bytes);
    }while(remains.bits > 0);
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
    int i , tmp_i = 0;
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
  * INPUT : a character
  * OUTPUT : NULL
  * SIDE EFFECTS : print a char as its byte form
  *  */

void print_byte(char x){
    int i,k=0x80;
    for (i=0;i<8;i++){
        printf("%d",(x & k)/k);
        k = k >> 1;
    }
    printf(" ");
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


