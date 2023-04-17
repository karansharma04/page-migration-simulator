#include <bits/stdc++.h>

#include <chrono>
#include <ctime>

#include "./page-migration/page-migration.h"

using namespace std;

int main() {
  // Create the page migration simulator with migration latency as 1 ms
  PageMigrationSimulator simulator(1, 1);
  // Add some pages to local memory
  for (int i = 0; i <= 100; i++) {
    Page p = Page(i, true);
    simulator.localMemory.addPage(p);
  }
  // Add some pages to remote memory
  for (int i = 101; i < 1000; i++) {
    Page p = Page(i, false);
    simulator.remoteMemory.addPage(p);
  }
  // Access 1000 pages at random
  for (int i = 0; i < 10000; i++) {
    auto start = std::chrono::high_resolution_clock::now();
    int page_id = rand() % 1000;
    simulator.accessPage(page_id);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::ratio<1L, 1000L>> duration =
        (end - start);
    cout << "Accessed page " << page_id << " in " << duration.count()
         << " milliseconds." << std::endl;
  }

  // Set the epoch duration to 1 second
  const std::chrono::seconds epoch_duration{1};

  // Get the current time
  auto start_time = std::chrono::steady_clock::now();

  // Perform an action every epoch
  int epoch_counter = 0;
  while (true) {
    // Get the current time
    auto current_time = std::chrono::steady_clock::now();

    // Check if the epoch duration has elapsed
    if (current_time - start_time >= epoch_duration) {
      // Increment the epoch counter
      epoch_counter++;

      // Perform the page migration
      auto hot_pages = simulator.getHotPages();
      auto cold_pages = simulator.getVictimPages();

      simulator.performMigration(hot_pages, cold_pages);

      std::cout << "Epoch " << epoch_counter << " action performed."
                << std::endl;

      // Reset the start time for the next epoch
      start_time = current_time;
    }

    // Sleep for a short time to avoid using 100% CPU
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  return 0;
}
