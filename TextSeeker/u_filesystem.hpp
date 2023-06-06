#ifndef filsym
#define filsym


#include "pch.hpp"

struct sFile
{
	std::wstring path;
	std::wstring name;
	bool is_directory = false;

};

namespace fs
{
	bool file_exists(const std::wstring_view& file);
	std::wstring full_path_to_name(const std::wstring_view& path);
	std::wstring file_extension(const std::wstring_view& file);
	bool files_in_directory(const std::wstring& directory, std::vector<sFile>& out);

}
std::wstring get_last_error();


#endif