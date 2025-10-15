#pragma once

#include <memory>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace crawler {
class CrawlOptions {
public:
  CrawlOptions();
  CrawlOptions(int default_delay);

  int default_delay;
};

class DatabaseOptions {
public:
  DatabaseOptions();
  DatabaseOptions(const std::string &db_path,
                  const std::string &fts_html_ext_path);

  std::string db_path;
  std::string fts_html_ext_path;
};

class Options {
public:
  Options();
  Options(const CrawlOptions &crawl_options,
          const DatabaseOptions &database_options);
  Options(const YAML::Node &options_node);

  std::unique_ptr<CrawlOptions> crawl_options;
  std::unique_ptr<DatabaseOptions> database_options;
  std::vector<std::string> seed_links;
};
} // namespace crawler
