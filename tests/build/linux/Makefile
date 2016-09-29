TARGET=run_all

VPATH=../../
SRCS=../../gen1.cpp ../../proc.cpp

LIBDIR=../../../build/linux
LIB=$(LIBDIR)/libsysanal.a

CFLAGS=-I../../../ -std=c++11
LDFLAGS=-lboost_unit_test_framework -L$(LIBDIR) -lsysanal

all: $(TARGET)

$(TARGET): $(SRCS) $(LIB)
	g++ -g -O0 -o $@ $(SRCS) $(CFLAGS) $(LDFLAGS)
