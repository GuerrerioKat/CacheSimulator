Team Members: Katherine Guerrerio (kguerre6) and Angela Guo (aguo14)

Milestone 2--------------------------------------------------------
We used VSCode Live Share to work on almost all functions together.

Katherine:
- handling invalid parameters
- set_append helper function
- evict_block helper function
- load count, load hits, load misses
- store count, store hits, store misses
- total cycles

Angela:
- set_append helper function
- evict_block helper function
- load count, load hits, load misses
- total cycles

Milestone 3--------------------------------------------------------
We ran a variety of experiments to determine which cache configuration had the best overall effectiveness.
There were a few factors that ended up changing the overall performance (looking at the total cycles and misses),
which included cache and block sizes, associativity levels, as well as using either LRU/FIFO and write-through vs.
write-back, write-allocate vs no-write-allocate.

For cache size, the larger the cache, the less total cycles there were, but more misses.

1. Small Cache: ./csim 64 4 16 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 310136
Load misses: 8061
Store hits: 186911
Store misses: 10575
Total cycles: 12749222

2. Medium Cache: ./csim 256 4 16 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314171
Load misses: 4026
Store hits: 188047
Store misses: 9439
Total cycles: 9681657

3. Large Cache: ./csim 1024 4 16 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315334
Load misses: 2863
Store hits: 188499
Store misses: 8987
Total cycles: 7708820

4. Very Large Cache : ./csim 4096 4 16 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315811
Load misses: 2386
Store hits: 188606
Store misses: 8880
Total cycles: 5125697

5. Very Very Very Large Cache: ./csim 1048576 4 16 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315854
Load misses: 2343
Store hits: 188614
Store misses: 8872
Total cycles: 5001340

The larger the cache got (at first), the less the total cycles, although it eventually plateaued off.
Sequential increases to cache size don’t result in much of an impact, so we should choose the smaller
one since smaller caches result in a faster access.

For block size, the higher that went, the higher the total cycles went, but the lower the store misses
went, and by a lot.

1. Small Cache: ./csim 4096 4 4 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 313117
Load misses: 5080
Store hits: 169962
Store misses: 27524
Total cycles: 4297403

2. Medium Cache: ./csim 4096 4 16 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315811
Load misses: 2386
Store hits: 188606
Store misses: 8880
Total cycles: 5125697

3. Large Cache : ./csim 4096 4 64 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317196
Load misses: 1001
Store hits: 195109
Store misses: 2377
Total cycles: 5932282

Obviously, we don't want too many cycles or too many misses, so we can find a good sort of middle ground.
Good Middle Ground:  ./csim 4096 4 32 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 316685
Load misses: 1512
Store hits: 192951
Store misses: 4535
Total cycles: 5364571

For associativity levels, the higher the set associative, the lower the total cycles AND the misses... or so it appears.
It seemed like that for the first two tests of increasing the set associative, but there was a plateau after that,
and the resulting statistics remained largely the same.

1. Direct-Mapped: ./csim 4096 1 32 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315938
Load misses: 2259
Store hits: 192838
Store misses: 4648
Total cycles: 7584624

2. 4-Way Set Associative: ./csim 4096 4 32 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 316685
Load misses: 1512
Store hits: 192951
Store misses: 4535
Total cycles: 5364571

3. 64-Way Set Associative (plateau begins here): ./csim 4096 64 32 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 316694
Load misses: 1503
Store hits: 192955
Store misses: 4531
Total cycles: 5341380

4. 256-Way Set Associative: ./csim 4096 256 32 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 316694
Load misses: 1503
Store hits: 192955
Store misses: 4531
Total cycles: 5341380

5. Fully Associative: ./csim 4096 4096 32 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 316694
Load misses: 1503
Store hits: 192955
Store misses: 4531
Total cycles: 5341380

Trying a same set of inputs, but changing the type of eviction from LRU to FIFO did not make a difference, either.

1. FIFO: ./csim 4096 64 32 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 316694
Load misses: 1503
Store hits: 192955
Store misses: 4531
Total cycles: 5341380
2. LRU : ./csim 4096 64 32 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 316694
Load misses: 1503
Store hits: 192955
Store misses: 4531
Total cycles: 5341380

Write-through produced a LOT of total cycles and a lot of misses compared to write-back, although trying write-allocate
with write-through produced less misses than write-allocate.

1.
Write-Through: ./csim 4096 64 32 write-allocate write-through fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 316694
Load misses: 1503
Store hits: 192955
Store misses: 4531
Total cycles: 24636880

(Write-Through, No-Write-Allocate): ./csim 4096 64 32 no-write-allocate write-through fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314408
Load misses: 3789
Store hits: 169733
Store misses: 27753
Total cycles: 23263941

2. Write-Back: ./csim 4096 64 32 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 316694
Load misses: 1503
Store hits: 192955
Store misses: 4531
Total cycles: 5341380