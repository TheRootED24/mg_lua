#ifndef MG_WS_MESSAGE_H
#define MG_WS_MESSAGE_H

#define MG_WS_MESSAGE "mg_ws_message"

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
#include "mg_ws_message.h"

#ifdef __cplusplus
}
#endif

typedef struct mg_ws_message ws_message;

void mg_open_mg_ws_message(lua_State *L);

#endif
