#ifndef MG_HTTP_HEADER_H
#define MG_HTTP_HEADER_H

#define MG_HTTP_HEADER "header"

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

#ifdef __cplusplus
}
#endif

typedef struct mg_http_header http_header;

void mg_open_mg_http_header(lua_State *L);
int new_mg_http_header (lua_State *L);
http_header *check_mg_http_header(lua_State *L, int pos);

#endif
