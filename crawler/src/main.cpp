// SPDX-License-Identifier: AGPL-3.0-only
#include <iostream>
#include <optional>
#include <thread>

#include "config.hpp"
#include "index_writer.hpp"
#include "link_manager.hpp"
#include "web_crawler.hpp"

int main() {
  crawler::LinkManager link_manager({"https://theakash.dev"});
  crawler::WebCrawler web_crawler;
  crawler::IndexWriter index_writer(std::string(DB_PATH));

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

      if (page_result->content.has_value()) {
        if (page_result->title.has_value()) {
          std::cout << "Page title: " << page_result->title.value()
                    << std::endl;
        } else {
          std::cout << "Page title is missing for: " << link << std::endl;
        }

        if (page_result->content->length() > 100) {
          std::cout << "Page content (first 100 chars): "
                    << page_result->content->substr(0, 100) << "..."
                    << std::endl;
        } else {
          std::cout << "Page content: " << *page_result->content << std::endl;
        }

        std::cout << "Inserting page into index: " << link << std::endl;
        if (index_writer.InsertPage(link, *page_result)) {
          std::cout << "Page inserted successfully." << std::endl;
        } else {
          std::cout << "Failed to insert page into index: " << link
                    << std::endl;
        }
      } else {
        std::cout << "Page content is missing for: " << link << std::endl;
      }
    } else {
      std::cout << "Failed to retrieve page content from: " << link
                << std::endl;
    }
  }

  return 0;
}
