//
// Created by Pygmalion on 2021/10/29.
//
#include<iostream>
#include"mysql.h"
#include<string>
#include<list>
#include<vector>
#include <iomanip>
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
    /**
     * 查询A表，对应B表
     */
    struct foreign_key{
        string btable_name;
        string bcolumn_name;
        string constraint_name;
        string acolumn_name;
        string atable_name;
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
    vector<vector<string>> query_tabledata(string table_name);
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
    result=NULL;
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
    查询表的表名:cfoucuss
    查询表中外键列名：cid
    外键对应表名:customerinfo
    对应表中外键列名：cid
    -------------------------------------
    外键名为：csid
    查询表的表名:cfoucuss
    查询表中外键列名：sid
    外键对应表名:storeinfo
    对应表中外键列名：sid
    -------------------------------------
 */
list<DBUtils::foreign_key> DBUtils::query_foreignkey(string TABLE_NAME) {
    //建立连接
    init_mysql();
    MYSQL_FIELD *field = NULL;
    MYSQL_ROW row;
    string sql =
            "select REFERENCED_TABLE_NAME,CONSTRAINT_NAME,REFERENCED_COLUMN_NAME,COLUMN_NAME from information_schema.key_column_usage t where t.constraint_schema='" +
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
        foreignKey1.bcolumn_name = row[2];
        foreignKey1.constraint_name = row[1];
        foreignKey1.btable_name = row[0];
        foreignKey1.acolumn_name=row[3];
        foreignKey1.atable_name=TABLE_NAME;
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
/***
 * 查询表中字段，并按照查询table_info中顺序输出
 * Input:查询表的名字
 * Output:vector<vector<string>>，输出table_info顺序的字段信息
 *eg:
 *  ----查询的是cid,cname,csex,birth,motto,cmoney,tel,结果如下:
    20004 王建新 男 1997-02-15 好好学习. 45.000 18239165907
    20008 "" "" null "" 4000.000 99998888777
    20009 李澳 男 1999-12-14 学习使我快乐 450.000 15824827198
    20010 李淑婉 女 1999-01-01 不吃饭则饥，不读书则愚。 100.000 13509230000
    20012 李咏德 男 1999-01-01 不能则学，不知则问，耻于问人，决无长进。 100.000 13509230001
    20013 李红英 女 1999-01-01 不听指点，多绕弯弯。不懂装懂，永世饭桶。 100.000 13509230002
    20014 李新荣 男 1999-01-01 不问的人永远和愚昧在一起。 100.000 13509230003
 */
vector<vector<string>> DBUtils::query_tabledata(string table_name) {
    list<table_info> tableinfo_list=query_tableinfo(table_name);
    string columns;
    int num=tableinfo_list.size();
    list<table_info>::iterator it;
    for(it=tableinfo_list.begin();it!=tableinfo_list.end();it++){
        columns.append(it->column_name);
        columns.append(",");
    }
    string sql="SELECT "+columns.substr(0,columns.size()-1)+" FROM "+table_name;
    cout<<sql;
    init_mysql();
    MYSQL_FIELD *field = NULL;
    MYSQL_ROW row;
    if (!mysql_query(&mysql_conn, sql.c_str()))  //若查询成功返回0，失败返回随机数
    {
        cout << "----查询表字段成功----" << endl;
        cout<<"----查询的是"+columns+"结果如下:"<<endl;
    }else
        cout<<"查询失败";
    result = mysql_store_result(&mysql_conn);//将查询到的结果集储存到result中
    int num_fields = mysql_num_fields(result);
    vector<vector<string>> tabledata_list;
    while ((row = mysql_fetch_row(result))) {
        vector<string> table_data;
        for(int i=0;i<num_fields;i++){
            if(row[i]==NULL)
                table_data.push_back("null");
            else if(!strcmp(row[i]," "))
                table_data.push_back("\"\"");
            else
                table_data.push_back(row[i]);
        }
        tabledata_list.push_back(table_data);
    }
    close_mysql();
    return tabledata_list;
}
int main(){
    string table_name="customerinfo";
    string database_name="usedbookstore";
    DBUtils dbUtils;
    
    //测试查询外键
    list<DBUtils::foreign_key> foreign_list=dbUtils.query_foreignkey(table_name);
    cout<<"----外键共有："<<foreign_list.size()<<"个----"<<endl;
    list<list<DBUtils::foreign_key>>::iterator it;
    for(list<DBUtils::foreign_key>::iterator it=foreign_list.begin();it!=foreign_list.end();it++){
        cout<<"-------------------------------------"<<endl;
        cout<<"外键名为："<<it->constraint_name<<endl;
        cout<<"查询表的表名:"<<it->atable_name<<endl;
        cout<<"查询表中外键列名："<<it->acolumn_name<<endl;
        cout<<"外键对应表名:"<<it->btable_name<<endl;
        cout<<"对应表中外键列名："<<it->bcolumn_name<<endl;
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

    //测试查询数据
    vector<vector<string>> tabledata_list =dbUtils.query_tabledata(table_name);
    cout<<setiosflags(ios::left);
    for(int i=0;i<tabledata_list.size();i++){
        for(int j=0;j<tabledata_list[i].size();j++){
            cout<<tabledata_list[i][j]<<" ";
        }
        cout<<endl;
    }
}