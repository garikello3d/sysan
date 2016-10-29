#include "file_slice_parser.h"
#include "slice_db.h"
#include <sstream>
#include <fcntl.h>
#include <unistd.h>

FileSliceParser::FileSliceParser(const std::string& file)
	: m_file(file)
{
}

bool FileSliceParser::readFileIntoStream(std::stringstream* const str) {
	bool ret = true;
	char buf[8192];
	int fd = open(m_file.c_str(), 0);
	if (fd < 0)
		return false;
	for (;;) {
		int rd = read(fd, buf, sizeof(buf));
		if (rd < 0) {
			ret = false;
			break;
		}
		else if (rd == 0)
			break;
		else
			str->write(buf, rd);
	}
	close(fd);
	return ret;
}

bool FileSliceParser::parse(Slices* const slices) {
	std::stringstream ss;
	
	if (!readFileIntoStream(&ss))
		return false;
	
	slices->clear();
	
	cereal::BinaryInputArchive iarchive(ss);

	try {
		for (;;) {
			uint64_t time;
			Slice slice;
			iarchive(time);
			iarchive(slice);
			slices->insert(std::make_pair(time, slice));
		}
	}
	catch (const cereal::Exception& e) {
		// nothing
		puts(e.what());
	}

	return true;
}
