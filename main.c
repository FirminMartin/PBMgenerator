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
    long long remains_bpixels;
    long long remains_bits;
    double locale_bpixels_ratio;
}remains_t;

remains_t remains;

void write_pbm(const char* name, dim_t* dim, double bpixels_ratio);
void init_remains(dim_t dim, double bpixels_ratio);
char random_make_byte(int nb_bit);
int sum_nb_bit(char arr[], int size);
void print_byte(char x);
//void make_rand_buffer(char buff[BUFF_LEN], int size);
void random_make_sum(char buff[BUFF_LEN], int size);


int  count_bit(char x){
    int i,k=0x80, sum = 0;
    for (i=0;i<8;i++){
        sum += (x & k)/k;
        k = k >> 1;
    }
    return sum;
}

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

void write_pbm(const char* name, dim_t* dim, double bpixels_ratio){
    static int file_nb = 0;
    int write_bits = 0;
    long long remains_bytes=0;
    char buff[BUFF_LEN]={0};
    char new_name[255]={0};
    double tmp_locale_bpixels_ratio = 0;
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
    fwrite(buff, sizeof(char),strlen(buff), fp);
    do{
        remains_bytes = remains.remains_bits/8;
        write_bits = (remains_bytes > BUFF_LEN) ? BUFF_LEN : (!remains_bytes ? 1 : remains_bytes);



        random_make_sum(buff, write_bits);

        remains.remains_bits -= 8*fwrite(buff, sizeof(char), write_bits, fp);

        tmp_locale_bpixels_ratio = remains.locale_bpixels_ratio;
        if (remains.remains_bpixels == 0) remains.locale_bpixels_ratio = 0;
        remains.locale_bpixels_ratio = remains.remains_bpixels / (remains.remains_bits + 0.0000000000000000001);
        if (remains.locale_bpixels_ratio < tmp_locale_bpixels_ratio) remains.locale_bpixels_ratio *= (0.01*(rand()%5)); //TODO

//system("pause");
    }while(remains_bytes  > 0);
    fclose(fp);
    file_nb++;
}

void random_make_sum(char buff[BUFF_LEN], int size){
    long long i, tmp_v, tmp_i;
    long long sum = 0;
    for(i = 0; i < size; i++){
        buff[i] = rand()%256;
        sum += count_bit(buff[i]);
    }
    if (sum > 8*size*remains.locale_bpixels_ratio ){
        do{
            tmp_i = (rand()*rand()*rand())%size;
            tmp_v = count_bit(buff[tmp_i]);
            if (tmp_v == 0) continue;
            buff[tmp_i] = random_make_byte(tmp_v - 1);
            sum-=1;
        }while(sum > 8*size*remains.locale_bpixels_ratio && sum - 1 > 8*size*remains.locale_bpixels_ratio);
    }
    else {
        do{
            tmp_i = rand()%size;
            tmp_v = count_bit(buff[tmp_i]);
            if (tmp_v == 8) continue;
            buff[tmp_i] = random_make_byte(tmp_v + 1);
            sum+=1;
        }while(sum < 8*size*remains.locale_bpixels_ratio && sum + 1 < 8*size*remains.locale_bpixels_ratio);
    }
    while(remains.remains_bpixels== 0 && sum == 1 ) {
            tmp_i = (rand()*rand()*rand())%size;
            tmp_v = count_bit(buff[tmp_i]);
            if (tmp_v == 0) continue;
            buff[tmp_i] = random_make_byte(tmp_v - 1);
            sum-=1;
    };
    remains.remains_bpixels -= sum;

}

char random_make_byte(int nb_bit){
    char bit[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
    int i;
    char arr[8]={0}, byte = 0;
    while(sum_nb_bit(arr, 8)!=nb_bit){
        for (i=0;i<8;i++) arr[i] = rand()%2;
    }
    for(i=0;i<8;i++) byte += arr[i]*bit[i];
    return byte;
}

int sum_nb_bit(char arr[], int size){
    int i, sum = 0;
    for (i=0;i<size;i++) sum += arr[i];
    return sum;
}

void print_byte(char x){
    int i,k=0x80;
    for (i=0;i<8;i++){
        printf("%d",(x & k)/k);
        k = k >> 1;
    }
    printf(" ");
}

void init_remains(dim_t dim, double bpixels_ratio){
    remains.remains_bpixels = dim.width * dim.height * bpixels_ratio;
    remains.remains_bits = dim.width*dim.height;
    remains.locale_bpixels_ratio = bpixels_ratio;
}
