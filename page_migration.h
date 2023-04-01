#include <thread>

#include "memory.h"

using namespace std;

#define PAGE_MIGRATION_THRESHOLD 5
#define MAX_PAGES_TO_MIGRATE 10
#define MAX_HOTNESS_TABLE_SIZE 500

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
    // Find the page in local memory
    for (auto it = localMemory.pages.begin(); it != localMemory.pages.end();
         it++) {
      if (it->id == id) {
        // Increment the page's access count in remote memory
        it->isLocal = true;
        it->isReferenced = true;
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
          // if hotness table is full, remove the least hote page
          if (remoteMemory.hotness.size() == MAX_HOTNESS_TABLE_SIZE) {
            int minHotness = INT_MAX;
            for(auto& [_, hotness]: remoteMemory.hotness) {
              minHotness = min(minHotness, hotness);
            }
            for(auto it = remoteMemory.hotness.begin(); it != remoteMemory.hotness.end(); it++) {
              if((*it).second==minHotness) {
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
    for (auto page : hot_pages) {
      // Remove the page from remote memory
      remoteMemory.removePage(page.id);
      // Add the page to local memory
      localMemory.addPage(Page(page.id, true));
      // Add migration latency
      this_thread::sleep_for(chrono::milliseconds(migrationLatency));
      cout << "Hot Page " << page.id
           << " migrated from remote to local memory in " << migrationLatency
           << " ms" << endl;
    }
    for (auto page : victim_pages) {
      // Remove the page from local memory
      localMemory.removePage(page.id);
      // Add the page to remote memory
      remoteMemory.addPage(Page(page.id, true));
      // Add migration latency
      this_thread::sleep_for(chrono::milliseconds(migrationLatency));
      cout << "Victim Page " << page.id
           << " migrated from local to remote memory in " << migrationLatency
           << " ms" << endl;
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