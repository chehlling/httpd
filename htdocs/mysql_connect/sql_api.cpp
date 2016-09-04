#include "sql_api.h"

mysql_api::mysql_api()
	:host("127.0.0.1")
	,user("root")
	,passwd("184.00")
	,db("9_students")
	,port(3306)
	,client_flag(0)
{
	//获得或初始化一个MYSQL结构
	mysql_init(&mysql);
}
void mysql_api::mysql_connect_sql()
{
	//连接到MYSQL服务器
	mysql_real_connect(&mysql,host.c_str(),user.c_str(),passwd.c_str(),db.c_str(),port,NULL,0);
}
bool mysql_api::mysql_select(std::string id,std::string name,std::string sex,std::string age,std::string tel,std::string school)
{
	//select 列名称 from 表名称 [查询条件];
	std::string sql_op="select * from students where ";
	if(name!=""){
		sql_op.append("name=\"");
		sql_op.append(name);
		sql_op.append("\"");
	}else if(sex!=""){
		sql_op.append("sex=\"");
		sql_op.append(sex);
		sql_op.append("\"");
	}else if(age!=""){
		sql_op.append("age=");
		sql_op.append(age);
	}else if(tel!=""){
		sql_op.append("tel=\"");
		sql_op.append(tel);
		sql_op.append("\"");
	}else if(school!=""){
		sql_op.append("school=\"");
		sql_op.append(school);
		sql_op.append("\"");
	}else if(id!=""){
		sql_op.append("id=");
		sql_op.append(id);
	}else{
		sql_op="select * from students";
	}
//	std::cout<<sql_op<<std::endl;
	if(_sql_op(sql_op)){
		bool ret=_select_data();
		//std::cout<<ret<<std::endl;
		return ret;
	}
}

bool mysql_api::mysql_insert(std::string name,std::string sex,std::string age,std::string tel=NULL,std::string school=NULL)
{
	//insert [into] 表名 [(列名1, 列名2, 列名3, ...)] values (值1, 值2, 值3, ...);
	//insert into students values(NULL, "王刚", "男", 20, "13811371377",NULL);
	std::string sql_op;
	sql_op="insert into students (name,sex,age,tel,school) values (\"";
	sql_op.append(name);
	sql_op.append("\",\"");
	sql_op.append(sex);
	sql_op.append("\",");
	sql_op.append(age);
	sql_op.append(",\"");
	sql_op.append(tel);
	sql_op.append("\",\"");
	sql_op.append(school);
	sql_op.append("\")");
//	std::cout<<sql_op<<std::endl;
	return _sql_op(sql_op);
}

bool mysql_api::mysql_delete(std::string id,std::string name,std::string sex,std::string age,std::string tel,std::string school)
{
	//delete from 表名称 where 删除条件
	//删除表中的所有数据: delete from students;
	std::string sql_op="delete from students where ";
	if(name!=""){
		sql_op.append("name=\"");
		sql_op.append(name);
		sql_op.append("\"");
	}else if(sex!=""){
		sql_op.append("sex=\"");
		sql_op.append(sex);
		sql_op.append("\"");
	}else if(age!=""){
		sql_op.append("age=");
		sql_op.append(age);
	}else if(tel!=""){
		sql_op.append("tel=\"");
		sql_op.append("\"");
		sql_op.append(tel);
	}else if(school!=""){
		sql_op.append("school=\"");
		sql_op.append(school);
		sql_op.append("\"");
	}else if(id!=""){
		sql_op.append("id=");
		sql_op.append(id);
	}
//	std::cout<<sql_op<<std::endl;
	if(sql_op=="delete from students where ")
		return false;
	return _sql_op(sql_op);
}

bool mysql_api::mysql_updata(std::string oldname,std::string newname,std::string oldsex,std::string newsex,\
			std::string oldage,std::string newage,std::string oldtel,std::string newtel,std::string oldschool,std::string newschool)
{
	//update 表名称 set 列名称=新值 where 更新条件;
	// update students set age=19,tel="18829342744" where tel="6703" and school="xpu";
	std::string sql_op;
	std::string where;
	bool ret=false;
	sql_op="update students set " ;
	where="where ";
	if(oldname!=""){
		where.append("name=\"");
		where.append(oldname);
		where.append("\"");
	}
	if(newname!=""){
		sql_op.append("name=\"");
		sql_op.append(newname);
		sql_op.append("\"");
	}
	if(oldsex!=""){
		if(where!="where ")
			where.append(" and ");
		where.append("sex=\"");
		where.append(oldsex);
		where.append("\"");
	}
	if(newsex!=""){
		sql_op.append("sex=\"");
		sql_op.append(newsex);
		sql_op.append("\"");
	}
	if(oldage!=""){
		if(where!="where ")
			where.append(" and ");
		where.append("age=");
		where.append(oldage);
	}
	if(newage!=""){
		sql_op.append("age=");
		sql_op.append(newage);
	}
	if(oldtel!=""){
		if(where!="where ")
			where.append(" and ");
		where.append("tel=\"");
		where.append(oldtel);
		where.append("\"");
	}
	if(newtel!=""){
		sql_op.append("tel=\"");
		sql_op.append(newtel);
		sql_op.append("\"");
	}
	if(oldschool!=""){
		if(where!="where ")
			where.append(" and ");
		where.append("school=\"");
		where.append(oldschool);
		where.append("\"");
	}
	if(newschool!=""){
		sql_op.append("school=\"");
		sql_op.append(newschool);
		sql_op.append("\"");
	}
	sql_op.append(" ");

	std::string res=sql_op+where;
//	std::cout<<res<<std::endl;
	return _sql_op(res);
}

bool mysql_api::mysql_close_sql()
{
	//关闭与MySQL服务器的连接
	mysql_close(&mysql);
}

mysql_api::~mysql_api()
{
}

bool mysql_api::_sql_op(std::string &sql_op)
{
//	std::cout<<sql_op<<std::endl;
	bool ret=false;
	if(mysql_query(&mysql,sql_op.c_str())==0){
		ret=true;
//		std::cout<<ret<<std::endl;
	}
	return ret;
}

bool mysql_api::_select_data()
{
	//MYSQL_RES *mysql_store_result(MYSQL *mysql)
	MYSQL_RES *ret=mysql_store_result(&mysql);
	MYSQL_ROW row;
	int num_fields=mysql_num_fields(ret);
	std::cout<<"HTTP/1.1 200 OK\r\n\r\n";
	std::cout<<"<html>"<<std::endl;
	std::cout<<"<head>"<<std::endl;
	std::cout<<"<h1>id	name	sex	  age	tel		school";
	std::cout<<"</h1>"<<std::endl;
//	int count=0;
	while ((row = mysql_fetch_row(ret)))
	{
//		++count;
	    unsigned long *lengths;
		lengths = mysql_fetch_lengths(ret);
		std::cout<<"<h2>";
		for(int i = 0; i < num_fields; i++)
	    {
			printf("[%.*s] ", (int)lengths[i], row[i] ? row[i] : "NULL");
		}
		std::cout<<"</h2>"<<std::endl;
		std::cout<<std::endl;
	}
	std::cout<<"</head>"<<std::endl;
	std::cout<<"</html>"<<std::endl;
	mysql_free_result(ret);
//	std::cout<<count<<std::endl;
	return true;
}
