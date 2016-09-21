#pragma once
#include "slices.h"

bool readFile(const char* name, int max_size, std::string* const contents);
std::set<int> getSocketInodesInCurrentDir();
void collectSlice(Slice* const s);
