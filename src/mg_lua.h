#ifndef MG_H
#define MG_H

#define MG "mg_lua"

#include "mg_addr.h"
#include "mg_dns.h"
#include "mg_iobuf.h"
#include "mg_mgr.h"
#include "mg_http_header.h"
#include "mg_http_message.h"
#include "mg_connection.h"
#include "mg_http.h"
#include "mg_ws.h"
#include "mg_sntp.h"
#include "mg_mqtt.h"
#include "mg_tls.h"
#include "mg_timer.h"
#include "mg_time.h"
#include "mg_json.h"
#include "mg_rpc.h"
#include "mg_utility.h"
#include "mg_string.h"

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

typedef struct {
	lua_State *L;
	const char *callback;
} lua_State_t;

void fn_serv(mg_connection *c, int ev, void *ev_data);

typedef struct mg_mgr mg_mgr;
typedef struct mg_addr mg_addr;
typedef struct mg_connection mg_connection;
typedef struct mg_http_message mg_http_message;

#endif
