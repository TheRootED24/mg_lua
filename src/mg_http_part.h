#ifndef MG_HTTP_SERV_OPTS_H
#define MG_HTTP_SERV_OPTS_H

#define MG_HTTP_SERV_OPTS "serve_opts"
#include "mg_http_header.h"

typedef struct mg_http_part http_part;

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
void mg_open_mg_http_part(lua_State *L);
int newpart (lua_State *L);
http_part *checkpart(lua_State *L);
#endif
