LIB=libsysanal.a

VPATH=../../

SRCS=file_slice_parser.cpp file_packet_parser.cpp collector.cpp netstat.cpp proc_net_parser.cpp sniffer.cpp

HDRS=netstat.h slices.h slice_db.h packets.h proc_net_parser.h collector.h sniffer.h worker.h

OBJS=$(patsubst %.cpp,%.o,$(SRCS))

CFLAGS=-g -Wall -O0 -Wall -std=c++11 -I../../


all: $(LIB)

$(LIB): $(OBJS)
	ar cr $@ $^

$(OBJS): %.o: %.cpp $(HDRS)
	g++ -c -o $@ $< $(CFLAGS)

clean:
	rm $(OBJS) $(LIB)
