#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>

#define file_size 2000000

unsigned char *my_array;

unsigned long hash_one(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        // hash = hash * 33 + c
        hash = ((hash << 5) + hash) + c; 
    }
    return hash;
}

unsigned long hash_two(const char *str) {
    unsigned long hash = 0;
    int c;
    while ((c = *str++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

void writeFile(void){
    FILE *file_out = fopen("my_filter.bin", "wb");
    if (file_out == NULL) {
        printf("Error: Could not save the filter file!\n");
    } else {
        fwrite(my_array, sizeof(unsigned char), file_size, file_out);
        fclose(file_out);
        printf("Save complete.\n");
    }
}
void readfiles(unsigned char *array){
    printf("Val at Array index 0 :%d\n", array[0]);
    unsigned long array_size_in_bits = 16000000;
    struct dirent *de;

    DIR *dr = opendir(".");
    if(dr == NULL){
        printf("Error reading directory\n");
        return;
}
    while((de = readdir(dr)) != NULL){
        /* printf("%s\n", de->d_name); */
        
         if((strcmp(de->d_name, ".") == 0) || (strcmp(de->d_name, "..") == 0)){
        printf(". or .. detected\n");
        continue;
        } 
        unsigned long h1 = hash_one(de->d_name);
        unsigned long h2 = hash_two(de->d_name);
        /* printf("%u\n", h1);
        printf("%u\n", h2); */

        for(int i = 0; i < 6; i++){
            unsigned long combined_hash = h1 + (i * h2);
            unsigned long bit_index = combined_hash % array_size_in_bits;
            unsigned long byte_index = bit_index / 8;

            // 2. Find the correct light switch (bit)
            int bit_position = bit_index % 8;

            array[byte_index] |= (1 << bit_position);
           /*  printf("Array index:%u file name : %s\n", byte_index, de->d_name);
            printf("File: %s -> Bit Index: %lu -> Byte Index: %lu -> Bit Position: %d\n", de->d_name, bit_index, byte_index, bit_position); */
            printf("%d\n", bit_index);
            //printf("%d\n", array[array_index]);
        }

    }
    writeFile();
}



void loadbin(void){
    my_array = malloc(file_size * sizeof(unsigned char));
    FILE *file_pointer = fopen("my_filter.bin", "rb");

    fread(my_array, sizeof(unsigned char), file_size, file_pointer);
    fclose(file_pointer);
    readfiles(my_array);
    free(my_array); 
}

void main(void) {
    loadbin();
}