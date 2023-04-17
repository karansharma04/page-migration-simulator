#include <thread>

#include "./../memory/memory.h"

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

  void accessPage(int);

  void performMigration(vector<Page>, vector<Page>);

  vector<Page> getHotPages();

  vector<Page> getVictimPages();
};