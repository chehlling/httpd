#include "sql_api.h"

void get_args(std::vector<std::string> &vec,std::string &op)
{
	std::string method=getenv("METHOD");//getenv return NULL to string is error
	//std::cout<<method<<std::endl;
	std::string query_string; 
	std::string content_length;
	char ch='a';
	int index=0;
	int ret=-1;
	char buf[_SIZE_];
	memset(buf,'\0',sizeof(buf));
	if(method=="POST"){//POST
		std::string content_length=getenv("CONTENT_LENGTH");
		int len=atoi(content_length.c_str());
		memset(buf,'\0',sizeof(buf));
		while(len){//&& index<sizeof(buf)-1){
			ret=read(0,&ch,1);
			buf[index++]=ch;
			len--;
		}
		if(len!=0){
			printf("error!\n");
			return;
		}
	}else{//GET
		query_string=getenv("QUERY_STRING");
		//std::cout<<query_string<<std::endl;
		strcpy(buf,query_string.c_str());
		index=query_string.size();
	}
	buf[index]='\0';
	//std::cout<<buf<<std::endl;
	if(strncmp(buf,"name",4)==0){
		op="insert";
	}else if(strncmp(buf,"select_name",11)==0){
		op="select";
	}else if(strncmp(buf,"delete_name",11)==0){
		op="delete";
	}else if(strncmp(buf,"updata_name",11)==0){
		op="updata";
	}
	//std::cout<<op<<std::endl;
	char *end=buf+index;
	int incount=0;
	while(end!=buf){
		if(*end == '='){
			vec.push_back(end+1);
		}
		if(*end == '&'){
			*end='\0';
		}
		end--;
	}
}

void insert_echo_html()
{
	std::cout<<"HTTP/1.1 200 OK\r\n\r\n";
	std::cout<<"<html>"<<std::endl;
	std::cout<<"<head>"<<std::endl;
	std::cout<<"<h1>insert success!</h1>"<<std::endl;
	std::cout<<"</head>"<<std::endl;
	std::cout<<"</html>"<<std::endl;
}

void delete_echo_html()
{
	std::cout<<"HTTP/1.1 200 OK\r\n\r\n";
	std::cout<<"<html>"<<std::endl;
	std::cout<<"<head>"<<std::endl;
	std::cout<<"<h1>delete success!</h1>"<<std::endl;
	std::cout<<"</head>"<<std::endl;
	std::cout<<"</html>"<<std::endl;
}
void updata_echo_html()
{
	std::cout<<"HTTP/1.1 200 OK\r\n\r\n";
	std::cout<<"<html>"<<std::endl;
	std::cout<<"<head>"<<std::endl;
	std::cout<<"<h1>updata success!</h1>"<<std::endl;
	std::cout<<"</head>"<<std::endl;
	std::cout<<"</html>"<<std::endl;
}

void failed_echo_html()
{
	std::cout<<"HTTP/1.1 200 OK\r\n\r\n";
	std::cout<<"<html>"<<std::endl;
	std::cout<<"<head>"<<std::endl;
	std::cout<<"<h1>operator failed!Info Is Not Exit or Your Can Check Your Args...</h1>"<<std::endl;
	std::cout<<"</head>"<<std::endl;
	std::cout<<"</html>"<<std::endl;
}
int main()
{
	std::vector<std::string> vec;
	std::string op;
	
	get_args(vec,op);
	
	int vec_size=vec.size();
	const char *name=vec[vec_size-1].c_str();
	const char *sex=vec[vec_size-2].c_str();
	const char *age=vec[vec_size-3].c_str();
	const char *tel=vec[vec_size-4].c_str();
	const char *school=vec[vec_size-5].c_str();
	const char *id=vec[vec_size-6].c_str();

//	std::cout<<name<<std::endl;
//	std::cout<<sex<<std::endl;
//	std::cout<<age<<std::endl;
//	std::cout<<tel<<std::endl;
//	std::cout<<school<<std::endl;
//	std::cout<<id<<std::endl;
	
	mysql_api sql;
	//连接到MYSQL服务器
	sql.mysql_connect_sql();
	
	bool ret=false;
	if(op=="insert"){
		ret=sql.mysql_insert(name,sex,age,tel,school);
		if(ret==true){
			ret=false;
			insert_echo_html();
		}else{
			failed_echo_html();
		}
	}else if(op=="select"){
		ret=sql.mysql_select(id,name,sex,age,tel,school);
		if(ret==true){
			ret=false;
		}else{
			failed_echo_html();
		}
	}else if(op=="delete"){
//		std::cout<<op<<std::endl;
		ret=sql.mysql_delete(id,name,sex,age,tel,school);
		if(ret==true){
			ret=false;
			delete_echo_html();
		}else{
			failed_echo_html();
		}
	}else if(op=="updata"){
		const char *oldname=vec[vec_size-1].c_str();
		const char *newname=vec[vec_size-2].c_str();
		const char *oldsex=vec[vec_size-3].c_str();
		const char *newsex=vec[vec_size-4].c_str();
		const char *oldage=vec[vec_size-5].c_str();
		const char *newage=vec[vec_size-6].c_str();
		const char *oldtel=vec[vec_size-7].c_str();
		const char *newtel=vec[vec_size-8].c_str();
		const char *oldschool=vec[vec_size-9].c_str();
		const char *newschool=vec[vec_size-10].c_str();

		ret=sql.mysql_updata(oldname,newname,oldsex,newsex,oldage,newage,oldtel,newtel,oldschool,newschool);
		if(ret==true){
			ret=false;
			updata_echo_html();
		}else{
			failed_echo_html();
		}
	}

	sql.mysql_close_sql();
	
	return 0;
}
