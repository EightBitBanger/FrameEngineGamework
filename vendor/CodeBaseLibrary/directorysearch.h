#ifndef _DIRECTORY_SEARCH__
#define _DIRECTORY_SEARCH__

#include <vector>
#include <string>

__declspec(dllexport) std::vector<std::string> DirectoryGetList(std::string path);

#endif
