ROOT_PATH=$(shell pwd)#shell是函数，它的参数是操作系统shell的命令，它和反引号的功能是相同的，这就是说shell函数把执行操作系统命令后的输出作为函数返回值，这个函数会生成一个shell程序来执行命令，所以要注意运行性能

#LINK=-I/home/chehlling/httpd/mysql_connect/mysql-clib/include\
	 -L/home/chehlling/httpd/mysql_connect/mysql-clib/lib -lmysqlclient -lpthread -lm -lrt -ldl

sql_api:main.cpp sql_api.cpp
	g++ -o $@ $^ `${ROOT_PATH}/mysql-clib/bin/mysql_config --cflags --libs ` -lstdc++ 

.PHONY:clean
clean:
	rm -f sql_api

.PHONY:output
output:sql_api
	cp -f sql_api ../../output/htdocs/cgi_mysql/
