TARGET=run_all

SRCS=gen1.cpp proc.cpp

LIB=../libsysanal.a

CFLAGS=-I../ -std=c++11
LDFLAGS=-lboost_unit_test_framework -L../ -lsysanal

all: $(TARGET)

$(TARGET): $(SRCS) $(LIB)
	g++ -g -O0 -o $@ $(SRCS) $(CFLAGS) $(LDFLAGS)
