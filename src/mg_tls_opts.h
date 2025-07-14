#ifndef MG_TLS_OPTS_H
#define MG_TLS_OPTS_H

#include "mg_tls.h"

#define MG_TLS_OPTS "mg_tls_opts"

typedef struct mg_tls_opts tls_opts;

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

void  mg_open_mg_tls_opts (lua_State *L);
int new_tls_opts (lua_State *L);
tls_opts *check_tls_opts(lua_State *L);

#endif