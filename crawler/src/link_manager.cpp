// SPDX-License-Identifier: AGPL-3.0-only
#include "link_manager.hpp"

#include <ada.h>
#include <iostream>

#include "config.hpp"
#include "options.hpp"
#include "utils.hpp"
#include "web_crawler.hpp"

namespace crawler {

LinkManager::LinkManager(const std::vector<std::string> &seed_links,
                         const int default_delay) {
  this->default_delay = default_delay;
  for (const auto &link : seed_links) {
    this->seed_links.insert(link);
    links_to_visit.push(link);
    all_known_links.insert(link);

    std::string host = utils::GetHostFromUrl(link);
    if (!robots_txt_parsers.contains(host)) {
      std::cout << "Fetching robots.txt for host: " << host << std::endl;

      WebCrawler web_crawler;
      std::optional<PageResult> robots_txt_result =
          web_crawler.GetPage(utils::GetBaseUrl(link) + "/robots.txt");
      if (robots_txt_result.has_value() &&
          robots_txt_result->content.has_value()) {
        robots_txt_parsers[host] =
            std::make_unique<RobotsParser>(robots_txt_result->content.value());
      } else {
        // If robots.txt is not found, we assume an "allow all" policy
        robots_txt_parsers[utils::GetHostFromUrl(link)] =
            std::make_unique<RobotsParser>();
      }
    }
  }
}

void LinkManager::AddDiscoveredLinks(const std::vector<std::string> &links,
                                     const std::string &source_link) {
  // Links discovered means that the source link has been visited
  MarkLinkAsVisited(source_link);

  for (const auto &link : links) {
    if (isRelativeLink(link)) {
      // Add the domain from the source link to the relative link and add it to
      // the links to visit
      std::string full_link;

      if (link[0] == '/') {
        // If the link is a relative path, we need to prepend the source domain
        full_link = utils::GetBaseUrl(source_link) + link;
      } else if (link[0] == '#') {
        // If the link is a fragment (starts with '#'), we can skip it
        continue;
      } else {
        // If the link is a relative URL (not starting with '/'), we need to
        // prepend the source url (remove any trailing slashes)
        std::string source_link_string = source_link;
        if (source_link_string.back() == '/') {
          source_link_string.pop_back(); // Remove trailing slash if present
        }
        full_link = source_link_string + '/' + link;
      }

      if (isBasedOnSeedLink(full_link) &&
          all_known_links.find(full_link) == all_known_links.end()) {
        links_to_visit.push(full_link);
        all_known_links.insert(full_link);
      }
    } else if (isBasedOnSeedLink(link)) {
      // check if the link is based on a seed link
      if (all_known_links.find(link) == all_known_links.end()) {
        links_to_visit.push(link);
        all_known_links.insert(link);
      }
    } else {
      // If the link is not relative and not based on a seed link, we can skip
      continue;
    }
  }
}

void LinkManager::MarkLinkAsVisited(const std::string &link) {
  visited_hosts[utils::GetHostFromUrl(link)] = std::chrono::steady_clock::now();
}

bool LinkManager::HasLinksToVisit() const { return !links_to_visit.empty(); }

std::string LinkManager::GetNextLinkToVisit() {
  if (links_to_visit.empty()) {
    return "";
  }
  // Get the first link to visit
  std::string next_link = links_to_visit.front();
  links_to_visit.pop();
  return next_link;
}

void LinkManager::RequeLink(const std::string &link) {
  // Requeue the link to visit again
  links_to_visit.push(link);
}

bool LinkManager::IsCrawlAllowed(const std::string &link) const {
  auto host = utils::GetHostFromUrl(link);
  if (robots_txt_parsers.contains(host)) {
    bool is_allowed = robots_txt_parsers.at(host)->IsAllowed(
        utils::GetPathFromUrl(link), SEARCHLIGHT_CRAWLER_USER_AGENT);
    return is_allowed;
  }

  return true; // If no robots.txt is found, allow by default
}

bool LinkManager::HasEnoughDelay(const std::string &link) const {
  auto host = utils::GetHostFromUrl(link);
  if (visited_hosts.contains(host)) {
    int crawl_delay = robots_txt_parsers.at(host)
                          ->GetCrawlDelay(SEARCHLIGHT_CRAWLER_USER_AGENT)
                          .value_or(default_delay);
    return std::chrono::steady_clock::now() - visited_hosts.at(host) >=
           std::chrono::seconds(crawl_delay);
  }
  return true; // If no previous visit, allow by default
}

// Private methods

bool LinkManager::isRelativeLink(const std::string &link) const {
  return !(link.starts_with("http"));
}

bool LinkManager::isBasedOnSeedLink(const std::string &link) const {
  for (const auto &seed_link : seed_links) {
    if (link.starts_with(seed_link)) {
      return true;
    }
  }
  return false;
}

} // namespace crawler
