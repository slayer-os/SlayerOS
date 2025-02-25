#include "memory.h"
#include "serial.h"

// Define the size of your heap (adjust as needed)
#define HEAP_SIZE 1024 * 1024 // 1 MB heap

// The start of the heap
static uint8_t heap[HEAP_SIZE];

// Block header structure for tracking allocated blocks
typedef struct BlockHeader {
    size_t size;
    struct BlockHeader* next;
    int is_free;
} BlockHeader;

// The first block header in the free list
static BlockHeader* free_list = NULL;

void memory_init(void) {
    serial_puts("Initializing memory management...\n");
    free_list = (BlockHeader*)heap;
    free_list->size = HEAP_SIZE - sizeof(BlockHeader);
    free_list->next = NULL;
    free_list->is_free = 1;
    serial_puts("Memory initialized. Heap size: 1048576 bytes\n");
}

void* malloc(size_t size) {
    BlockHeader* current = free_list;
    BlockHeader* prev = NULL;

    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            if (current->size - size >= sizeof(BlockHeader)) {
                // Split the block
                BlockHeader* new_block = (BlockHeader*)((uint8_t*)current + size + sizeof(BlockHeader));
                new_block->size = current->size - size - sizeof(BlockHeader);
                new_block->next = current->next;
                new_block->is_free = 1;

                current->size = size;
                current->next = new_block;
            }

            current->is_free = 0;
            return (uint8_t*)current + sizeof(BlockHeader);
        }

        prev = current;
        current = current->next;
    }

    serial_puts("Error: Out of memory\n");
    return NULL;
}

void free(void* ptr) {
    if (ptr == NULL) {
        return;
    }

    BlockHeader* block_to_free = (BlockHeader*)((uint8_t*)ptr - sizeof(BlockHeader));
    block_to_free->is_free = 1;

    // Merge with adjacent free blocks
    BlockHeader* current = free_list;
    while (current != NULL) {
        if (current->is_free && (uint8_t*)block_to_free + block_to_free->size + sizeof(BlockHeader) == (uint8_t*)current) {
            block_to_free->size += current->size + sizeof(BlockHeader);
            block_to_free->next = current->next;
            break;
        } else if (current->next != NULL && current->next == block_to_free && current->is_free) {
            current->size += block_to_free->size + sizeof(BlockHeader);
            current->next = block_to_free->next;
            break;
        }
        current = current->next;
    }

    // Insert the free block into the free list in the correct position
    if (free_list == NULL || block_to_free < free_list) {
        block_to_free->next = free_list;
        free_list = block_to_free;
    } else {
        current = free_list;
        while (current->next != NULL && current->next < block_to_free) {
            current = current->next;
        }
        block_to_free->next = current->next;
        current->next = block_to_free;
    }
}
