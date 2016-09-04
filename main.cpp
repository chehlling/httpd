#include "httpd.h"
//#include "threadpool.h"

void* thread_run(void *arg)
{
	int sock=(int)arg;
	accept_request(sock);
	printf("one accept finshed...\n");
}

int main(int argc,char *argv[])
{
	if(argc!=3){
		usage(argv[0]);
		return 1;
	}

	int port=atoi(argv[2]);
	char *ip=argv[1];
//	threadpool<buf_t>* pool=new threadpool<buf_t>[_MAX_FD_NUM_];//_thread_number=8,int _max_requests=100
	
	int listen_sock = bind_sock(port,ip);

#ifdef _V1_
	struct sockaddr_in client;
	socklen_t client_len=sizeof(client);

	int done=0;
	while(!done){
		int sock=accept(listen_sock,(struct sockaddr*)&client,&client_len);
		if(sock<0){
			print_log(errno,__FUNCTION__,__LINE__);
			return 1;
		}
		printf("get a new connection:%s,%d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
		pthread_t tid;
		if(pthread_create(&tid,NULL,thread_run,(void *)sock)<0){
			print_log(errno,__FUNCTION__,__LINE__);
			return 1;
		}
		if(pthread_detach(tid)<0){
			print_log(errno,__FUNCTION__,__LINE__);
			return 1;
		}
	}
#elif _V2_
	//客户端数据：客户端socket、从客户端读到的数据、待写到客户端的数据和文件描述符、通信管道
	//buf_p user=(buf_p)malloc(sizeof(buf_t)*_MAX_FD_NUM_);
	buf_p user=new buf_t[_MAX_FD_NUM_];
	if(user==NULL){
		return -1;
	}
	int i=0;
	for(i=0;i<_MAX_FD_NUM_;++i){
		user[i].sock=-1;
		user[i].fd=-1;
		user[i].fd_size=-1;
		user[i].pipefd=-1;
		user[i].ptr[0]='\0';
	}

	int user_count=0;

	//创建epoll描述符
	int epoll_fd=epoll_create(256);
	if(epoll_fd<0){
		print_log(errno,__FUNCTION__,__LINE__);
		return 2;
	}

	//尽管分配了足够多的client对象即buf_t,但为了提高性能仍然需要限制用户数量
	struct epoll_event ev_set[_MAX_FD_NUM_];//存放所有被监视文件描述符的数组
	i=0;
	for(;i<_MAX_FD_NUM_;++i){
		ev_set[i].events=0;
		ev_set[i].data.fd=-1;
	}

	//listen_sock上是不能注册EPOLLONESHOT事件的，否则只会处理一个连接请求
	struct epoll_event ev;
	ev.events=EPOLLIN|EPOLLET;
	ev.data.fd=listen_sock;
	//添加监听描述符事件
	if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,listen_sock,&ev)<0){
		print_log(errno,__FUNCTION__,__LINE__);
		return 2;
	}

	int timeout=-1;

	char buf[1024];
	int ready_num=-1;
	int done=0;
	while(!done){
		//等待事件的发生
		if((ready_num=epoll_wait(epoll_fd,ev_set,_MAX_FD_NUM_,timeout))<0){
			print_log(errno,__FUNCTION__,__LINE__);
			return 2;
		}else if(ready_num==0){
			printf("epoll timeout...\n");
		}else{
			int i;
			for(i=0;i<ready_num;++i){
				//printf("EPOLLIN=%d,ready_num=%d\n",EPOLLIN,ready_num);
				if(ev_set[i].data.fd==listen_sock && (ev_set[i].events&EPOLLIN)){
					struct sockaddr_in client;
					socklen_t len=sizeof(client);
					//连接请求
					int sock=accept(listen_sock,(struct sockaddr*)&client,&len);
					if(sock<0){
						print_log(errno,__FUNCTION__,__LINE__);
						continue;
					}
					//too many user
					if(user_count >= _USER_LIMIT_){
						error_html(sock,500,"Internal server busy");
						close(sock);
						continue;
					}
					//设置套接字为非阻塞
					if(set_non_block(sock)<0){
						print_log(errno,__FUNCTION__,__LINE__);
						close(sock);
						continue;
					}
					//添加套接字到epoll_fd
					add_fd(epoll_fd,sock);
					user[sock].sock=sock;
					++user_count;
				}else if(ev_set[i].events&(EPOLLIN|EPOLLET)){
					//调用线程池中的线程进行处理
					//pool->append();
					//pool->work();
					
					int sock=ev_set[i].data.fd;
					
					readdata(sock,user);
					//修改为可写事件
					mod_fd(epoll_fd,sock,user);
				}else if((ev_set[i].events&(EPOLLOUT)|EPOLLET)){
					int sock=((buf_p)(ev_set[i].data.ptr))->sock;
					writedata(sock,user);//非阻塞写操作
					
					//直接关闭，没有判断是否是Keep-alive
					remove_fd(epoll_fd,sock);
					--user_count;
					close(sock);
				}
			}
		}
	}
	free(user);
#else
	printf("no more version...\n");

#endif
	
	return 0;
}
	
