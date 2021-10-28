#include<iostream>
#include"mysql.h"
#include<string>
#include<list>
using namespace std;
//数据库基本信息,可以在这里修改
const char *host="127.0.0.1";
const char *user="root";
const char *passwd="zdh622";
const char *dbname="usedbookstore";
const int port=3306;
string CONSTRAINT_SCHEMA="usedbookstore";
//存储结构体
struct foreign_key{
    string table_name;
    string column_name;
    string constraint_name;
};

//数据库连接及查询相关信息
MYSQL mysql_conn;
MYSQL_RES *result;

//连接数据库
void init_mysql() {
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

//断开连接数据库
void close_mysql() {
    mysql_close(&mysql_conn);
    mysql_free_result(result);
    cout << "----MySQL断开连接----" << endl;
}

//查询外键
list<foreign_key> query_foreignkey(string TABLE_NAME) {
    MYSQL_FIELD *field = NULL;
    MYSQL_ROW row;
    string sql =
            "select REFERENCED_TABLE_NAME,CONSTRAINT_NAME,REFERENCED_COLUMN_NAME from information_schema.key_column_usage t where t.constraint_schema='" +
            CONSTRAINT_SCHEMA +
            "' and t.constraint_name in (SELECT CONSTRAINT_NAME FROM information_schema.TABLE_CONSTRAINTS where CONSTRAINT_SCHEMA='" +
            CONSTRAINT_SCHEMA + "' and CONSTRAINT_TYPE='FOREIGN KEY' and TABLE_NAME='" + TABLE_NAME + "')";
    if (!mysql_query(&mysql_conn, sql.c_str()))  //若查询成功返回0，失败返回随机数
    {
        cout << "----查询成功----" << endl;
    }
    result = mysql_store_result(&mysql_conn);    //将查询到的结果集储存到result中
    list<foreign_key> foreign_list;
    while ((row = mysql_fetch_row(result))) {
        //do something with row[name_field]
        foreign_key foreignKey1;
        foreignKey1.column_name=row[2];
        foreignKey1.constraint_name=row[1];
        foreignKey1.table_name=row[0];
        foreign_list.push_back(foreignKey1);

    }
    return foreign_list;
}
int main() {
    //给定查询表名
    string table_name = "cfoucuss";
    //建立连接
    init_mysql();
    //获取外键列表
    list<foreign_key> foreign_list= query_foreignkey(table_name);
    //关闭数据库链接
    close_mysql();
    //输出阶段
    cout<<"----外键共有："<<foreign_list.size()<<"个----"<<endl;
    list<list<foreign_key>>::iterator it;
    for(list<foreign_key>::iterator it=foreign_list.begin();it!=foreign_list.end();it++){
        cout<<"-------------------------------------"<<endl;
        cout<<"外键名为："<<it->constraint_name<<endl;
        cout<<"外键列名："<<it->column_name<<endl;
        cout<<"外键对应表名:"<<it->table_name<<endl;
    }

}