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
# 2.1 生成100W行数据
# 2.2 进入数据库，执行导入
```sql
LOAD DATA LOCAL INFILE 'to-collect-energy_100W.csv' INTO TABLE to_collect_energy FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '"' LINES TERMINATED BY '\n';

LOAD DATA LOCAL INFILE 'total-energy_100W.csv' INTO TABLE total_energy 
FIELDS TERMINATED BY ',' 
OPTIONALLY ENCLOSED BY '"' 
LINES TERMINATED BY '\n';
```
- `to_collect_energy`表，导入时间`5.517 sec`
- `total-energy`表，导入时间`10.051 sec`

# total_energy 表测试

# to_collect_energy 表测试

# 总结
