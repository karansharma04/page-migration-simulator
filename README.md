# page-migration-simulator
Simulator for page migration between local and remote memories, with page migration based on novel method for finding hotness of pages in remote memory, and LRU for victim page selection in local memory.
Hotness of  a page based on both the recency and frequency of accesses.
Size of the hotness-table is fixed, so the hotness detection algorithm can be implemented in the remote memory controller without any significant overhead.
Page migration done on a fixed page migration epoch, where the central memory manager will request hot pages from the remote memory controller, and victim pages from local memory, and perform page migration between the memories.
