math_cgi:math_cgi.cpp bigdata.cpp
	g++ -o $@ $^ -lstdc++

.PHONY:clean
clean:
	rm -rf math_cgi

.PHONY:output
output:math_cgi
	cp -f math_cgi ../../output/htdocs/cgi_bin/ 

