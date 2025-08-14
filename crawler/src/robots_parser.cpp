// SPDX-License-Identifier: AGPL-3.0-only
#include "robots_parser.hpp"

#include <algorithm>
#include <charconv>
#include <optional>
#include <sstream>

#include "utils.hpp"

namespace crawler {

// Default constructor: results in an empty `rules` map, leading to an "allow
// all" policy.
RobotsParser::RobotsParser() = default;

// Constructor that parses file content.
RobotsParser::RobotsParser(const std::string &content) {
  std::stringstream stream(content);
  std::string line;
  std::vector<std::string> current_agents;

  while (std::getline(stream, line)) {
    // Remove comments
    if (auto comment_pos = line.find('#'); comment_pos != std::string::npos) {
      line = line.substr(0, comment_pos);
    }

    // Split line into key and value
    auto colon_pos = line.find(':');
    if (colon_pos == std::string::npos) {
      continue; // Not a valid rule line
    }

    std::string key = utils::NormalizeKey(line.substr(0, colon_pos));
    std::string value = utils::TrimValue(line.substr(colon_pos + 1));

    if (key.empty() || value.empty()) {
      continue;
    }

    if (key == "user-agent") {
      current_agents.clear();
      current_agents.push_back(normalizeUserAgent(value));
    } else if (key == "allow") {
      for (const auto &agent : current_agents) {
        rules[agent].allow_patterns.push_back(value);
      }
    } else if (key == "disallow") {
      for (const auto &agent : current_agents) {
        rules[agent].disallow_patterns.push_back(value);
      }
    } else if (key == "crawl-delay") {
      int delay = 0;
      auto [ptr, ec] =
          std::from_chars(value.data(), value.data() + value.size(), delay);
      if (ec == std::errc()) {
        for (const auto &agent : current_agents) {
          rules[agent].crawl_delay = delay;
        }
      }
    }
  }
}

// Normalizes a user-agent string: converts to lowercase and removes version
// info.
std::string RobotsParser::normalizeUserAgent(const std::string &agent) {
  std::string lower_agent = agent;
  std::transform(lower_agent.begin(), lower_agent.end(), lower_agent.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (auto slash_pos = lower_agent.find('/'); slash_pos != std::string::npos) {
    return lower_agent.substr(0, slash_pos);
  }
  return lower_agent;
}

const RuleGroup *
RobotsParser::findRulesForAgent(const std::string &user_agent) const {
  std::string normalized_agent = normalizeUserAgent(user_agent);

  // Try for a specific match first using the normalized agent
  if (auto it = rules.find(normalized_agent); it != rules.end()) {
    return &it->second;
  }
  // Fall back to the wildcard agent "*"
  if (auto it = rules.find("*"); it != rules.end()) {
    return &it->second;
  }
  return nullptr; // No applicable rules
}

bool RobotsParser::IsAllowed(const std::string &path,
                             const std::string &user_agent) const {
  const RuleGroup *group = findRulesForAgent(user_agent);
  if (!group) {
    return true; // If no rules apply, everything is allowed by default.
  }

  size_t longest_disallow_match = 0;
  for (const auto &pattern : group->disallow_patterns) {
    if (path.rfind(pattern, 0) == 0) { // Check if path starts with pattern
      if (pattern.length() > longest_disallow_match) {
        longest_disallow_match = pattern.length();
      }
    }
  }

  size_t longest_allow_match = 0;
  for (const auto &pattern : group->allow_patterns) {
    if (path.rfind(pattern, 0) == 0) {
      if (pattern.length() > longest_allow_match) {
        longest_allow_match = pattern.length();
      }
    }
  }

  // If a disallow rule is more specific, disallow.
  // Otherwise, it's allowed (either by a more specific allow rule or by
  // default).
  return longest_allow_match >= longest_disallow_match;
}

std::optional<int>
RobotsParser::GetCrawlDelay(const std::string &user_agent) const {
  const RuleGroup *group = findRulesForAgent(user_agent);
  if (group && group->crawl_delay.has_value()) {
    return group->crawl_delay;
  }
  return std::nullopt;
}

} // namespace crawler
