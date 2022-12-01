#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
 
#include "mariadb/conncpp.hpp"

// 实际修改的记录条数，(100W条耗时太长)
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

void find_all() {
    auto conn = get_conn();
    std::unique_ptr<sql::PreparedStatement> stmt1(conn->prepareStatement(
        "SELECT * FROM to_collect_energy"
    ));
    sql::ResultSet *rs1 = stmt1->executeQuery();
    while (rs1->next()) {
        auto id = rs1->getInt("id");
        auto user_id = rs1->getString("user_id");
        auto to_collect_energy = rs1->getInt("to_collect_energy");
        std::cout << "id = " << id << ", user_id = " << user_id \
            << ", to_collect_energy = " << to_collect_energy << std::endl;
    }
}

void insert() {
    auto conn = get_conn();
    std::unique_ptr<sql::PreparedStatement> stmt1(conn->prepareStatement(
        "insert into to_collect_energy values(123, null, null, 123, 10000, 'EMPTY');"
    ));
    sql::ResultSet *rs1 = stmt1->executeQuery();
    while (rs1->next()) {
    }
}

void create_to_collect_energy() {
    auto conn = get_conn();
    {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "drop table to_collect_energy;"
        ));
        stmt->executeQuery();
        std::cout << "success: start drop table to_collect_energy" << std::endl;
    }
    {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "create table to_collect_energy\
                (\
                id                int auto_increment\
                primary key,\
                gmt_create        timestamp   null,\
                gmt_modified      timestamp   null,\
                user_id           varchar(64) null,\
                to_collect_energy int         null,\
                status            varchar(32) null\
                );"
            ));
        stmt->executeQuery();
        std::cout << "success: create table to_collect_energy" << std::endl;
    }
    {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "LOAD DATA LOCAL INFILE '../to-collect-energy_10W.csv' INTO TABLE to_collect_energy FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '\"' LINES TERMINATED BY '\n';"
            ));
        using namespace std::chrono_literals;
        auto start = std::chrono::high_resolution_clock::now();
        stmt->executeQuery();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end-start;
        std::cout << "success: create table to_collect_energy, elapsed time: " << elapsed.count() << "ms" << std::endl;
    }
}

void toCollectEnergy_update(const std::shared_ptr<sql::Connection> &conn, int toCollectEnergy, const char *status, int id) {
    std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "update to_collect_energy set to_collect_energy = ?, status = ? where id = ?"
    ));
    stmt->setInt(1, toCollectEnergy);
    stmt->setString(2, status);
    stmt->setInt(3, id);
    stmt->executeUpdate();
}

void multi_statement_update(bool is_single_txn) {
    auto conn = get_conn();
    conn->setAutoCommit(!is_single_txn);
    for (int id = 0; id < RECORD_NUM; id++) {
        toCollectEnergy_update(conn, /* toCollectEnergy */id, /* status */"EMPTY", id);
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

int main() {
    // find_all();
    // insert();
    create_to_collect_energy();
    multi_statement_single_txn_update();
    create_to_collect_energy();
    multi_statement_multi_txn_update();
    return 0;
}
