// SPDX-License-Identifier: AGPL-3.0-only
#include "utils.hpp"

#include <algorithm>
#include <iostream>

#include "config.hpp"

namespace utils {

std::string NormalizeKey(const std::string &s) {
  size_t start = s.find_first_not_of(" \t");
  if (std::string::npos == start) {
    return "";
  }
  size_t end = s.find_last_not_of(" \t");
  std::string key = s.substr(start, end - start + 1);
  std::transform(key.begin(), key.end(), key.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return key;
}

std::string TrimValue(const std::string &s) {
  size_t start = s.find_first_not_of(" \t");
  if (std::string::npos == start) {
    return "";
  }
  return s.substr(start);
}

std::string GetHostFromUrl(const std::string &url_string) {
  auto url = ada::parse(url_string);
  if (!url) {
    std::cerr << "Invalid URL: " << url_string << std::endl;
    return "";
  }

  return std::string(url->get_host());
}

std::string GetBaseUrl(const std::string &url_string) {
  auto url = ada::parse(url_string);
  if (!url) {
    std::cerr << "Invalid URL: " << url_string << std::endl;
    return "";
  }

  return std::string(url->get_origin());
}

std::string GetPathFromUrl(const std::string &url_string) {
  auto url = ada::parse(url_string);
  if (!url) {
    std::cerr << "Invalid URL: " << url_string << std::endl;
    return "";
  }

  return std::string(url->get_pathname());
}

} // namespace utils