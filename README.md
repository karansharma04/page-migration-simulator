# Simulator for page migration between local and remote memories

## Steps to run:

1. Clone the repo with the command: 
		```git clone https://github.com/karansharma04/page-migration-simulator.git```
2. Open the folder in terminal and run the following command to compile the project: 
		```make build```
3. Run the following command to run the executable file:
		```./simulator```

## Background

Simulator for page migration between local and remote memories, with page migration based on novel method for finding hotness of pages in remote memory, and LRU for victim page selection in local memory. 


Hotness of  a page based on both the recency and frequency of accesses.


```hot_new = 1 + d * hot_old```,


where *hot_new* is the new hotness of the page, *hot_old* is the old hotness of the page, and *d* is the decay coefficient which will decrease as the page accesses become older. This would decrease the importance of old accesses in the current hotness. *d* for a page will be calculated as:


```d = hot_old / ( hot_old +( (global_access_count - global_access_count_till_last_access)/total_pages_in_memory) )```. 


*hot_old* is the old hotness of the page, *global_access_count* is the total number of accesses in the memory till now, and *global_access_count_till_last_access* is the total number of accesses till the last access to the page.


Thus,  ```(global_access_count - global_access_count_till_last_access)/total_pages_in_memory``` roughly gives us the number of cycles of accesses to the memory since the page under consideration was last accessed.
For victim page selection, traditional LRU based approach is used.

Size of the hotness-table is fixed, so the hotness detection algorithm can be implemented in the remote memory controller without any significant overhead.

Page migration done on a fixed page migration epoch, where the central memory manager will request hot pages from the remote memory controller, and victim pages from local memory, and perform page migration between the memories.
