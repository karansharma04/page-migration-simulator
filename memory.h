#include <unordered_map>
#include <vector>

#include "page.h"

// Define the local memory class
class LocalMemory {
 public:
  vector<Page> pages;
  void addPage(Page page) {
    pages.push_back(page);
    pages[pages.size() - 1].isLocal = true;
    pages[pages.size() - 1].isReferenced = false;
  }
  void removePage(int id) {
    for (auto it = pages.begin(); it != pages.end(); it++) {
      if (it->id == id) {
        pages.erase(it);
        return;
      }
    }
  }
};

// Define the remote memory class
class RemoteMemory {
 public:
  vector<Page> pages;
  unordered_map<int, Page> pageTable;
  unordered_map<int, int> hotness;
  unordered_map<int, unsigned long long> globalAccessCountTillLastAccess;
  void addPage(Page page) {
    pages.push_back(page);
    pages[pages.size() - 1].isLocal = false;
    pages[pages.size() - 1].isReferenced = false;
    pageTable[page.id] = page;
  }
  void removePage(int id) {
    for (auto it = pages.begin(); it != pages.end(); it++) {
      if (it->id == id) {
        pages.erase(it);
        hotness.erase(id);
        pageTable.erase(id);
        globalAccessCountTillLastAccess.erase(id);
        return;
      }
    }
  }
};