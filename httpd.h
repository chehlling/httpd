#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <sys/epoll.h>

#define _USER_LIMIT_ 6 ////最大用户数量
#define _SIZE_ 1024 //读缓冲区的大小
#define _MAX_FD_NUM_ 64 //文件描述符数量限制
#define _DEF_HTML_ "index.html" //网站根目录

//定义HTTP响应的一些状态信息
#define ok_200_title "OK"
#define error_400_title "Bad Request"
#define error_400_from "Your request has bad syntax or is inherently impossible to satisfy.\n"
#define error_403_title "Forbidden"
#define error_403_from "You do not have permission to get file from this server.\n"
#define error_404_title "Not Found"
#define error_404_from "The Requested file was not found on this server.\n"
#define error_500_title "Interbal Error"
#define error_500_from "There was an unusual probleem serving the requested filr.\n"

//客户端数据：客户端socket、从客户端读到的数据、待写到客户端的数据和文件描述符、通信管道
typedef struct buf_ptr
{
	int sock;//用于通信的描述符
	int fd;//客户端请求的文件的描述符
	int fd_size;//客户端请求的文件的大小
	int pipefd;//管道描述符
	char ptr[4096];//存放接收到的内容  和  要发送给客户端的内容
}buf_t,*buf_p;

void usage(const char *proc);
void print_log(int err,const char *func,int line);

int set_non_block(int fd);
int bind_sock(int _port,const char *ip);

void add_fd(int epoll_fd,int sock);
void mod_fd(int epoll_fd,int sock,buf_p user);
void remove_fd(int epoll_fd,int sock);

void readdata(int sock,buf_p user);
void writedata(int sock,buf_p user); 

buf_p accept_request(int sock);

void echo_html(int sock,int fd,off_t size);
void error_html(int sock,int status,const char* statusmessage);
