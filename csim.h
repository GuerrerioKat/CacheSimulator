#ifndef CSIM_H
#define CSIM_H

// declare functions here
int write_back(uint32_t address, int* store_hits, int* store_misses);
int write_through(uint32_t address, int* store_hits, int* store_misses);
int write_alloc(uint32_t address, int* load_hits, int* load_misses);
int no_write_allocate(uint32_t address, int* load_hits, int* load_misses);

#endif