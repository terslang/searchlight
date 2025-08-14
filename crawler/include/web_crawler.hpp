// SPDX-License-Identifier: AGPL-3.0-only
#pragma once

#include <memory>
#include <optional>
#include <regex>
#include <string>
#include <vector>

namespace crawler {

typedef void CURL;

// Custom deleter for CURL to ensure proper cleanup
struct CURLDeleter {
  void operator()(CURL *curl) const;
};

struct PageResult {
  std::optional<std::string> content;
  std::optional<std::string> title;
  std::vector<std::string> links;
};

class WebCrawler {
public:
  // Constructor
  WebCrawler();

  // Destructor
  ~WebCrawler();

  std::optional<PageResult> GetPage(const std::string &url);

private:
  std::unique_ptr<CURL, CURLDeleter> curl;
  bool is_curl_global_init;
  std::regex link_regex;
  std::regex title_regex;

  static std::size_t writeCallback(void *contents, std::size_t size,
                                   std::size_t nmemb, void *userp);
};

} // namespace crawler
