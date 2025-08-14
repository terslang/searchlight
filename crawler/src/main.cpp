// SPDX-License-Identifier: AGPL-3.0-only
#include <iostream>
#include <optional>
#include <thread>

#include "link_manager.hpp"
#include "web_crawler.hpp"

int main() {
  crawler::LinkManager link_manager(
      {"https://theakash.dev", "https://blog.s20n.dev"});
  crawler::WebCrawler web_crawler;

  while (link_manager.HasLinksToVisit()) {
    std::string link = link_manager.GetNextLinkToVisit();
    if (!link_manager.IsCrawlAllowed(link)) {
      std::cout << "Skipping disallowed link: " << link << std::endl;
      // Mark the link as visited and continue to the next one
      link_manager.MarkLinkAsVisited(link);
      continue;
    }

    if (!link_manager.HasEnoughDelay(link)) {
      std::cout << "Waiting for delay, link requeued: " << link << std::endl;
      // Requeue the link to visit again later
      link_manager.RequeLink(link);
      // wait for a while before checking the next link
      std::this_thread::sleep_for(std::chrono::seconds(1));
      continue;
    }

    std::optional<crawler::PageResult> page_result = web_crawler.GetPage(link);
    link_manager.MarkLinkAsVisited(link);
    std::cout << "Visited: " << link << std::endl;

    if (page_result.has_value()) {
      link_manager.AddDiscoveredLinks(page_result->links, link);
    } else {
      std::cout << "Failed to retrieve page content from: " << link
                << std::endl;
    }
  }

  return 0;
}
