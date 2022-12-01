#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <thread>
 
#include "mariadb/conncpp.hpp"

// 实际修改的记录条数，(100W条耗时太长)
// #define RECORD_NUM (10 * 10000)
#define RECORD_NUM (10 * 10000)

sql::Driver *driver = sql::mariadb::get_driver_instance();
// Configure Connection
sql::SQLString url("jdbc:mariadb://127.0.0.1/atec2022");
sql::Properties properties(
    {
        {"user",     "root"},
        {"password", "111111"},
        {"allowLocalInfile", "true"}
    }
);

std::shared_ptr<sql::Connection> get_conn() {
    std::shared_ptr<sql::Connection> conn(driver->connect(url, properties));
    return conn;
}

void create_total_energy() {
    auto conn = get_conn();
    {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "drop table total_energy;"
        ));
        stmt->executeQuery();
        std::cout << "success: start drop table total_energy" << std::endl;
    }
    {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "create table total_energy\
                (\
                id           int auto_increment\
                primary key,\
                gmt_create   datetime    null,\
                gmt_modified datetime    null,\
                user_id      varchar(64) null,\
                total_energy int         null,\
                constraint total_energy_pk\
                unique (user_id)\
                );"
            ));
        stmt->executeQuery();
        std::cout << "success: create table total_energy" << std::endl;
    }
    {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "LOAD DATA LOCAL INFILE '../total-energy_10W.csv' INTO TABLE total_energy FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '\"' LINES TERMINATED BY '\n';"
            ));
        using namespace std::chrono_literals;
        auto start = std::chrono::high_resolution_clock::now();
        stmt->executeQuery();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end-start;
        std::cout << "success: create table total_energy, elapsed time: " << elapsed.count() << "ms" << std::endl;
    }
}

void totalEnergy_update(const std::shared_ptr<sql::Connection> &conn, const char *user_id, int totalEnergy) {
    std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "update total_energy set total_energy = ? where user_id = ?"
    ));
    stmt->setInt(1, totalEnergy);
    stmt->setString(2, user_id);
    stmt->executeUpdate();
}

void multi_statement_update(bool is_single_txn) {
    auto conn = get_conn();
    conn->setAutoCommit(!is_single_txn);
    for (int id = 0; id < RECORD_NUM; id++) {
        std::string user_id = std::to_string(id);
        totalEnergy_update(conn, /* user_id */user_id.c_str(), /* totalEnergy */id);
    }
    if (is_single_txn) {
        conn->commit();
    }
}

void multi_statement_multi_txn_update() {
    using namespace std::chrono_literals;
    auto start = std::chrono::high_resolution_clock::now();
    multi_statement_update(false);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout << "success: multi_statement_multi_txn_update, elapsed time: " << elapsed.count() << " ms\n"; 
}

void multi_statement_single_txn_update() {
    using namespace std::chrono_literals;
    auto start = std::chrono::high_resolution_clock::now();
    multi_statement_update(true);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout << "success: multi_statement_single_txn_update, elapsed time: " << elapsed.count() << " ms\n"; 
}

void rewrite_update() {
    auto conn = get_conn();
    using namespace std::chrono_literals;
    auto start = std::chrono::high_resolution_clock::now();
    std::string stmt_str;
    stmt_str.reserve(23000000);
    stmt_str = "update total_energy as m, ( select 0 as c1, \"0\" as c2 ";
    for (int id = 1; id < RECORD_NUM; id++) {
        stmt_str += "union all select "+ std::to_string(id) +", "+ std::to_string(id) + " ";
    }
    stmt_str += ") as r set m.total_energy = r.c1 where m.user_id = r.c2;";
    std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(stmt_str));
    // note(wq): 下列做法，非常费时!
    // for (int i = 0; i < RECORD_NUM; i++) {
    //     stmt->setInt(2 * i + 1, i);
    //     stmt->setString(2 * i + 2, std::to_string(i));
    // }
    std::cout << "--------------------------------------------\n"; 
    stmt->executeQuery();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout << "success: rewrite_update, elapsed time: " << elapsed.count() << " ms\n"; 
}

int main() {
    create_total_energy();
    multi_statement_single_txn_update();
    create_total_energy();
    multi_statement_multi_txn_update();
    create_total_energy();
    rewrite_update();
    return 0;
}
