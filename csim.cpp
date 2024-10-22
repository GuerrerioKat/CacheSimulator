#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstdint>
#include <sstream>
#include "csim.h"

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

bool set_append(std::vector<Slot> set, Slot newSlot, uint32_t num_blocks, std::string eviction_type) {
    if(set.size() < num_blocks && newSlot.valid) {
        set.insert(set.begin(), newSlot);
    } else {
        evict_block(set, eviction_type);
    }
    return true;
}

void evict_block(std::vector<Slot> set, std::string eviction_type) {
    if(eviction_type.compare("lru") == 0) { // not too sure how passing in eviction type into console is going to work
        set.pop_back(); // remove last element aka least used since we're adding to beginning
    } else {
        // fifo
    }
}

int main(int argc, char **argv) {
    //SETTING UP USER ARGUMENTS

    std::cout << argc << std::endl;
    if (argc != 7) {
        std::cerr << "Not enough arguements provided!" << std::endl;
        return 1;
    }
    
    std::stringstream ss;

    uint32_t num_sets;
    ss << argv[1];
    ss >> num_sets;
    if ((num_sets & (num_sets - 1)) != 0) {
        std::cerr << "The number of sets provided is not a power of 2!" << std::endl;
        return 2;
    }

    uint32_t num_blocks;
    ss << argv[2];
    ss >> num_blocks;
    if ((num_blocks & (num_blocks - 1)) != 0) {
        std::cerr << "The number of blocks provided is not a power of 2!" << std::endl;
        return 2;
    }

    uint32_t num_bytes;
    ss << argv[3];
    ss >> num_bytes;
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
            Slot slot;
            set.slots.push_back(slot);
        }
        cache.sets.push_back(set)
    }

    //START LOADING/STORING FROM TRACE FILE

    int load_hits = 0;
    int load_misses = 0;
    int store_hits = 0;
    int store_misses = 0;

    std::string line;
    while (std::getline(std::cin, line)) {
        std::string stat = line.substr(0, 1);
        char status = (char) stat;
        std::string tag = line.substr(4, 8); //TODO: fix these
        std::string idx = line.substr(4, 8);
        uint32_t index = (uint32_t) idx;
        
        //pulling the slot
        Set set = cache.sets[index];
        Slot found = null;
        for (std::vector<Slot>::iterator it = set.begin(); it != set.end(); ++it) {
            if (it->tag.equals(tag)) {
                found = (*it);
            }
        }
        if (status == 'l') { //conditional statments for load
            if (found == null) { //miss (add value to set; update counts)
                Slot newVal = {(uint32_t) tag, true, false, index, index}; // fix this later bc idk what is going on here
                bool add = set_append(set, newVal, num_blocks, eviction_type);
                //eviction policies (don't forget to check dirty blocks) --> eviction checked in append, but not dirty
                load_misses++;
            } else { //hit (update counts only)
                load_hits++;
            }
        } else if (status == 's') { //conditinal statements for store
            if (found == null) { //miss (add value to set; update counts)
                if (store_miss == "write-allocate") {
                    //bring block into cache
                        //eviction policies (don't forget to check dirty blocks)
                    //modify value
                    store_misses++;
                } else if (store_miss == "no-write-allocate") { //update counts and don't change cache
                    store_misses++;
                }
            } else { //hit (write through vs write back; update counts)
                //do the write        
                if (store_policy == "write-back") { //update counts and don't change cache
                    //set dirty block (if necessary)
                }
                store_hits++;
            }
        }
    }

    return 0;
}