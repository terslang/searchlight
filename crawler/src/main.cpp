// SPDX-License-Identifier: AGPL-3.0-only
#include <iostream>
#include <optional>
#include <thread>

#include "config.hpp"
#include "index_writer.hpp"
#include "link_manager.hpp"
#include "options.hpp"
#include "web_crawler.hpp"
#include <yaml-cpp/yaml.h>

int main() {
  YAML::Node options_node = YAML::LoadFile(OPTIONS_FILE_PATH);
  crawler::Options options(options_node);
  crawler::LinkManager link_manager(options.seed_links,
                                    options.crawl_options->default_delay);
  crawler::WebCrawler web_crawler;
  crawler::IndexWriter index_writer(std::move(options.database_options));

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
