#ifndef MG_TIMER_H
#define MG_TIMER_H

#include "mg_lua.h"

#define MGTIMER "mg_timer"

typedef struct mg_timer mg_timer;

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

void  mg_open_mg_timer (lua_State *L);
int new_mg_timer (lua_State *L);
mg_timer *check_mg_timer(lua_State *L);

#endif
