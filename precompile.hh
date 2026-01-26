//
// Created by Reid Woodbury Jr on 1/25/26.
//

#ifndef DISKERROR_PRECOMPILE_H
#define DISKERROR_PRECOMPILE_H


#define CROW_USE_BOOST
#define CROW_DISABLE_STATIC_DIR
#define CROW_ENABLE_COMPRESSION
#define CROW_ENABLE_SSL
#define CROW_STATIC_DIRECTORY "static/"

#include "crow_all.h"

#include <iostream>
#include <string>
#include <filesystem>

#endif //DISKERROR_PRECOMPILE_H