# 1. 启动mariadb
## 1.1 启动一个mariadb容器
```sh
docker pull mariadb:5.5.64
mkdir -p /data/mariadb/data
docker run --name mariadb -p 3306:3306 -e MYSQL_ROOT_PASSWORD=111111 -v /data/mariadb/data:/var/lib/mysql -d mariadb:5.5.64
# 确认容器启动
docker ps -a
docker container update --restart=always mariadb
docker exec -it mariadb bash
```

## 1.2 连接mariadb
```sh
# 连接数据库
mysql -h127.0.0.1 -uroot -p111111
```
## 1.3 创建表
```sql
CREATE DATABASE atec2022;
use atec2022;
drop table total_energy;
create table total_energy
    (
    id           int auto_increment
    primary key,
    gmt_create   datetime    null,
    gmt_modified datetime    null,
    user_id      varchar(64) null,
    total_energy int         null,
    constraint total_energy_pk
    unique (user_id)
    );
drop table to_collect_energy;
create table to_collect_energy
    (
    id                int auto_increment
    primary key,
    gmt_create        timestamp   null,
    gmt_modified      timestamp   null,
    user_id           varchar(64) null,
    to_collect_energy int         null,
    status            varchar(32) null
    );
```
## 1.4 确认表结构正确：
```sql
desc total_energy;
desc to_collect_energy;
```

# 2. 表导入数据
`生成数据`
```py
python generate_csv.py 10 #生成10W行数据
python generate_csv.py 100 #生成100W行数据
```
# 2.1 100W行数据
`执行导入的语句`
```sql
LOAD DATA LOCAL INFILE 'to-collect-energy_100W.csv' INTO TABLE to_collect_energy FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '"' LINES TERMINATED BY '\n';

LOAD DATA LOCAL INFILE 'total-energy_100W.csv' INTO TABLE total_energy FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '"' LINES TERMINATED BY '\n';
```
- `to_collect_energy`表，导入时间`5517 ms`
- `total-energy`表，导入时间`10051 ms`

# 2.2 10W行数据
```sql
LOAD DATA LOCAL INFILE 'to-collect-energy_10W.csv' INTO TABLE to_collect_energy FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '"' LINES TERMINATED BY '\n';

LOAD DATA LOCAL INFILE 'total-energy_10W.csv' INTO TABLE total_energy FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '"' LINES TERMINATED BY '\n';
```
- `to_collect_energy`表，导入时间`546 ms`
- `total-energy`表，导入时间`996 ms`

# 3. total_energy 10W表测试
# 3.1 多语句批更新
`update total_energy set total_energy = ? where user_id = ?`
# 3.2 多事务
```sql
update total_energy set total_energy = 0; where user_id = "0";
update total_energy set total_energy = 1; where user_id = "1";
update total_energy set total_energy = 2; where user_id = "2";
-- ......
```
耗时`12697.3 ms`
# 3.3 单事务
```sql
START TRANSACTION;
update total_energy set total_energy = 0; where user_id = "0";
update total_energy set total_energy = 1; where user_id = "1";
update total_energy set total_energy = 2; where user_id = "2";
-- ......
COMMIT;
```
耗时`12134.1 ms`
# 4. to_collect_energy 10W表测试
# 4.1 多语句批更新
`update to_collect_energy set to_collect_energy = ?, status = ? where id = ?`
# 4.2 多事务
```sql
update to_collect_energy set to_collect_energy = 0, status = "EMPTY" where id = 0;
update to_collect_energy set to_collect_energy = 1, status = "EMPTY" where id = 1;
update to_collect_energy set to_collect_energy = 2, status = "EMPTY" where id = 2;
-- ......
```
耗时 `68312.7 ms`
# 4.3 单事务
```sql
START TRANSACTION;
update to_collect_energy set to_collect_energy = 0, status = "EMPTY" where id = 0;
update to_collect_energy set to_collect_energy = 1, status = "EMPTY" where id = 1;
update to_collect_energy set to_collect_energy = 2, status = "EMPTY" where id = 2;
-- ......
COMMIT;
```
耗时 `14799.3 ms`
# 5. 总结
