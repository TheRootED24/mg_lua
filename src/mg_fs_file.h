#ifndef MG_FILE_H
#define MG_FILE_H

#define MG_FILE "file"

// C++ GAURDS FOR C LIBS
#ifndef __cplusplus
// LUA LIBS FOR gcc
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#endif

#ifdef __cplusplus
// LUA LIBS FOR g++
#include <lua.hpp>
extern "C" {
#endif
// MONGOOSE C LIB
#include "../includes/libmongoose/src/mongoose.h"
#include "mg_fs.h"

#ifdef __cplusplus
}
#endif

typedef struct mg_fs mg_fs;

void mg_open_mg_fs_file (lua_State *L);

#endif
