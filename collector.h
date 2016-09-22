#pragma once
#include "slices.h"

bool readFile(const std::string& name, /*int max_size,*/ std::string* const contents);
std::set<int> getSocketInodes(const std::string& fd_path);
bool getSystemInterfaces(Slice::Interfaces* const ifaces);
void collectSlice(Slice* const s);
