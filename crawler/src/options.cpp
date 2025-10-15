#include "options.hpp"
#include "config.hpp"

crawler::CrawlOptions::CrawlOptions() : default_delay(DEFAULT_CRAWL_DELAY) {}
crawler::CrawlOptions::CrawlOptions(int default_delay)
    : default_delay(default_delay) {}

crawler::DatabaseOptions::DatabaseOptions()
    : db_path(DB_PATH), fts_html_ext_path(FTS_HTML_EXT_PATH) {}
crawler::DatabaseOptions::DatabaseOptions(const std::string &db_path,
                                          const std::string &fts_html_ext_path)
    : db_path(db_path), fts_html_ext_path(fts_html_ext_path) {}

crawler::Options::Options() {
  crawl_options = std::make_unique<CrawlOptions>();
  database_options = std::make_unique<DatabaseOptions>();
}

crawler::Options::Options(const CrawlOptions &crawl_options,
                          const DatabaseOptions &database_options)
    : crawl_options(std::make_unique<CrawlOptions>(crawl_options)),
      database_options(std::make_unique<DatabaseOptions>(database_options)) {}

crawler::Options::Options(const YAML::Node &options_node) {
  if (options_node["crawl-opts"]) {
    auto crawl_node = options_node["crawl-opts"];
    int default_delay = crawl_node["default-delay"]
                            ? crawl_node["default-delay"].as<int>()
                            : DEFAULT_CRAWL_DELAY;
    crawl_options = std::make_unique<CrawlOptions>(default_delay);
  } else {
    crawl_options = std::make_unique<CrawlOptions>();
  }

  if (options_node["database-opts"]) {
    auto db_node = options_node["database-opts"];
    std::string db_path =
        db_node["db-path"] ? db_node["db-path"].as<std::string>() : DB_PATH;
    std::string fts_html_ext_path =
        db_node["fts-html-ext-path"]
            ? db_node["fts-html-ext-path"].as<std::string>()
            : FTS_HTML_EXT_PATH;
    database_options =
        std::make_unique<DatabaseOptions>(db_path, fts_html_ext_path);
  } else {
    database_options = std::make_unique<DatabaseOptions>();
  }

  if (options_node["seed-links"]) {
    for (const auto &link : options_node["seed-links"]) {
      seed_links.push_back(link.as<std::string>());
    }
  }
}
