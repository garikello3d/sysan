LIB=libsysanal.a

SRCS=netstat.cpp

HDRS=netstat.h slices.h packets.h

OBJS=$(patsubst %.cpp,%.o,$(SRCS))

CFLAGS=-g -O0 -Wall -std=c++11


all: $(LIB)

$(LIB): $(OBJS)
	ar cr $@ $^

$(OBJS): %.o: %.cpp $(HDRS)
	g++ -c -o $@ $< $(CFLAGS)

clean:
	rm $(OBJS) $(LIB)
