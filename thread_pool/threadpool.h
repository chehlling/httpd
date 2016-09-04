#include "locker.h"
#include <iostream>
#include <vector>
#include <errno.h>
//池的概念就是以空间换时间
template<class T>//T是任务类
class threadpool
{
public:
	threadpool(int _threat_number=8,int _max_requests=100);
	~threadpool();
	bool append(T* request);//往请求队列中添加任务
private:
	static void* worker(void* arg);
	void run();
private:
	int thread_number;
	int max_requests;   
	pthread_t* threads;//描述线程池的数组，大小是thread_number

	std::vector<T*> workqueue;
	locker queuelocker;

	sem queuestat;

	bool stop;
};


template<class T>//T是任务类
threadpool<T>::threadpool(int _thread_number,int _max_requests)
	:thread_number(_thread_number)
	 ,max_requests(_max_requests)
{
	if(_thread_number<=0 || _max_requests<=0)
		std::cout<<"error"<<std::endl;
	
	threads=new pthread_t[thread_number];
	if(threads==NULL)
		std::cout<<"error"<<std::endl;
		//print_log(errno,__FUNCTION__,__LINE__);
	for(int i=0;i<thread_number;++i){
		// int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
		//                           void *(*start_routine) (void *), void *arg);
		int ret=pthread_create(threads+i,NULL,worker,this);//?????
		if(ret!=0){
			std::cout<<"error"<<std::endl;
		}
		pthread_detach(threads[i]);
	}
}

template<class T>//T是任务类
threadpool<T>::~threadpool()
{
	delete[] threads;
	stop=true;
}

template<class T>//T是任务类
bool threadpool<T>::append(T* request)//往请求队列中添加任务
{
	//操作工作队列时要加锁，因为它被所有线程所共享
	//lock
	queuelocker.lock();
	if(workqueue.size()>max_requests){
		//unlock()
		queuelocker.unlock();
		return false;
	}
	workqueue.push_back(request);
	//unlock()
	queuelocker.unlock();
	queuestat.post();
	return true;
}

template<class T>//T是任务类
void* threadpool<T>::worker(void* arg)//静态成员函数中不能用this,传参数
{
	threadpool* pool=(threadpool*)arg;
	pool->run();
	return pool;
}

template<class T>//T是任务类
void threadpool<T>::run()
{
	while(!stop){
		queuestat.wait();
		queuelocker.lock();
		if(workqueue.empty()){
			queuelocker.lock();
			continue;
		}
		T* request=workqueue.front();
		workqueue.pop_back();
		queuelocker.unlock();
		if(!request)
			continue;
//		request->process();
	}

}
