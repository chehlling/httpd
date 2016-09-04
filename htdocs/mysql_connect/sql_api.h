#include <iostream>
#include <string>
#include <vector>
#include <mysql.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define _SIZE_ 2048

class mysql_api
{
public:
	mysql_api();
	void mysql_connect_sql();
	bool mysql_select(std::string id="",std::string name="",std::string sex="",std::string age="",std::string tel="",std::string school="");
	bool mysql_insert(std::string name,std::string sex,std::string age,std::string tel,std::string school);
	bool mysql_delete(std::string id="",std::string name="",std::string sex="",std::string age="",std::string tel="",std::string school="");
	bool mysql_updata(std::string oldname="",std::string newname="",std::string oldsex="",std::string newsex="",\
			std::string oldage="",std::string newage="",std::string oldtel="",std::string newtel="",\
			std::string oldschool="",std::string newschool="");
	bool mysql_close_sql();
	~mysql_api();
private:
	bool _sql_op(std::string &sql_op);
	bool _select_data();
private:
	MYSQL mysql;
	std::string host;
	std::string user;
	std::string passwd;
	std::string db;
	unsigned int port;
	unsigned long client_flag;
};
