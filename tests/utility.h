#include "proc_net_parser.h"
#include "collector.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

std::string dumpInterfaces(const Slice::Interfaces ifaces);
void enumerateAppsAndConns();
