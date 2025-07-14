#ifndef MG_HTTP_H
#define MG_HTTP_H

#define MG_HTTP "http"

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
#include "mg_connection.h"
#include "mg_http_header.h"
#include "mg_http_message.h"
#include "mg_http_serve_opts.h"
#include "mg_http_part.h"
#include "mg_mgr.h"
#include "mg_lua_cb.h"

#ifdef __cplusplus
}
#endif

void mg_open_mg_http(lua_State *L);

#endif
