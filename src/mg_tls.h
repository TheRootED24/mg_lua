#ifndef MG_TLS_H
#define MG_TLS_H

#define MGTLS "mg_tls"

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
#include "mg_tls_opts.h"
#include "mg_connection.h"

#ifdef __cplusplus
}
#endif

typedef struct mg_tls mg_tls;

void  mg_open_mg_tls (lua_State *L);


#endif