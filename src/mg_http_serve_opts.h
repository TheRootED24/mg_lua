#ifndef MG_HTTP_SERVE_OPTS_H
#define MG_HTTP_SERVE_OPTS_H

#define MGHTTPSERV_OPTS "serve_opts"

typedef struct mg_http_serve_opts http_serve_opts;
#include "mg_lua.h"

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

void mg_open_mg_http_serve_opts(lua_State *L);
int newopts (lua_State *L);
http_serve_opts *checkopts(lua_State *L);
#endif
