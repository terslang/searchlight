#pragma once
#include <string>

#include "web_crawler.hpp"

struct sqlite3;
struct sqlite3_stmt;

namespace crawler {
struct SQLiteDbDeleter {
  void operator()(sqlite3 *db) const;
};

struct SQLiteStmtDeleter {
  void operator()(sqlite3_stmt *stmt) const;
};

class IndexWriter {
public:
  explicit IndexWriter(const std::string &db_path);
  ~IndexWriter();

  bool InsertPage(const std::string &url, const PageResult &page_result);

private:
  std::unique_ptr<sqlite3, SQLiteDbDeleter> db;
  std::unique_ptr<sqlite3_stmt, SQLiteStmtDeleter> insert_stmt;
};
} // namespace crawler
