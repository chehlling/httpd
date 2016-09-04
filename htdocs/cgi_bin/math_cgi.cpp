#include "bigdata.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <errno.h>
#include <vector>
#include <stdlib.h>
#include <string.h>
#define _SIZE_ 2048

void get_args(std::vector<std::string> &vec)
{
	std::string method=getenv("METHOD");
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
		strcpy(buf,query_string.c_str());
		index=query_string.size();
	}
	buf[index]='\0';
//	std::cout<<buf<<std::endl;
	int countin=0;
	char *end=buf+index;
	while(end!=buf){
		if(*end == '='){
			vec.push_back(end+1);
		//	std::cout<<vec[countin++]<<std::endl;
		}
		if(*end == '&'){
			*end='\0';
		}
		end--;
	}
}

void echo_result(std::vector<std::string> &vec)
{
//	char ch;
//	const char *op=vec[0].c_str();
//	BigData data1(vec[2].c_str());
//	BigData data2(vec[1].c_str());
//	BigData sum;
////	std::cout<<"data1:"<<data1<<std::endl;
////	std::cout<<"data2:"<<data2<<std::endl;
////	std::cout<<"sum:"<<sum<<std::endl;
////	std::cout<<"op:"<<op<<std::endl;
//	if( strcmp("ADD",op)==0 ){
//		sum=data1+data2;
//		ch='+';
//	}else if( strcmp("SUB",op)==0 ){
//		sum=data1-data2;
//		ch='-';
//	}else if( strcmp("MUL",op)==0 ){
//		sum=data1*data2;
//		ch='*';
//	}else if( strcmp("DIV",op)==0 ){
//		sum=data1/data2;
//		ch='/';
//	//}else if( strcmp("MOD",op)==0 ){
//	//	sum=data1%data2;
//	//	ch='%';
//	}else{
//		return;
//	}

	
	const char *op=vec[0].c_str();
	BigData data1(vec[2].c_str());
	BigData	data2(vec[1].c_str());
//	std::cout<<data1<<"     "<<data2<<std::endl;
	BigData sum;
	char ch;
	if( strcmp("ADD",op)==0 ){
		sum=data1+data2;
		ch='+';
	}else if( strcmp("SUB",op)==0 ){
		sum=data1-data2;
		ch='-';
	}else if( strcmp("MUL",op)==0 ){
		sum=data1*data2;
		ch='*';
	}else if( strcmp("DIV",op)==0 ){
		sum=data1/data2;
		ch='/';
	}else if( strcmp("MOD",op)==0 ){
		sum=data1%data2;
		ch='%';
	}else{
		return;
	}
	std::cout<<"HTTP/1.1 200 OK\r\n\r\n";
	std::cout<<"<html>"<<std::endl;
	std::cout<<"<head>"<<std::endl;
	std::cout<<"  ";
	std::cout<<"<h1>math_cgi     ";
	std::cout<<op <<"</h1>"<<std::endl;
	std::cout<<"</head>"<<std::endl;
	std::cout<<"<body>"<<std::endl;
	std::cout<<data1<<ch<<data2<<"="<<sum<<std::endl;
	std::cout<<"</body>"<<std::endl;
	std::cout<<"</html>"<<std::endl;
	
}
int main(int argc,char *argv[])
{
	std::vector<std::string> vec;
	get_args(vec);//reverse store args
	echo_result(vec);

	return 0;
}
