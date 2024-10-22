#ifndef CSIM_H
#define CSIM_H

// declare functions here
// int write_back(uint32_t address, int* store_hits, int* store_misses);
// int write_through(uint32_t address, int* store_hits, int* store_misses);
// int write_alloc(uint32_t address, int* load_hits, int* load_misses);
// int no_write_allocate(uint32_t address, int* load_hits, int* load_misses);
bool set_append(std::vector<Slot> set, Slot newSlot, uint32_t num_blocks, std::string eviction_type);
void evict_block(std::vector<Slot> set, std::string eviction_type);

#endif