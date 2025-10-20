#ifndef MG_FD_H
#define MG_FD_H

#define MG_FD "fd"

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

typedef struct mg_fd mg_fd;

void mg_open_mg_fd (lua_State *L);
mg_fd *check_mg_fd(lua_State *L, int pos);

#endif
