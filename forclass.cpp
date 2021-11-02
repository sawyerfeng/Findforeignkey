//
// Created by Pygmalion on 2021/10/29.
//
#include<iostream>
#include"mysql.h"
#include<string>
#include<list>
using namespace std;

class DBUtils{
private: const char *host="127.0.0.1";
    const char *user="root";
    const char *passwd="zdh622";
    const char *dbname="usedbookstore";
    const int port=3306;
public:
    MYSQL mysql_conn;
    MYSQL_RES *result;
    //查询的数据库名，目前是类中指定，可以后期更改
    string QUERY_SCHEMA="usedbookstore";
    //外键结构体
    struct foreign_key{
        string table_name;
        string column_name;
        string constraint_name;
    };
    //表中字段结构体
    struct table_info{
        string column_name;
        string data_type;
    };
    void init_mysql();
    void close_mysql();
    list<foreign_key> query_foreignkey(string TABLE_NAME);
    list<string> query_tablename(string database_name);
    list<table_info> query_tableinfo(string table_name);
};

void DBUtils::init_mysql() {
    mysql_init(&mysql_conn);   //初始化MYSQL变量
    if (mysql_real_connect(&mysql_conn, host, user, passwd, dbname, 3306, 0, 0))  //连接到mysql 账号 密码 数据库名 端口
    {
        cout << "----"<<host<<"地址的MySQL已连接,数据库是" <<dbname<<"----"<< endl;
    }
    else
    {
        cout << "----MySQL连接失败----" << endl;
    }
}

void DBUtils::close_mysql() {
    mysql_close(&mysql_conn);
    mysql_free_result(result);
    cout << "----MySQL断开连接----" << endl;
}
/**
 * 查询表中外键的名字
 * 类给定了数据库名字(若不符合要求可以修改）
 * Input:表名
 * Output:list<foreign_key> 外键结构体的一个list，包含外键名，引用的表，和字段名
 * eg:
    ----外键共有：2个----
    -------------------------------------
    外键名为：ccid
    外键列名：cid
    外键对应表名:customerinfo
    -------------------------------------
    外键名为：csid
    外键列名：sid
    外键对应表名:storeinfo
    -------------------------------------
 */
list<DBUtils::foreign_key> DBUtils::query_foreignkey(string TABLE_NAME) {
    //建立连接
    init_mysql();
    MYSQL_FIELD *field = NULL;
    MYSQL_ROW row;
    string sql =
            "select REFERENCED_TABLE_NAME,CONSTRAINT_NAME,REFERENCED_COLUMN_NAME from information_schema.key_column_usage t where t.constraint_schema='" +
            QUERY_SCHEMA +
            "' and t.constraint_name in (SELECT CONSTRAINT_NAME FROM information_schema.TABLE_CONSTRAINTS where CONSTRAINT_SCHEMA='" +
            QUERY_SCHEMA + "' and CONSTRAINT_TYPE='FOREIGN KEY' and TABLE_NAME='" + TABLE_NAME + "')";
    if (!mysql_query(&mysql_conn, sql.c_str()))  //若查询成功返回0，失败返回随机数
    {
        cout << "----查询外键成功----" << endl;
    }
    result = mysql_store_result(&mysql_conn);    //将查询到的结果集储存到result中
    list<foreign_key> foreign_list;
    while ((row = mysql_fetch_row(result))) {
        //do something with row[name_field]
        foreign_key foreignKey1;
        foreignKey1.column_name = row[2];
        foreignKey1.constraint_name = row[1];
        foreignKey1.table_name = row[0];
        foreign_list.push_back(foreignKey1);
    }
    close_mysql();
    return foreign_list;
}
/**
 * 查询数据库中每个表的名字
 * Input:数据库名
 * Output：list<string> 表名的一个list
 * eg:
    ----usedbookstore共有11个表----
    1:purchase
    2:storeinfo
    3:cfoucuss
    4:accounts
    5:store
    6:book_amount
    7:shopping_cart
    8:customerinfo
    9:books
    10:customeradr
    11:admininfo
 */

list<string> DBUtils::query_tablename(string database_name) {
    init_mysql();
    MYSQL_FIELD *field = NULL;
    MYSQL_ROW row;
    string sql="SELECT TABLE_NAME FROM information_schema.`TABLES` WHERE TABLE_SCHEMA='"+database_name+"'";
    if (!mysql_query(&mysql_conn, sql.c_str()))  //若查询成功返回0，失败返回随机数
    {
        cout << "----查询表名成功----" << endl;
    }
    result = mysql_store_result(&mysql_conn);    //将查询到的结果集储存到result中
    list<string> schema_list;
    while ((row = mysql_fetch_row(result))) {
        schema_list.push_back(row[0]);
    }
    close_mysql();
    return schema_list;
}
/**
 * 查询表中信息
 * Input:查询表的名字
 * Output:输出结构体table_info,包含字段名和数据类型
 * eg:
 * ----usedbookstore共有5个字段----
    -------------------------------------
    表中字段为为：tel--数据类型为:varchar
    -------------------------------------
    表中字段为为：username--数据类型为:varchar
    -------------------------------------
    表中字段为为：userpwd--数据类型为:varchar
    -------------------------------------
    表中字段为为：email--数据类型为:varchar
    -------------------------------------
    表中字段为为：role--数据类型为:enum
    -------------------------------------

 */
list<DBUtils::table_info> DBUtils::query_tableinfo(string table_name) {
    init_mysql();
    MYSQL_FIELD *field = NULL;
    MYSQL_ROW row;
    string sql="SELECT COLUMN_NAME,DATA_TYPE FROM information_schema.`COLUMNS` WHERE TABLE_SCHEMA='"+QUERY_SCHEMA+"' and TABLE_NAME='"+table_name+"'";
    if (!mysql_query(&mysql_conn, sql.c_str()))  //若查询成功返回0，失败返回随机数
    {
        cout << "----查询表信息成功----" << endl;
    }
    result = mysql_store_result(&mysql_conn);    //将查询到的结果集储存到result中
    list<table_info> tableinfo_list;
    while ((row = mysql_fetch_row(result))) {
        table_info tableInfo;
        tableInfo.column_name=row[0];
        tableInfo.data_type=row[1];
        tableinfo_list.push_back(tableInfo);
    }
    close_mysql();
    return tableinfo_list;

}
int main(){
    string table_name="cfoucuss";
    string database_name="usedbookstore";
    DBUtils dbUtils;
    //测试查询外键
    list<DBUtils::foreign_key> foreign_list=dbUtils.query_foreignkey(table_name);
    cout<<"----外键共有："<<foreign_list.size()<<"个----"<<endl;
    list<list<DBUtils::foreign_key>>::iterator it;
    for(list<DBUtils::foreign_key>::iterator it=foreign_list.begin();it!=foreign_list.end();it++){
        cout<<"-------------------------------------"<<endl;
        cout<<"外键名为："<<it->constraint_name<<endl;
        cout<<"外键列名："<<it->column_name<<endl;
        cout<<"外键对应表名:"<<it->table_name<<endl;
    }
    cout<<"-------------------------------------"<<endl;
    //测试查询数据库表名
    list<string> schema_list=dbUtils.query_tablename(database_name);
    list<string>::iterator it1;
    cout<<"----"<<database_name<<"共有"<<schema_list.size()<<"个表----"<<endl;
    int i=1;
    for(it1=schema_list.begin();it1!=schema_list.end();it1++,i++){
        cout<<i<<":";
        cout<<it1->c_str()<<endl;

    }
    //测试查询表信息
    list<DBUtils::table_info> tableinfo_list =dbUtils.query_tableinfo("accounts");
    list<string>::iterator it2;
    cout<<"----"<<dbUtils.QUERY_SCHEMA<<"共有"<<tableinfo_list.size()<<"个字段----"<<endl;
    for(list<DBUtils::table_info>::iterator it2=tableinfo_list.begin();it2!=tableinfo_list.end();it2++){
        cout<<"-------------------------------------"<<endl;
        cout<<"表中字段为为："<<it2->column_name<<"--";
        cout<<"数据类型为:"<<it2->data_type<<endl;
    }
    cout<<"-------------------------------------"<<endl;

}