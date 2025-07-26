#ifndef MG_WS_MESSAGE_H
#define MG_WS_MESSAGE_H

#define MG_WS_MESSAGE "mg_ws_message"

#include "mg_ws_message.h"
#include "mg_http_header.h"
#include "mg_http_message.h"
#include "mg_http_serve_opts.h"
#include "mg_http_part.h"
#include "mg_mgr.h"
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

typedef struct mg_ws_message ws_message;
void mg_open_mg_ws_message(lua_State *L);

#endif
