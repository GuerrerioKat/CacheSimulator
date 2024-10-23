#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstdint>
#include <sstream>
#include <cmath>
#include "csim.h"

int main(int argc, char **argv) {
    //SETTING UP USER ARGUMENTS

    if (argc != 7) {
        std::cerr << "Not enough arguements provided!" << std::endl;
        return 1;
    }

    // num_sets
    int32_t num_sets;
    std::stringstream ss1(argv[1]);
    ss1 >> num_sets;
    if ((num_sets & (num_sets - 1)) != 0) {
        std::cerr << "The number of sets provided is not a power of 2!" << std::endl;
        return 2;
    }

    // num_blocks
    int32_t num_blocks;
    std::stringstream ss2(argv[2]);
    ss2 >> num_blocks;
    if ((num_blocks & (num_blocks - 1)) != 0) {
        std::cerr << "The number of blocks provided is not a power of 2!" << std::endl;
        return 2;
    }

    // num_bytes
    int32_t num_bytes;
    std::stringstream ss3(argv[3]);
    ss3 >> num_bytes;
    if ((num_bytes & (num_bytes - 1)) != 0) {
        std::cerr << "The number of bytes per block provided is not a power of 2!" << std::endl;
        return 2;
    }

    if (num_bytes < 4) {
        std::cerr << "The number of bytes per block must be at least 4!" << std::endl;
        return 2;
    }

    std::string store_miss = argv[4];
    std::string store_policy = argv[5];
    std::string eviction_type = argv[6];

    //SET UP COLD CACHE
    Cache cache;
    for (int i = 0; i < num_sets; i++) {
        Set set;
        for (int j = 0; j < num_blocks; j++) {
            Slot slot = {0, false, false, 0, 0};
            set.slots.push_back(slot);
        }
        cache.sets.push_back(set);
    }

    //START LOADING/STORING FROM TRACE FILE

    int load_hits = 0;
    int load_misses = 0;
    int store_hits = 0;
    int store_misses = 0;

    int total_cycles = 0;

    std::string line;
    while (std::getline(std::cin, line)) {
        char status;
        std::string addr_str;

        std::stringstream ss(line);
        ss >> status >> addr_str;
        int32_t addr = std::stoul(addr_str, nullptr, 16);

        //32 bit addresses
        int32_t offset_bits = log2(num_bytes);
        int32_t index_bits = log2(num_sets);
        int32_t tag_bits = 32 - index_bits - offset_bits;

        int32_t offset = addr & ((1 << offset_bits) - 1);
        int32_t idx = (addr >> offset_bits) & ((1 << index_bits) - 1);
        int32_t tag = addr >> (offset_bits + index_bits);
            
        //pulling the slot
        std::vector<Slot>& set = cache.sets[idx].slots;
        
        idx = -1; //repurposing
        int i = 0;
        for (std::vector<Slot>::iterator it = set.begin(); it != set.end(); ++it) {
            if (it->valid && it->tag == tag) {
                idx = i;
                break;
            }
            i++;
        }

        if (status == 'l') { //conditional statments for load
            if (idx < 0) { //miss (add value to set; update counts)
                Slot newVal = {tag, true, false, total_cycles, total_cycles};
                set_append(set, newVal, total_cycles, eviction_type, num_bytes);
                load_misses++;
            } else { //hit (update counts only)
                set[idx].access_ts = total_cycles;
                total_cycles++;
                load_hits++;
            }
        } else if (status == 's') { //conditinal statements for store
            if (idx < 0) { //miss (add value to set; update counts)
                if (store_miss == "write-allocate") {
                    //bring block into cache
                    Slot newVal = {tag, true, false, total_cycles, total_cycles};
                    int32_t index = set_append(set, newVal, total_cycles, eviction_type, num_bytes);
                    
                    //modify value
                    set[index].access_ts = total_cycles;
                    total_cycles++;
                    store_misses++;
                } else if (store_miss == "no-write-allocate") { //update counts and don't change cache
                    store_misses++;
                    total_cycles += 100;
                }
            } else { //hit (write through; update counts)
                //do the write 
                set[idx].access_ts = total_cycles;
                if (store_policy == "write-through") {
                    total_cycles += 100; //also write to main memory
                } else if (store_policy == "write-back") {
                    set[idx].dirty = true; //set dirty block
                }

                total_cycles++;
                store_hits++;
            }
        }
    }

    std::cout << "Total loads: " << load_hits + load_misses 
        << "\nTotal stores: " << store_hits + store_misses
        << "\nLoad hits: " << load_hits
        << "\nLoad misses: " << load_misses
        << "\nStore hits: " << store_hits
        << "\nStore misses: " << store_misses
        << "\nTotal cycles: " << total_cycles
        << "\n";
             
    return 0;
}

uint32_t set_append(std::vector<Slot>& set, Slot newSlot, int& total_cycles, std::string eviction_type, uint32_t block_size) {
    uint32_t i = 0;
    for (std::vector<Slot>::iterator it = set.begin(); it != set.end(); ++it) {
        if (!it->valid) {
            total_cycles += 100 * block_size / 4;
            *it = newSlot;
            return i;
        }
        i++;
    }

    uint32_t index = evict_block(set, eviction_type);
    
    if (set[index].dirty) {
        total_cycles += 100 * block_size / 4; //write back to memory
    }

    //loading from memory to cache
    set[index] = newSlot;
    total_cycles += 100 * block_size / 4;

    return index;
}

uint32_t evict_block(std::vector<Slot>& set, std::string eviction_type) {
    int32_t lru_ts = -1;
    int32_t fifo_ts = -1;
    int32_t index = -1;
    
    if(eviction_type == "lru") { // not too sure how passing in eviction type into console is going to work
        //finding least recently accessed slot
        int i = 0;
        for (std::vector<Slot>::iterator it = set.begin(); it != set.end(); ++it) {
            if (it->valid) {
                if ((lru_ts < 0) || (it->access_ts == lru_ts && it->load_ts < fifo_ts) || (it->access_ts < lru_ts)) {
                    lru_ts = it->access_ts;
                    fifo_ts = it->load_ts;
                    index = i;
                }
                i++;
            }
        }
    } else if (eviction_type == "fifo") {
        //finding earlies loaded block
        int i = 0;
        for (std::vector<Slot>::iterator it = set.begin(); it != set.end(); ++it) {
            if (it->valid) {
                if ((fifo_ts < 0) || (it->load_ts < fifo_ts)) {
                    fifo_ts = it->load_ts;
                    index = i;
                }
                i++;
            }
        }
    }

    return index;
}