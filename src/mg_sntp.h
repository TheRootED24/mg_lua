#ifndef MG_SNTP_H
#define MG_SNTP_H

#define MG_SNTP "sntp"

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
#include "mg_mgr.h"
#include "mg_lua_cb.h"

#ifdef __cplusplus
}
#endif

void mg_open_mg_sntp (lua_State *L);

#endif
