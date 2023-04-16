#include <thread>

#include "memory.h"

using namespace std;

#define PAGE_MIGRATION_THRESHOLD 3
#define MAX_PAGES_TO_MIGRATE 10
#define MAX_HOTNESS_TABLE_SIZE 500
#define MAX_L1_CACHE_SIZE 32
#define MAX_L2_CACHE_SIZE 256
#define MAX_L3_CACHE_SIZE 1048

class PageMigrationSimulator {
 public:
  LocalMemory localMemory;
  RemoteMemory remoteMemory;
  int migrationLatency;
  int remoteAccessLatency;
  unsigned long long globalAccessCount = 0;
  PageMigrationSimulator(int _migrationLatency, int _remoteAccessLatency)
      : migrationLatency(_migrationLatency),
        remoteAccessLatency(_remoteAccessLatency) {}

  void accessPage(int id) {
    // Find the page in L1 cache
    for (auto it = localMemory.L1_cache.begin();
         it != localMemory.L1_cache.end(); it++) {
      if (it->id == id) {
        it->isLocal = true;
        it->isReferenced = true;
        return;
      }
    }
    // Find the page in L2 cache
    for (auto it = localMemory.L2_cache.begin();
         it != localMemory.L2_cache.end(); it++) {
      if (it->id == id) {
        it->isLocal = true;
        it->isReferenced = true;
        if (localMemory.L1_cache.size() == MAX_L1_CACHE_SIZE)
          localMemory.L1_cache.pop_front();
        localMemory.L1_cache.push_back(*it);
        return;
      }
    }
    // Find the page in L3 cache
    for (auto it = localMemory.L3_cache.begin();
         it != localMemory.L3_cache.end(); it++) {
      if (it->id == id) {
        it->isLocal = true;
        it->isReferenced = true;
        if (localMemory.L1_cache.size() == MAX_L1_CACHE_SIZE)
          localMemory.L1_cache.pop_front();
        if (localMemory.L2_cache.size() == MAX_L2_CACHE_SIZE)
          localMemory.L2_cache.pop_front();
        localMemory.L1_cache.push_back(*it);
        localMemory.L2_cache.push_back(*it);
        return;
      }
    }
    // Find the page in local memory
    for (auto it = localMemory.pages.begin(); it != localMemory.pages.end();
         it++) {
      if (it->id == id) {
        it->isLocal = true;
        it->isReferenced = true;
        if (localMemory.L1_cache.size() == MAX_L1_CACHE_SIZE)
          localMemory.L1_cache.pop_front();
        if (localMemory.L2_cache.size() == MAX_L2_CACHE_SIZE)
          localMemory.L2_cache.pop_front();
        if (localMemory.L3_cache.size() == MAX_L3_CACHE_SIZE)
          localMemory.L3_cache.pop_front();
        localMemory.L1_cache.push_back(*it);
        localMemory.L2_cache.push_back(*it);
        localMemory.L3_cache.push_back(*it);
        return;
      }
    }
    // Find the page in remote memory
    for (auto it = remoteMemory.pages.begin(); it != remoteMemory.pages.end();
         it++) {
      if (it->id == id) {
        // add network latency
        this_thread::sleep_for(chrono::milliseconds(remoteAccessLatency));

        if (remoteMemory.hotness.find(id) == remoteMemory.hotness.end()) {
          // if hotness table is full, remove the least hot page
          if (remoteMemory.hotness.size() == MAX_HOTNESS_TABLE_SIZE) {
            double minHotness = 1e5;
            for (auto& [_, hotness] : remoteMemory.hotness) {
              minHotness = min(minHotness, hotness);
            }
            for (auto it = remoteMemory.hotness.begin();
                 it != remoteMemory.hotness.end(); it++) {
              if ((*it).second == minHotness) {
                remoteMemory.hotness.erase(it);
                break;
              }
            }
          }
          remoteMemory.hotness[id] = 1;
        } else {
          double d =
              remoteMemory.hotness[id] /
              (remoteMemory.hotness[id] +
               ((double)(globalAccessCount -
                         remoteMemory.globalAccessCountTillLastAccess[id]) /
                (double)remoteMemory.pages.size()));
          remoteMemory.hotness[id] = 1 + d * remoteMemory.hotness[id];
        }
        globalAccessCount++;
        remoteMemory.globalAccessCountTillLastAccess[id] = globalAccessCount;
        return;
      }
    }
  }

  void performMigration(vector<Page> hot_pages, vector<Page> victim_pages) {
    int sz = min(hot_pages.size(), victim_pages.size());
    int cnt = 0;
    for (auto page : hot_pages) {
      cnt++;
      // Remove the page from remote memory
      remoteMemory.removePage(page.id);
      // Add the page to local memory
      Page p = Page(page.id, true);
      localMemory.addPage(p);
      // Add migration latency
      this_thread::sleep_for(chrono::milliseconds(migrationLatency));
      cout << "Hot Page " << page.id
           << " migrated from remote to local memory in " << migrationLatency
           << " ms" << endl;
      if (cnt == sz) break;
    }
    cnt = 0;
    for (auto page : victim_pages) {
      // Remove the page from local memory
      localMemory.removePage(page.id);
      // Add the page to remote memory
      Page p = Page(page.id, false);
      remoteMemory.addPage(p);
      // Add migration latency
      this_thread::sleep_for(chrono::milliseconds(migrationLatency));
      cout << "Victim Page " << page.id
           << " migrated from local to remote memory in " << migrationLatency
           << " ms" << endl;
      if (cnt == sz) break;
    }
  }

  vector<Page> getHotPages() {
    vector<Page> hot_pages;
    for (auto& [id, hotness] : remoteMemory.hotness) {
      if (hotness >= PAGE_MIGRATION_THRESHOLD) {
        hot_pages.push_back(remoteMemory.pageTable[id]);
      }
      if (hot_pages.size() >= MAX_PAGES_TO_MIGRATE) break;
    }
    return hot_pages;
  }

  vector<Page> getVictimPages() {
    vector<Page> victim_pages;
    for (auto page : localMemory.pages) {
      if (!page.isReferenced)
        victim_pages.push_back(page);
      else
        page.isReferenced = false;
      if (victim_pages.size() >= MAX_PAGES_TO_MIGRATE) break;
    }
    return victim_pages;
  }
};