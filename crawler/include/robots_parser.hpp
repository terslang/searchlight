// SPDX-License-Identifier: AGPL-3.0-only
#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace crawler {

// A structure to hold the rules for a group of user-agents.
struct RuleGroup {
  std::vector<std::string> allow_patterns;
  std::vector<std::string> disallow_patterns;
  std::optional<int> crawl_delay;
};

class RobotsParser {
public:
  // Default constructor for an "allow all" policy (e.g., when robots.txt is
  // missing).
  RobotsParser();

  // Constructor that parses content from a robots.txt file.
  explicit RobotsParser(const std::string &content);

  // Checks if a given path is allowed for a specific user-agent.
  // This method is case-insensitive for the user-agent and ignores version
  // numbers.
  bool IsAllowed(const std::string &path, const std::string &user_agent) const;

  // Gets the crawl delay specified for a user-agent, if any.
  std::optional<int> GetCrawlDelay(const std::string &user_agent) const;

private:
  // Internal storage mapping a normalized user-agent string to its specific
  // rules.
  std::map<std::string, RuleGroup> rules;

  // Helper to find the most relevant RuleGroup for a given user-agent.
  const RuleGroup *findRulesForAgent(const std::string &user_agent) const;

  // Helper to normalize user-agent strings (lowercase, no version).
  static std::string normalizeUserAgent(const std::string &agent);
};

} // namespace crawler
