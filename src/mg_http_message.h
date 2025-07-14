#ifndef MG_HTTP_MESAGE_H
#define MG_HTTP_MESAGE_H

#include "mg_http_header.h"

#define MGHTTPMSG "mg_http_message"

typedef struct mg_http_message http_message;
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
void  mg_open_mg_http_message (lua_State *L);
int newmessage (lua_State *L);
http_message *checkmessage(lua_State *L);

#endif
