#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h> // For getcwd() on Unix-like systems
#include <sys/stat.h>
#include <sys/types.h>

#define file_size 2000000

unsigned char *my_array;

unsigned long hash_one(const char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
    {
        // hash = hash * 33 + c
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

unsigned long hash_two(const char *str)
{
    unsigned long hash = 0;
    int c;
    while ((c = *str++))
    {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

void writeFile(void)
{
    FILE *file_out = fopen("my_filter.bin", "wb");
    if (file_out == NULL)
    {
        printf("Error: Could not save the filter file!\n");
    }
    else
    {
        fwrite(my_array, sizeof(unsigned char), file_size, file_out);
        fclose(file_out);
        // printf("Save complete.\n");
    }
}
void readfiles(unsigned char *array, char *basePath)
{
    char path[1000];
    unsigned long array_size_in_bits = 16000000;
    struct dirent *de;
    /* char basePath[1024]; */
    // printf("basepath at readfiles%s",basePath);
    // exit(0);
    DIR *dr = opendir(basePath);
    if (dr == NULL)
    {
        printf("Error reading directory\n");
        return;
    }
    while ((de = readdir(dr)) != NULL)
    {
        /* if ((strcmp(de->d_name, ".") == 0) || (strcmp(de->d_name, "..") == 0))
        {
            continue;
        } */
        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0 && strcmp(de->d_name, ".git") != 0)
        {
            snprintf(path, sizeof(path), "%s/%s", basePath, de->d_name);
            struct stat statbuf;

            if (stat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode))
            {
                readfiles(array, path);
            }
            else
            {
                //printf("%s\n", de->d_name);
                printf("Indexing: %s\n", path);
                unsigned long h1 = hash_one(de->d_name);
                unsigned long h2 = hash_two(de->d_name);

                for (int i = 0; i < 6; i++)
                {
                    unsigned long combined_hash = h1 + (i * h2);
                    unsigned long bit_index = combined_hash % array_size_in_bits;
                    unsigned long byte_index = bit_index / 8;
                    int bit_position = bit_index % 8;

                    array[byte_index] |= (1 << bit_position);
                }
            }
        }
    }
    closedir(dr);
}

void loadbin(char *basePath)
{
    writeFile();
    my_array = malloc(file_size * sizeof(unsigned char));
    FILE *file_pointer = fopen("my_filter.bin", "rb");
    fread(my_array, sizeof(unsigned char), file_size, file_pointer);
    fclose(file_pointer);
    readfiles(my_array, basePath);
    writeFile();
    free(my_array);
}

void search(char *file, unsigned char *array)
{
    my_array = malloc(file_size * sizeof(unsigned char));
    unsigned long array_size_in_bits = 16000000;
    FILE *file_pointer = fopen("my_filter.bin", "rb");

    if (file_pointer == NULL)
    {
        printf("Error: Could not open my_filter.bin!\n");
        printf("Did you run the program that creates the file first?\n");
        free(my_array);
        return;
    }
    fread(my_array, sizeof(unsigned char), file_size, file_pointer);
    fclose(file_pointer);

    unsigned long h1 = hash_one(file);
    unsigned long h2 = hash_two(file);

    for (int i = 0; i < 6; i++)
    {
        unsigned long combined_hash = h1 + (i * h2);
        unsigned long bit_index = combined_hash % array_size_in_bits;
        unsigned long byte_index = bit_index / 8;

        int bit_position = bit_index % 8;
        if (!(my_array[byte_index] & (1 << bit_position)))
        {
            printf("File not found!");
            free(my_array);
            return;
        }
    }
    printf("File is probably in the storage");
    free(my_array);
}

void main(int argc, char **argv)
{
    char basePath[1024];
    getcwd(basePath, sizeof(basePath)) != NULL;
    if (argc == 1)
    {
        printf("Use --index-files to index the files\n Use --search [filename] to search for files");
        return;
    }
    if (strcmp("--index-files", argv[1]) == 0)
    {
        // printf("Files are being indexed. please wait...\n");
        loadbin(basePath);
    }
    else if (strcmp("--search", argv[1]) == 0)
        search(argv[2], my_array);
    else
    {
        printf("Use --index-files to index the files\n Use --search [filename] to search for files");
        return;
    }
}