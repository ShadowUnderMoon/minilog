#pragma once

#include <filesystem>
#include <format>
#include <string>
#include <iostream>
#include <magic_enum.hpp>
#include <mysql++/mysql++.h>

#include <minilog/log_msg.h>

namespace minilog::sinks {
class DBSink {
public:
    static mysqlpp::Connection& getConnection() {
        static DBSink db_sink;
        return db_sink.conn;
    }

    DBSink(const DBSink &) = delete;
    DBSink& operator=(const DBSink &) = delete;
private:
    DBSink() : conn("logs", std::getenv("MINILOG_DB_HOST"), std::getenv("MINILOG_DB_USER"), std::getenv("MINILOG_DB_PASSWORD")) {}
    mysqlpp::Connection conn;
};

inline void sink_to_db(const log_msg &msg) {
    try {
        mysqlpp::Connection conn = DBSink::getConnection();
        if (conn.connected()) {
            std::filesystem::path absolute_path = msg.location.file_name();
            const char *sql_stat = "INSERT INTO logs (log_time, level, message, filename, linenumber) VALUES (%0q, %1q, %2q, %3q, %4q);";
            
            //     msg.time, magic_enum::enum_name(msg.level), msg.payload, std::string(absolute_path.filename()), msg.location.line());
            // std::cout << std::string(10, '-') << std::endl;
            // std::cout << sql_stat << std::endl;
            // std::cout << std::string(10, '-') << std::endl;
            mysqlpp::Query query = conn.query(sql_stat);
            query.parse();
            if (auto res = query.execute(
                format("{}", msg.time).c_str(),
                std::string(magic_enum::enum_name(msg.level)).c_str(),
                std::string(msg.payload).c_str(),
                std::string(absolute_path.filename()).c_str(),
                std::format("{}",msg.location.line()).c_str())) {
                std::cout << "inserted " << res.rows() << " rows into the table" << std::endl;
            }            
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
}