#include<stdio.h>
#include<stdlib.h>

#define file_size 2000000



void main(void) {
    unsigned char *my_array = malloc(file_size * sizeof(unsigned char));

    FILE *file_pointer = fopen("my_filter.bin", "rb");

    if (file_pointer == NULL) {
        printf("Error: Could not open my_filter.bin!\n");
        printf("Did you run the program that creates the file first?\n");
        free(my_array); // Clean up memory before exiting
        return; // Exit the program safely
    }
    fread(my_array, sizeof(unsigned char), file_size, file_pointer);
    fclose(file_pointer);
    for (int byte_index = 0; byte_index < file_size; byte_index++) {
        
        // We only need to check inside this byte if it's not zero
        if (my_array[byte_index] != 0) {
            
            // 2. Loop through each of the 8 BITS (light bulbs) inside this byte
            for (int bit_position = 0; bit_position < 8; bit_position++) {
                
                // 3. Use a bitwise AND to check if the specific bit is ON
                if (my_array[byte_index] & (1 << bit_position)) {
                    
                    // Calculate the overall bit number for our report
                    unsigned long overall_bit_index = (unsigned long)byte_index * 8 + bit_position;
                    printf("Found a '1' at bit index: %lu\n", overall_bit_index);
                }
            }
        }
    }
    free(my_array); 
}