#ifndef _DIRECTORY_SEARCH__
#define _DIRECTORY_SEARCH__

#include <dirent.h>
#include <vector>
#include <string>
#include <iostream>

__declspec(dllexport) std::vector<std::string> DirectoryGetList(std::string path);

#endif
