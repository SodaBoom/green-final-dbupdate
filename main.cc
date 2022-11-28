#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
 
#include "mariadb/conncpp.hpp"

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

void load_to_collect_energy_100W() {
    auto conn = get_conn();
    std::unique_ptr<sql::PreparedStatement> stmt1(conn->prepareStatement(
        "LOAD DATA LOCAL INFILE 'to-collect-energy_100W.csv' INTO TABLE to_collect_energy FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '\"' LINES TERMINATED BY '\n';"
    ));
    
    using namespace std::chrono_literals;
    auto start = std::chrono::high_resolution_clock::now();

    sql::ResultSet *rs1 = stmt1->executeQuery();
    while (rs1->next()) {}

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout << "load_to_collect_energy_100W: " << elapsed.count() << " ms\n";
}

int main() {
    find_all();
    insert();
    load_to_collect_energy_100W();
    return 0;
}
