#include "httpd.h"


void usage(const char *proc)
{
	printf("usage:%s [ip] [port]\n",proc);
}

void print_log(int err,const char *func,int line)
{
	FILE* fd=fopen("./log/error_report","w");
	fprintf(fd,"%s****** %d :%s\n",func,line,strerror(err));
	fclose(fd);
}

int set_non_block(int fd)
{
	int OLD_FL=fcntl(fd,F_GETFL);
	if(OLD_FL<0){
		perror("fcntl");
		return -1;
	}
	if(fcntl(fd,F_SETFL,OLD_FL|O_NONBLOCK)<0){
		perror("fcntl");
		return -2;
	}
	return 0;
}

int bind_sock(int _port,const char *ip)
{
	int listen_sock=socket(AF_INET,SOCK_STREAM,0);
	if(listen_sock<0){
		print_log(errno,__FUNCTION__,__LINE__);
		return 2;
	}

	//避免进入TIME_WAIT状态，仅用于调试，实际运用中应去掉
	int opt=1;
	setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	struct sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_port=htons(_port);
	local.sin_addr.s_addr=inet_addr(ip);
	socklen_t len=sizeof(local);
	if(bind(listen_sock,(struct sockaddr*)&local,len)<0){
		print_log(errno,__FUNCTION__,__LINE__);
		return 2;
	}
	if(listen(listen_sock,_USER_LIMIT_)<0){
		print_log(errno,__FUNCTION__,__LINE__);
		return 2;
	}
	return listen_sock;
}

void add_fd(int epoll_fd,int sock)
{
	struct epoll_event ev;
	ev.data.fd=sock;
	ev.events=EPOLLIN|EPOLLET|EPOLLONESHOT;
	epoll_ctl(epoll_fd,EPOLL_CTL_ADD,sock,&ev);
	printf("get a new connection:%d...\n",sock);
}
//void mod_fd(int epoll_fd,int sock,buf_p ptr)
void mod_fd(int epoll_fd,int sock,buf_p user)
{
	struct epoll_event ev;
	ev.events=EPOLLOUT|EPOLLET|EPOLLONESHOT;//修改为可写事件
	//ev.data.ptr=(void*)ptr;//can't operator to ev_set[]
	ev.data.ptr=(void*)(&user[sock]);
	epoll_ctl(epoll_fd,EPOLL_CTL_MOD,sock,&ev);
}

void remove_fd(int epoll_fd,int sock)
{
	epoll_ctl(epoll_fd,EPOLL_CTL_DEL,sock,NULL);
	close(sock);
}

void readdata(int sock,buf_p user)
{
	buf_p tmp_buf=accept_request(sock);
	if(tmp_buf==NULL)
		return;
	user[sock].fd=tmp_buf->fd;
	user[sock].fd_size=tmp_buf->fd_size;
	user[sock].pipefd=tmp_buf->pipefd;

	int index=0;
	int ret=0;
	if(user[sock].pipefd>0){
		while(ret=read(user[sock].pipefd,(user[sock].ptr)+index,sizeof(user[sock].ptr)-1-index)){//notes! there only use read  ,not recv 
			if(ret<0){
				print_log(errno,__FUNCTION__,__LINE__);
				return;
			}
			index+=ret;
		}
	}
	printf("user[sock].ptr=%s\n",user[sock].ptr);
}

void writedata(int sock,buf_p user) 
{
	int fd=user[sock].fd;
	int fd_size=user[sock].fd_size;
	int pipefd=user[sock].pipefd;
	char *buf=user[sock].ptr;
	if(fd<0){
		//error_html();
		return;
	}
	
	if(pipefd==-1){//cgi==0
		echo_html(sock,fd,fd_size);
		return;
	}else{//cgi==1
		send(sock,buf,strlen(buf),0);
		
		close(pipefd);
	}
	close(fd);
}

//读取一行
int get_line(int sock,char *buf,int size)
{
	assert(buf);
	int index=0;
	char ch='\0';
	while(index<size-1 && ch!='\n'){
		if(recv(sock,&ch,1,0)>0){//***************************************block
			if(ch=='\r'){
				if(recv(sock,&ch,1,MSG_PEEK)<0){
					print_log(errno,__FUNCTION__,__LINE__);
					return -1;
				}else{
					if(ch=='\n'){
						recv(sock,&ch,1,0);
					}
				}
			}
			buf[index++]=ch;
		}else{
			ch='\n';
		}
	}
	buf[index]='\0';
	printf("url************%s\n",buf);
	return index;
}

void handler_clear(int sock)
{
	char buf[_SIZE_];
	while(get_line(sock,buf,sizeof(buf)) && strcmp(buf,"\n")){
		//printf("%s",buf);
	}
}

void echo_html(int sock,int fd,off_t size)
{
//	printf("path=%s\n",path);
	const char *status_line="HTTP/1.1 200 OK\r\n\r\n";
	if(send(sock,status_line,strlen(status_line),0)<0){
		print_log(errno,__FUNCTION__,__LINE__);
		close(fd);
		return;
	}

	if(sendfile(sock,fd,NULL,size)<0){
		print_log(errno,__FUNCTION__,__LINE__);
	}
	printf("send file success!\n");
	close(fd);
}

char *itoa( int n, char *str)
{
	bool symbol=true;
	int i=0;
	int j=0;
	if(n<0){
		symbol=false;
		n*=(-1);
	}
	while(n){
		str[i++]=n%10;
		n/=10;
	}	
	
	if(symbol==false)
		str[i++]='-';
	str[i]='\0';
	int size=strlen(str);
	i=0;
	j=size-1;
	for(i;i<j;++i){
		std::swap(str[i],str[j]);
	}
	return str;
}

void error_html(int sock,int status,const char* statusmessage)
{
	char str[5];
	char *status_line="HTTP/1.1 ";
	strcat(status_line,itoa(status,str));
	strcat(status_line,statusmessage);

	if(send(sock,status_line,strlen(status_line),0)<0){
		print_log(errno,__FUNCTION__,__LINE__);
		close(sock);
		return;
	}
}

//cgi实际上就是服务器与外部应用程序的接口
int exe_cgi(int sock,const char *path,const char *query_string)
{
	printf("path=====%s\n",path);
	char buf[_SIZE_];
	int ret=-1;
	int content_len=-1;
	char *len=NULL;
	int firstfd[2];
	int secondfd[2];
	if(strcmp(query_string,"\0")==0){//POST
		if(setenv("METHOD","POST",1)<0){
			print_log(errno,__FUNCTION__,__LINE__);
			return -1;
		}
		printf("export METHOD success!\n");
		while(ret=get_line(sock,buf,sizeof(buf)) && strcmp(buf,"\n")!=0){
			if(strncasecmp(buf,"Content-Length: ",15)==0){//length is 15 include a space
				buf[ret-1]='\0';
				len=buf+15;
				content_len=atoi(len);
				if(setenv("CONTENT_LENGTH",len,1)<0){//set env_var
					print_log(errno,__FUNCTION__,__LINE__);
					return -1;
				}
				printf("export CONTENT_LENGTH success!\n");
			}
		}
		printf("POST success\n");
	}else{//GET
		if(setenv("METHOD","GET",1)<0){
			print_log(errno,__FUNCTION__,__LINE__);
			return -1;
		}
		printf("export METHOD success!\n");
		if(setenv("QUERY_STRING",query_string,1)<0){
			print_log(errno,__FUNCTION__,__LINE__);
			return -1;
		}
		printf("export QUERY_STRING success!\n");
		handler_clear(sock);
		printf("GET success\n");
	}

	if(pipe(firstfd)<0){
		print_log(errno,__FUNCTION__,__LINE__);
		return -1;
	}
	if(pipe(secondfd)<0){
		print_log(errno,__FUNCTION__,__LINE__);
		return -1;
	}
	pid_t id=fork();
	if(id<0){
		print_log(errno,__FUNCTION__,__LINE__);
		return -1;
	}
	else if(id==0){
		close(firstfd[1]);
		close(secondfd[0]);
		if(dup2(firstfd[0],0)<0){
			print_log(errno,__FUNCTION__,__LINE__);
			return -1;
		}
		if(dup2(secondfd[1],1)<0){
			print_log(errno,__FUNCTION__,__LINE__);
			return -1;
		}
		close(secondfd[1]);
		close(firstfd[0]);
		execl(path,NULL);
	}else{
		close(firstfd[0]);
		close(secondfd[1]);
		char ch='a';
		if(strcmp(query_string,"")==0){//POST
			while(content_len){
				recv(sock,&ch,1,0);
				write(firstfd[1],&ch,1);
				content_len--;
			}
		}
		int ret=-1;
#ifdef _V1_
		while(ret=read(secondfd[0],&ch,1)){//notes! there only use read  ,not recv 
			if(ret<0){
				print_log(errno,__FUNCTION__,__LINE__);
				return -1;
			}
		//	printf("ret=%d,ch=%c\n",ret,ch);
			send(sock,&ch,1,0);
		}
		
		close(sock);
		close(secondfd[0]);
		close(firstfd[1]);
		return -1;
#elif _V2_
		return secondfd[0];
#endif
	}
}

//METHOD  GET,POST,HEAD,PUT,DELETE,TRACE,OPTIONS,CONNECT,PATCH
buf_p accept_request(int sock)
{
	char buf[_SIZE_];//_SIZE_=1024
#ifdef _DEBUG_
	int ret=-1;
	while(ret=get_line(sock,buf,sizeof(buf))){
		printf("%d:%s",ret,buf);
		if(strcmp(buf,"\n")==0){
			break;
		}
	}
#else 
	//获取请求行并分析其方法、目标URL、以及版本号
	int ret=get_line(sock,buf,sizeof(buf));
	if(ret<0){
		//error_html(sock,400,error_400_from);
		print_log(errno,__FUNCTION__,__LINE__);
		return NULL;
	}
	char *method=buf;
	char *url=buf;
	char *query_string=buf;
	char *protocol=buf;

	char path[_SIZE_*2];
	memset(path,'\0',sizeof(path));
	int cgi=0;
	//if method is not both GET or POST,exit!
	if( strncasecmp(method,"GET",3)!=0 && strncasecmp(method,"POST",4)!=0 ){//忽略大小写比较
		printf("method error!\n");
		return NULL;
	}
	//method is POST,running with cgi
	if( strncasecmp(method,"POST",4)==0 ){
		method[4]='\0';
		url+=5;
		cgi=1;
	}else{
		method[3]='\0';
		url+=4;
	}
	query_string=url;
	while(*query_string!='?' && *query_string!=' '){
		query_string++;
	}
	if(*query_string=='?'){
		*query_string='\0';
		query_string++;
		protocol=query_string;
		while(*protocol!=' '){
			protocol++;
		}
		*protocol='\0';
		protocol++;
		cgi=1;
	}else{
		*query_string='\0';
		protocol=query_string+1;
	}
	char *tmp=protocol;
	while(*tmp!='\r' && *tmp!='\n'){
		tmp++;
	}
	*tmp='\0';

	if(strcmp(url,"/favicon.ico")==0)
		return NULL;
	if(strcmp(url,"/")==0){
		strcpy(path,"htdocs/");
		strcat(path,_DEF_HTML_);
	}else{
	//	strcpy(path,url+1);
		
		strcpy(path,"htdocs");
		strcat(path,url);
	}
	
	printf("method=%s\n",method);
	printf("path=%s\n",path);
	printf("query_string=%s\n",query_string);
	printf("protocol=%s\n",protocol);
	printf("cgi=%d\n",cgi);
	//########################################################
	//分析目标文件的属性,是否存在，对所有用户可见，且不是目录
	int fd=open(path,O_RDONLY);
	if(fd<0){//file not exist
		print_log(errno,__FUNCTION__,__LINE__);
		//error_html(sock,404,error_404_from);
		return NULL;
	}

	struct stat fbuf;
	if(fstat(fd,&fbuf)<0){
		print_log(errno,__FUNCTION__,__LINE__);
		error_html(sock,500,error_500_from);
		close(fd);
		return NULL;
	}

	
	if(S_IFDIR&(fbuf.st_mode)){//dir
		//printf("file is invalid!\n");
		error_html(sock,404,error_404_from);
		return NULL;
	}else if((S_IXUSR&(fbuf.st_mode)) || (S_IXGRP&(fbuf.st_mode)) || (S_IXOTH&(fbuf.st_mode))){//2binary
		cgi=1;
	}
	int size=fbuf.st_size;
	
	int pipefd=-1;
	if(cgi==0){//GET
		handler_clear(sock);
#ifdef _V1_
	    echo_html(sock,fd,size);
		close(sock);
#endif
	}else{//GET or POST
		pipefd=exe_cgi(sock,path,query_string);
	}

	//多路复用使用，多线程直接忽略返回值
	buf_p tmp_buf=(buf_p)malloc(sizeof(buf_t));
	tmp_buf->fd=fd;
	tmp_buf->fd_size=size;
	tmp_buf->pipefd=pipefd;
	return tmp_buf;
#endif
}
