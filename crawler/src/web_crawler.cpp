// SPDX-License-Identifier: AGPL-3.0-only
#include "web_crawler.hpp"

#include <curl/curl.h>
#include <curl/easy.h>
#include <iostream>
#include <optional>
#include <string>

namespace crawler {

void CURLDeleter::operator()(CURL *curl) const {
  if (curl) {
    curl_easy_cleanup(curl);
  }
}

WebCrawler::WebCrawler() {
  if (!is_curl_global_init) {
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
      std::cerr << "curl_global_init() failed" << std::endl;
      throw std::runtime_error("Failed to initialize cURL");
    }
    is_curl_global_init = true;
  }

  CURL *handle = curl_easy_init();
  if (!handle) {
    std::cerr << "curl_easy_init() failed" << std::endl;
    throw std::runtime_error("Failed to initialize cURL handle");
  }
  curl.reset(handle);

  link_regex =
      std::regex(R"(<a\s+[^>]*href\s*=\s*(?:["']([^"']+)["']|([^\s>]+)))",
                 std::regex::icase);

  title_regex =
      std::regex(R"(<title[^>]*>([\s\S]*?)<\/title>)", std::regex::icase);
}

WebCrawler::~WebCrawler() = default;

std::optional<PageResult> WebCrawler::GetPage(const std::string &url) {
  CURLcode res;
  std::string read_buffer;

  // Set the URL and options that we want to fetch
  curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 0L);
  curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &read_buffer);
  curl_easy_setopt(curl.get(), CURLOPT_USERAGENT,
                   "SearchLight/0.1 (WebCrawler)");

  // Perform the request, res will get the return code
  // (not the HTTP status code)
  res = curl_easy_perform(curl.get());

  // Check for errors
  if (res != CURLE_OK) {
    std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
              << std::endl;
    return std::nullopt;
  } else {
    int http_code = 0;
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &http_code);

    if (http_code == 301 || http_code == 302) {
      char *redirect_url = nullptr;
      curl_easy_getinfo(curl.get(), CURLINFO_REDIRECT_URL, &redirect_url);
      return std::make_optional(
          PageResult{.content = std::nullopt,
                     .title = std::nullopt,
                     .links = {redirect_url ? redirect_url : ""}});
    }

    if (http_code == 200) {
      PageResult result;
      result.content = read_buffer;

      std::smatch title_match;
      if (std::regex_search(read_buffer, title_match, title_regex)) {
        if (title_match.size() > 1) {
          result.title = title_match[1].str();
        }
      } else {
        result.title = std::nullopt;
      }

      auto links_begin = std::sregex_iterator(read_buffer.begin(),
                                              read_buffer.end(), link_regex);
      auto links_end = std::sregex_iterator();

      for (std::sregex_iterator i = links_begin; i != links_end; ++i) {
        std::smatch match = *i;
        std::string link;

        if (match[1].matched) { // quoted link
          link = match[1].str();
        } else if (match[2].matched) { // unquoted link
          link = match[2].str();
        }

        if (!link.empty()) {
          result.links.push_back(link);
        }
      }

      return std::make_optional(result);
    }
  }

  return std::nullopt;
}

std::size_t WebCrawler::writeCallback(void *contents, std::size_t size,
                                      std::size_t nmemb, void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

} // namespace crawler
