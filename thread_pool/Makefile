ROOT_PATH=$(shell pwd)
INCLUDE=$(ROOT_PATH)

.PHONY:all
all:threadpool locker
	g++ -o $@ $^

locker:locker.cpp
	g++ -o $@ $^ -I$(INCLUDE) 

threadpool:threadpool.cpp
	g++ -o $@ $^ -I$(INCLUDE) 

.PHONY:clean
clean:
	rm -rf threadpool threadpool.o locker.o 


.PHONY:debug
debug:
	@echo $(ROOT_PATH)
	@echo $(INCLUDE)
