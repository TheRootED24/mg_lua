#ifndef MG_IOBUF_H
#define MG_IOBUF_H

#define MG_IOBUF "mg_iobuf"

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

typedef struct mg_iobuf mg_iobuf;

void mg_open_mg_iobuf(lua_State *L);
int _mg_iobuf_new (lua_State *L);
mg_iobuf *check_mg_iobuf(lua_State *L, int pos);

#endif
