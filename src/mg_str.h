#ifndef MG_STR_H
#define MG_STR_H

#define MG_STR_STR "str"

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
#include "mg_string.h"

#ifdef __cplusplus
}
#endif

typedef struct mg_str mg_str;

void mg_open_mg_str (lua_State *L);
int _mg_str_new (lua_State *L);
int _mg_str_newt (lua_State *L);
struct mg_str *check_mg_str(lua_State *L, int pos);

#endif
