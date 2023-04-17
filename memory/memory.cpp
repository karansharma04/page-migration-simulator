#include "memory.h"
using namespace std;

void LocalMemory::addPage(Page &page) {
  if (L1_cache.size() == MAX_L1_CACHE_SIZE) L1_cache.pop_front();
  if (L2_cache.size() == MAX_L2_CACHE_SIZE) L2_cache.pop_front();
  if (L3_cache.size() == MAX_L3_CACHE_SIZE) L3_cache.pop_front();
  L1_cache.push_back(page);
  L2_cache.push_back(page);
  L3_cache.push_back(page);
  pages.push_back(page);
  pages[pages.size() - 1].isLocal = true;
  pages[pages.size() - 1].isReferenced = false;
}

void LocalMemory::removePage(int id) {
  for (auto it = L1_cache.begin(); it != L1_cache.end(); it++) {
    if (it->id == id) {
      L1_cache.erase(it);
      break;
    }
  }
  for (auto it = L2_cache.begin(); it != L2_cache.end(); it++) {
    if (it->id == id) {
      L2_cache.erase(it);
      break;
    }
  }
  for (auto it = L3_cache.begin(); it != L3_cache.end(); it++) {
    if (it->id == id) {
      L3_cache.erase(it);
      break;
    }
  }
  for (auto it = pages.begin(); it != pages.end(); it++) {
    if (it->id == id) {
      pages.erase(it);
      return;
    }
  }
}

void RemoteMemory::addPage(Page &page) {
  pages.push_back(page);
  pages[pages.size() - 1].isLocal = false;
  pages[pages.size() - 1].isReferenced = false;
  pageTable[page.id] = page;
}

void RemoteMemory::removePage(int id) {
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