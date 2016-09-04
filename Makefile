ROOT_PATH=$(shell pwd)
LDFLAGS=-lpthread
FLAGS=#-D_DEBUG_#-g

CC=g++
BIN=httpd
SRC=$(shell ls *.cpp)
OBJ=$(SRC:.cpp=.o)

CGI_BIN=htdocs/cgi_bin
MYSQL_CONNECT=htdocs/mysql_connect
THREAD_POOL=thread_pool/

.PHONY:all
all:$(BIN) cgi
$(BIN):$(OBJ)
	@echo "Linking [$^] to [$@]"
	@$(CC) -o $@ $^  $(LDFLAGS)
	@echo "Linking done..."
%.o:%.cpp
	@echo "Compling [$<] to [$@]"
	@$(CC) -c $< -D_V2_ $(FLAGS) 
	@echo "Compling done..."


.PHONY:cgi
cgi:
	@for name in `echo $(CGI_BIN)`;\
	do\
		cd $$name;\
		make;\
		cd -;\
	done
	@for name in `echo $(MYSQL_CONNECT)`;\
	do\
		cd $$name;\
		make;\
		cd -;\
	done

.PHONY:clean
clean:
	@rm -rf *.o $(BIN) output
	@for name in `echo $(CGI_BIN)`;\
	do\
		cd $$name;\
		make clean;\
		cd -;\
	done
	@for name in `echo $(MYSQL_CONNECT)`;\
	do\
		cd $$name;\
		make clean;\
		cd -;\
	done

.PHONY:output
output:$(BIN) cgi
	@mkdir -p output/log
	@cp -rf htdocs output/
	@mkdir -p output/htdocs/cgi_bin
	@cp -f httpd output/
	@cp -f start.sh output/
	@cp -rf conf output/
	@mkdir -p output/htdocs/cgi_mysql
	@for name in `echo $(CGI_BIN)`;\
	do\
		cd $$name;\
		make output;\
		cd -;\
	done
	@for name in `echo $(MYSQL_CONNECT)`;\
	do\
		cd $$name;\
		make output;\
		cd -;\
	done

