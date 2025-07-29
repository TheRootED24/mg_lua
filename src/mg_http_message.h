#ifndef MG_HTTP_MESAGE_H
#define MG_HTTP_MESAGE_H

#define MG_HTTP_MSG "mg_http_message"

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
#include "mg_http_header.h"

#ifdef __cplusplus
}
#endif

typedef struct mg_http_message http_message;

void  mg_open_mg_http_message (lua_State *L);
int new_mg_http_message (lua_State *L);
http_message *check_mg_http_message(lua_State *L, int pos);

#endif
