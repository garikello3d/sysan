TARGET=run_all

VPATH=../../../:../../
SRCS=../../q.cpp ../../parse.cpp ../../snif.cpp ../../gen1.cpp ../../proc.cpp ../../ser.cpp ../../workers.cpp ../../../utility.cpp

LIBDIR=../../../../build/linux
LIB=$(LIBDIR)/libsysanal.a

CFLAGS=-I../../../../ -I../../../ -std=c++11 -Wall
LDFLAGS=-pthread -lboost_unit_test_framework -L$(LIBDIR) -lsysanal -lpcap

all: $(TARGET)

$(TARGET): $(SRCS) $(LIB)
	g++ -g -O0 -o $@ $(SRCS) $(CFLAGS) $(LDFLAGS)
