#pragma once
#include "slices.h"

class FileSliceParser : public SliceParser {
public:
	FileSliceParser(const std::string& file);
	virtual bool parse(Slices* const slices);
private:
	bool readFileIntoStream(std::stringstream* const str);
	const std::string m_file;
};
