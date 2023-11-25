#ifndef STRING_FUNCTIONS
#define STRING_FUNCTIONS

#include <sstream>

#include <string>
#include <vector>

/// Explode the string by the given delimiter into an array of strings.
__declspec(dllexport) std::vector<std::string> StringExplode(std::string String, const char Char);

/// Returns the filename from a path string.
__declspec(dllexport) std::string StringGetNameFromFilename(std::string FilePath);

/// Returns the filename without an extension from a path string.
__declspec(dllexport) std::string StringGetNameFromFilenameNoExt(std::string FilePath);

/// Returns the extension of the file.
__declspec(dllexport) std::string StringGetExtFromFilename(std::string FilePath);

/// Returns the path part of a filename.
__declspec(dllexport) std::string StringGetPathFromFilename(std::string FilePath);

#endif
