#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>

//Hüseyin Mert Yenilmez
//14216255208
//CMPE 382 Project 2

struct  header {
    size_t size;
    unsigned is_free;
    struct  header *next;
};

struct  header *head = NULL, *tail = NULL;

struct  header *get_free_block(size_t size)
{
    struct  header *curr = head;
    while(curr) {
        if (curr->is_free && curr->size >= size)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

void TEDU_free(void *block)
{
    struct  header *header, *tmp;
    void *programbreak;
    
    if (!block)
        return;
    header = (struct  header*)block - 1;
    programbreak = mmap (0,
                         0,
                         PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS,
                         -1,
                         0);
    

    if ((char*)block + header->size == programbreak) {
        if (head == tail) {
            head = tail = NULL;
        } else {
            tmp = head;
            while (tmp) {
                if(tmp->next == tail) {
                    tmp->next = NULL;
                    tail = tmp;
                }
                tmp = tmp->next;
            }
        }
      
        mmap (0,
              0 - header->size - sizeof(struct  header),
              PROT_READ | PROT_WRITE,
              MAP_PRIVATE | MAP_ANONYMOUS,
              -1,
              0);
        
        return;
    }
    header->is_free = 1;
    
}

void *TEDU_alloc(size_t size)
{
    size_t total_size;
    void *block;
    struct  header *header;
    if (!size)
        return NULL;
    header = get_free_block(size);
    if (header) {
        header->is_free = 0;
        
        return (void*)(header + 1);
    }
    total_size = sizeof(struct  header) + size;
    block = mmap (0,
                  total_size,
                  PROT_READ | PROT_WRITE,
                  MAP_PRIVATE | MAP_ANONYMOUS,
                  -1,
                  0);
    if (block == (void*) -1) {
        
        return NULL;
    }
    header = block;
    header->size = size;
    header->is_free = 0;
    header->next = NULL;
    if (!head)
        head = header;
    if (tail)
        tail->next = header;
    tail = header;
    
    return (void*)(header + 1);
}


void TEDU_GetStats()
{
    struct  header *curr = head;
    printf("beginning, head = %p, next = %p \n", (void*)head, (void*)tail);
    while(curr) {
        printf("current block = %p, size = %zu, is_free=%u, next block =%p\n",
               (void*)curr, curr->size, curr->is_free, (void*)curr->next);
        curr = curr->next;
    }
}

int main(int argc, char* argv[])
{
    int *a;
    a = TEDU_alloc(10);
    TEDU_free(TEDU_alloc(5));
    TEDU_GetStats();
}
