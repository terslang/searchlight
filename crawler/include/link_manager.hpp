// SPDX-License-Identifier: AGPL-3.0-only
#pragma once

#include <chrono>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "config.hpp"
#include "robots_parser.hpp"

namespace crawler {

class LinkManager {
public:
  LinkManager(const std::vector<std::string> &seed_links);

  void AddDiscoveredLinks(const std::vector<std::string> &links,
                          const std::string &source_link);

  void MarkLinkAsVisited(const std::string &link);

  bool HasLinksToVisit() const;

  std::string GetNextLinkToVisit();

  void RequeLink(const std::string &link);

  bool IsCrawlAllowed(const std::string &link) const;

  bool HasEnoughDelay(const std::string &link) const;

private:
  std::unordered_set<std::string> seed_links;
  std::unordered_set<std::string> all_known_links;
  std::queue<std::string> links_to_visit;
  std::unordered_map<std::string, std::unique_ptr<RobotsParser>>
      robots_txt_parsers;
  std::unordered_map<std::string, std::chrono::steady_clock::time_point>
      visited_hosts;

  bool isRelativeLink(const std::string &link) const;

  bool isBasedOnSeedLink(const std::string &link) const;
};

} // namespace crawler
