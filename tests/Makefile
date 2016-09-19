TARGET=run_all

SRCS=gen1.cpp

LIB=../libsysanal.a

CFLAGS=-I../
LDFLAGS=-lboost_unit_test_framework -L../ -lsysanal

all: $(TARGET)

$(TARGET): $(SRCS) $(LIB)
	g++ -g -O0 -o $@ $(SRCS) $(CFLAGS) $(LDFLAGS)
