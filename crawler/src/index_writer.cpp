#include <sqlite3.h>

#include "config.hpp"
#include "index_writer.hpp"
#include <iostream>

namespace crawler {
void SQLiteDbDeleter::operator()(sqlite3 *db) const {
  if (db) {
    sqlite3_close(db);
  }
}

void SQLiteStmtDeleter::operator()(sqlite3_stmt *stmt) const {
  if (stmt) {
    sqlite3_finalize(stmt);
  }
}

IndexWriter::IndexWriter(
    std::unique_ptr<crawler::DatabaseOptions> db_options) {
  // Open the SQLite database
  sqlite3 *raw_db_handle = nullptr;
  if (sqlite3_open(db_options->db_path.c_str(), &raw_db_handle) != SQLITE_OK) {
    std::string error_msg = sqlite3_errmsg(raw_db_handle);
    sqlite3_close(raw_db_handle);
    throw std::runtime_error("Failed to open SQLite database: " + error_msg);
  }

  db.reset(raw_db_handle);

  if (sqlite3_enable_load_extension(db.get(), 1) != SQLITE_OK) {
    std::string error_msg = sqlite3_errmsg(db.get());
    throw std::runtime_error("Failed to enable SQLite load extension: " +
                             error_msg);
  }

  char *err_msg = nullptr;
  if (sqlite3_load_extension(db.get(), db_options->fts_html_ext_path.c_str(), 0,
                             &err_msg) != SQLITE_OK) {
    std::string error_msg = err_msg ? err_msg : "Unknown error";
    sqlite3_free(err_msg);
    throw std::runtime_error("Failed to load FTS HTML extension: " + error_msg);
  }

  const char *sql = "INSERT INTO webpages(url, title, content) VALUES "
                    "(?, ?, ?) ON CONFLICT(url) DO UPDATE SET "
                    "title=excluded.title, content=excluded.content;";
  sqlite3_stmt *raw_stmt = nullptr;
  if (sqlite3_prepare_v2(db.get(), sql, -1, &raw_stmt, nullptr) != SQLITE_OK) {
    std::string error_msg = sqlite3_errmsg(db.get());
    sqlite3_close(db.get());
    throw std::runtime_error("Failed to prepare SQLite statement: " +
                             error_msg);
  }

  insert_stmt.reset(raw_stmt);
}

IndexWriter::~IndexWriter() = default;

bool IndexWriter::InsertPage(const std::string &url,
                             const PageResult &page_result) {
  sqlite3_bind_text(insert_stmt.get(), 1, url.c_str(), -1, SQLITE_TRANSIENT);

  if (page_result.title.has_value()) {
    sqlite3_bind_text(insert_stmt.get(), 2, page_result.title->c_str(), -1,
                      SQLITE_TRANSIENT);
  } else {
    sqlite3_bind_null(insert_stmt.get(), 2);
  }

  if (page_result.content.has_value()) {
    sqlite3_bind_text(insert_stmt.get(), 3, page_result.content->c_str(), -1,
                      SQLITE_TRANSIENT);
  } else {
    sqlite3_bind_null(insert_stmt.get(), 3);
  }

  if (sqlite3_step(insert_stmt.get()) != SQLITE_DONE) {
    std::cerr << "Failed to insert page into SQLite database: "
              << sqlite3_errmsg(db.get()) << std::endl;
    sqlite3_reset(insert_stmt.get());
    return false;
  }

  sqlite3_reset(insert_stmt.get());
  return true;
}
} // namespace crawler
