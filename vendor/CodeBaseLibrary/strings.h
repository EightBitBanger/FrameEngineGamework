#ifndef STRING_FUNCTIONS
#define STRING_FUNCTIONS

#include <sstream>

#include <string>
#include <vector>

/// Explode the string by the given delimiter into an array of strings.
__declspec(dllexport) std::vector<std::string> StringExplode(std::string String, const char Char);

/// Returns the file name from a path string.
__declspec(dllexport) std::string StringGetNameFromFilename(std::string FilePath);

/// Returns the file name without an extension from a path string.
__declspec(dllexport) std::string StringGetNameFromFilenameNoExt(std::string FilePath);

/// Returns the file extension only.
__declspec(dllexport) std::string StringGetExtFromFilename(std::string FilePath);

#endif
