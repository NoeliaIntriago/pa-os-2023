#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

unsigned int PAGE_SIZE = 256;
unsigned int NUM_PAGES = 256;
unsigned int FRAME_SIZE = 256;
unsigned int PHYSICAL_MEM_SIZE = 65536;

typedef struct
{
    unsigned int page_number;
    unsigned int offset;
} address;

FILE *backing_store;

address get_address(unsigned int logical_address)
{
    address address;
    address.page_number = (logical_address >> 8) & 0xFF;
    address.offset = logical_address & 0xFF;
    return address;
}

void handle_page_fault(unsigned int page_number, unsigned char *physical_memory, unsigned int *page_table)
{
    fseek(backing_store, page_number * PAGE_SIZE, SEEK_SET);
    fread(&physical_memory[page_number * PAGE_SIZE], sizeof(char), PAGE_SIZE, backing_store);
    page_table[page_number] = page_number * PAGE_SIZE;
}

void read_backing_store(unsigned char *physical_memory, unsigned int *page_table)
{
    for (unsigned int i = 0; i < PHYSICAL_MEM_SIZE; ++i)
    {
        physical_memory[i] = 0;
    }

    for (unsigned int i = 0; i < NUM_PAGES; ++i)
    {
        page_table[i] = -1;
    }

    backing_store = fopen("data/BACKING_STORE.bin", "rb");
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: ./memory_management <input file> <output file>\n");
        return 1;
    }

    FILE *input_file = fopen(argv[1], "r");
    FILE *output_file = fopen(argv[2], "w");

    if (input_file == NULL || output_file == NULL)
    {
        printf("Error opening files\n");
        return 1;
    }

    unsigned char physical_memory[PHYSICAL_MEM_SIZE];
    unsigned int page_table[NUM_PAGES];

    read_backing_store(physical_memory, page_table);

    unsigned int logical_address;
    while (fscanf(input_file, "%u", &logical_address) != EOF)
    {
        address addr = get_address(logical_address);

        if (page_table[addr.page_number] == -1)
        {
            handle_page_fault(addr.page_number, physical_memory, page_table);
        }

        unsigned int physical_address = page_table[addr.page_number] + addr.offset;
        unsigned int value = physical_memory[physical_address];
        fprintf(output_file, "Virtual address: %d Physical address: %d Value: %d\n", logical_address, physical_address, value);
    }

    fclose(input_file);
    fclose(output_file);
    fclose(backing_store);

    return 0;
}