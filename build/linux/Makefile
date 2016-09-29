LIB=libsysanal.a

VPATH=../../

SRCS=collector.cpp netstat.cpp proc_net_parser.cpp

HDRS=netstat.h slices.h packets.h proc_net_parser.h collector.h

OBJS=$(patsubst %.cpp,%.o,$(SRCS))

CFLAGS=-g -O0 -Wall -std=c++11


all: $(LIB)

$(LIB): $(OBJS)
	ar cr $@ $^

$(OBJS): %.o: %.cpp $(HDRS)
	g++ -c -o $@ $< $(CFLAGS)

clean:
	rm $(OBJS) $(LIB)
