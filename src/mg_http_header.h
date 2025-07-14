#ifndef MG_HTTP_HEADER_H
#define MG_HTTP_HEADER_H

#define MG_HTTP_HEADER "header"

typedef struct mg_http_header http_header;

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
void mg_open_mg_http_header(lua_State *L);
int newheader (lua_State *L);
http_header *checkheader(lua_State *L);
#endif
