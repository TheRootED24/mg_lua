#ifndef MG_UTILITY_H
#define MG_UTILITY_H

#include "mg_lua.h"
#include "mg_connection.h"
#include "mg_str.h"
#include "mg_addr.h"

#define MG_UTILITY "util"

typedef struct mg_md5_ctx mg_md5;
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

void mg_open_mg_util (lua_State *L);

#endif
