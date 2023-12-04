#ifndef STRING_FUNCTIONS
#define STRING_FUNCTIONS

#include <sstream>

#include <string>
#include <vector>

#include <GameEngineFramework/configuration.h>


/// Explode the string by the given delimiter into an array of strings.
ENGINE_API std::vector<std::string> StringExplode(std::string String, const char Char);

/// Returns the filename from a path string.
ENGINE_API std::string StringGetNameFromFilename(std::string FilePath);

/// Returns the filename without an extension from a path string.
ENGINE_API std::string StringGetNameFromFilenameNoExt(std::string FilePath);

/// Returns the extension of the file.
ENGINE_API std::string StringGetExtFromFilename(std::string FilePath);

/// Returns the path part of a filename.
ENGINE_API std::string StringGetPathFromFilename(std::string FilePath);

#endif
