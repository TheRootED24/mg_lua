#ifndef MG_DNS_H
#define MG_DNS_H

#include "mg_lua.h"
#include "mg_connection.h"

#define MG_DNS "mg_dns"

typedef struct mg_dns mg_dns;

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

void mg_open_mg_dns(lua_State *L);

#endif
