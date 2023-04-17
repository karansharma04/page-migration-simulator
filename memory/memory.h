#include <unordered_map>
#include <vector>
#include <list>
#include "./../page.h"

using namespace std;

#define MAX_L1_CACHE_SIZE 32
#define MAX_L2_CACHE_SIZE 256
#define MAX_L3_CACHE_SIZE 1048

// Define the local memory class
class LocalMemory {
 public:
  list<Page> L1_cache;  // max 32 pages
  list<Page> L2_cache;  // max 128 pages
  list<Page> L3_cache;  // max 2048 pages
  vector<Page> pages;   // local memory
  void addPage(Page &);
  void removePage(int);
};

// Define the remote memory class
class RemoteMemory {
 public:
  vector<Page> pages;
  unordered_map<int, Page> pageTable;
  unordered_map<int, double> hotness;
  unordered_map<int, unsigned long long> globalAccessCountTillLastAccess;
  void addPage(Page &);
  void removePage(int);
};