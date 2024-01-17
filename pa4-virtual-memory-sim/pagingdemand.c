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
int next_free_frame = 0;

address get_address(unsigned int logical_address)
{
    address address;
    address.page_number = (logical_address >> 8) & 0xFF;
    address.offset = logical_address & 0xFF;
    return address;
}

void handle_page_fault(unsigned int page_number, signed char *physical_memory, unsigned int *page_table)
{
    if (next_free_frame >= NUM_PAGES)
    {
        printf("Error: Out of memory\n");
        exit(EXIT_FAILURE);
    }

    fseek(backing_store, page_number * PAGE_SIZE, SEEK_SET);
    fread(&physical_memory[next_free_frame * PAGE_SIZE], sizeof(char), FRAME_SIZE, backing_store);
    page_table[page_number] = next_free_frame;
    next_free_frame++;
}

void read_backing_store(signed char *physical_memory, unsigned int *page_table)
{
    memset(physical_memory, 0, PHYSICAL_MEM_SIZE);
    for (unsigned int i = 0; i < NUM_PAGES; i++)
    {
        page_table[i] = -1;
    }

    backing_store = fopen("BACKING_STORE.bin", "rb");
    if (backing_store == NULL)
    {
        printf("Error opening backing store\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: ./pagingdemand <input file> <output file>\n");
        return 1;
    }

    FILE *input_file = fopen(argv[1], "r");
    FILE *output_file = fopen(argv[2], "w");

    if (input_file == NULL || output_file == NULL)
    {
        printf("Error opening files\n");
        return 1;
    }

    signed char physical_memory[PHYSICAL_MEM_SIZE];
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

        unsigned int frame_number = page_table[addr.page_number];
        unsigned int physical_address = frame_number * FRAME_SIZE + addr.offset;
        unsigned int value = physical_memory[physical_address];
        fprintf(output_file, "Virtual address: %d Physical address: %d Value: %d\n", logical_address, physical_address, value);
    }

    fclose(input_file);
    fclose(output_file);
    fclose(backing_store);

    return 0;
}