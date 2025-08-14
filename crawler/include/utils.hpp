// SPDX-License-Identifier: AGPL-3.0-only
#pragma once

#include <ada.h>
#include <ada/implementation.h> // IWYU pragma: keep
#include <string>

namespace utils {

// Normalizes a rule's key (e.g., "User-agent") to lowercase and trims
// whitespace.
std::string NormalizeKey(const std::string &s);

// Trims whitespace from the beginning of a rule's value.
std::string TrimValue(const std::string &s);

// Extracts the host from a URL string.
std::string GetHostFromUrl(const std::string &url_string);

// Extracts the base URL from a URL string
std::string GetBaseUrl(const std::string &url_string);

// Extracts the path from a URL string.
std::string GetPathFromUrl(const std::string &url_string);
} // namespace utils