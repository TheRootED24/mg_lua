#ifndef MG_STRING_H
#define MG_STRING_H

#define MG_STRING "string"

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
#include "mg_str.h"
#include "mg_mgr.h"
#include "mg_iobuf.h"

#ifdef __cplusplus
}
#endif

void mg_open_mg_string(lua_State *L);

#endif
