all: report

report: main.cpp
	g++ -g -O0 -Wall -o report main.cpp -I../../ -L../../build/linux -lsysanal -lpcap

clean:
	rm report
