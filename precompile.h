
#ifndef RAWSOUND_PRECOMPILE_HH
#define RAWSOUND_PRECOMPILE_HH

#define CROW_USE_BOOST
#define CROW_DISABLE_STATIC_DIR
#define CROW_ENABLE_COMPRESSION
#define CROW_ENABLE_SSL
#define CROW_STATIC_DIRECTORY "static/"

#include "crow_all.h"

#include <filesystem>
#include <format>
#include <iostream>
#include <string>
#include <string_view>

#endif // RAWSOUND_PRECOMPILE_HH
