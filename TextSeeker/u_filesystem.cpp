#include "pch.hpp"

bool fs::file_exists(const std::wstring_view& file)
{
	return _fs::exists(file);
}
std::wstring fs::full_path_to_name(const std::wstring_view& path)
{
	const auto p = path.find_last_of('\\');

	return std::wstring(path.substr(p + 1));
}
std::wstring fs::file_extension(const std::wstring_view& file)
{
	auto ext = file.find_last_of('.');

	if (ext == std::wstring::npos)
		return L"(no extension)";

	return std::wstring(file.substr(ext + 1));
}
bool fs::files_in_directory(const std::wstring& directory, std::vector<sFile>& out)
{
	if (!file_exists(directory))
		return false;

	sFile file;

	for (const auto& entry : _fs::directory_iterator(directory)) {
		file.path = entry.path().wstring();
		file.name = entry.path().filename();
		file.is_directory = entry.is_directory();
		out.push_back(file);
	}

	return true;
}
std::wstring get_last_error()
{
	const DWORD errorMessageID = ::GetLastError();
	WCHAR* messageBuffer = nullptr;

	size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (WCHAR*)&messageBuffer, 0, NULL);

	std::wstring output = std::wstring(messageBuffer, size);

	LocalFree(messageBuffer);
	return output;
}