#ifndef MG_HTTP_PART_H
#define MG_HTTP_PART_H

#define MG_HTTP_PART "http_part"

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

#ifdef __cplusplus
}
#endif

typedef struct mg_http_part http_part;

void mg_open_mg_http_part(lua_State *L);
int new_mg_http_part (lua_State *L);
http_part *check_mg_http_part(lua_State *L, int pos);

#endif
