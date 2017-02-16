// Firmin Martin
// 2017.02.11
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#define BUFF_LEN 300 //SEGFAULT for -O3

typedef struct{
    long width;
    long height;
} dim_t;

typedef struct{
    long long bpixels;
    long long remains_bits;
    double local_ratio;
}remains_t;

remains_t remains;

void write_pbm(const char* name, dim_t* dim, double bpixels_ratio);
void init_remains(dim_t dim, double bpixels_ratio);
char random_make_byte(int nb_bit);
int sum_first(char arr[], int n);
void print_byte(char x);
void random_make_sum(char buff[BUFF_LEN], int n);
int  total_bit(char x)

int main(){
    srand(time(NULL));
    dim_t dim;
    int i;
    for (i=500;i<=500;i++){
        printf("%d : ...", i);
        dim.height = i;
        dim.width = i+5;
        write_pbm("tst", &dim, 0.01);
        printf(" done\n");
    }


    //write_pbm("tst", &dim, 0.05);
    return 0;
}

/** 
  * INPUT : the file name, dimension struct and the black pixel ratio
  * OUPUT : NULL
  * SIDE EFFECTS : create a .pbm image with the giving name, dimension and black pixel ration.
  *  */

void write_pbm(const char* name, dim_t* dim, double bpixels_ratio){
    static int file_nb = 0;
    int write_bits = 0;
    long long remains_bytes=0;
    char buff[BUFF_LEN]={0};
    char new_name[255]={0};
    double tmp_local_ratio = 0;
    FILE* fp;
    init_remains(*dim, bpixels_ratio);
    sprintf(new_name, "%s-%ldx%ld-%.2f_%d.pbm", name,dim->width, dim->height, bpixels_ratio, file_nb);
    fp = fopen(new_name,"wb");
    if(!fp){
        perror("write_pbm() Error ");
        exit(-1);
    }
    /* write header*/
    sprintf(buff, "P4\n%ld %ld\n", dim->width, dim->height);
    fwrite(buff, nof(char),strlen(buff), fp);
    do{
        remains_bytes = remains.remains_bits/8;
        write_bits = (remains_bytes > BUFF_LEN) ? BUFF_LEN : (!remains_bytes ? 1 : remains_bytes);



        random_make_sum(buff, write_bits);

        remains.remains_bits -= 8*fwrite(buff, nof(char), write_bits, fp);

        tmp_local_ratio = remains.local_ratio;
        if (remains.bpixels == 0) remains.local_ratio = 0;
        remains.local_ratio = remains.bpixels / (remains.remains_bits + 0.0000000000000000001);
        if (remains.local_ratio < tmp_local_ratio) remains.local_ratio *= (0.01*(rand()%5)); //TODO

//system("pause");
    }while(remains_bytes  > 0);
    fclose(fp);
    file_nb++;
}


/** 
  * INPUT : the buffer (array of char) and the number of byte we want to randomize
  * OUTPUT : NULL
  * SIDE EFFECTS : randomize the first n bytes of the buffer 
  *  */

void random_make_sum(char buff[BUFF_LEN], int n){
    long long i, tmp_v, tmp_i;
    long long sum = 0;
    for(i = 0; i < n; i++){
        buff[i] = rand()%256;
        sum += total_bit(buff[i]);
    }
    if (sum > 8*n*remains.local_ratio ){
        do{
            tmp_i = (rand()*rand()*rand())%n;
            tmp_v = total_bit(buff[tmp_i]);
            if (tmp_v == 0) continue;
            buff[tmp_i] = random_make_byte(tmp_v - 1);
            sum-=1;
        }while(sum > 8*n*remains.local_ratio && sum - 1 > 8*n*remains.local_ratio);
    }
    else {
        do{
            tmp_i = rand()%n;
            tmp_v = total_bit(buff[tmp_i]);
            if (tmp_v == 8) continue;
            buff[tmp_i] = random_make_byte(tmp_v + 1);
            sum+=1;
        }while(sum < 8*n*remains.local_ratio && sum + 1 < 8*n*remains.local_ratio);
    }
    while(remains.bpixels== 0 && sum == 1 ) {
            tmp_i = (rand()*rand()*rand())%n;
            tmp_v = total_bit(buff[tmp_i]);
            if (tmp_v == 0) continue;
            buff[tmp_i] = random_make_byte(tmp_v - 1);
            sum-=1;
    };
    remains.bpixels -= sum;

}

/** 
  * INPUT : number of bit (i.e '1') we want to the byte have
  * OUTPUT : a char (byte) constituted by nb_bit '1' and n(char)-nb_bit '0'
  *
  *  */

char random_make_byte(int nb_bit){ //TODO : error tst of nb_bit value
    char bit[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
    int i;
    char arr[8]={0}, byte = 0;
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

int sum_first(char arr[], int n){
    int i, n = 0;
    for (i=0;i<n;i++) sum += arr[i];
    return n;
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
    remains.bpixels = dim.width * dim.height * bpixels_ratio;
    remains.remains_bits = dim.width*dim.height;
    remains.local_ratio = bpixels_ratio;
}

/** 
  * INPUT : A character
  * OUTPUT : The sum of bit constituting the char
  *
  *  */

int  total_bit(char x){
    int i,k=0x80, sum = 0;
    for (i=0;i<8;i++){
        sum += (x & k)/k;
        k = k >> 1;
    }
    return sum;
}


