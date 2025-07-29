#ifndef MG_MQTT_OPTS_H
#define MG_MQTT_OPTS_H

#include "mg_mqtt.h"

#define MG_MQTT_OPTS "mg_mqtt_opts"

typedef struct mg_mqtt_opts mqtt_opts;

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

void  mg_open_mg_mqtt_opts (lua_State *L);
int _mqtt_opts_new (lua_State *L);
mqtt_opts *check_mqtt_opts(lua_State *L, int pos);

#endif
