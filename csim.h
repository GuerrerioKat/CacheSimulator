#ifndef CSIM_H
#define CSIM_H

struct Slot {
    uint32_t tag;
    bool valid, dirty;
    uint32_t load_ts, access_ts;
};

struct Set {
    std::vector<Slot> slots;
};

struct Cache {
    std::vector<Set> sets;
};

// declare functions here
uint32_t set_append(std::vector<Slot>& set, Slot newSlot, int& total_cycles, std::string eviction_type, uint32_t block_size);
uint32_t evict_block(std::vector<Slot>& set, std::string eviction_type);

#endif