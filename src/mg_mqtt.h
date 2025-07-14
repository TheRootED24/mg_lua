#ifndef MG_MQTT_H
#define MG_MQTT_H

#include "mg_lua.h"
#include "mg_mqtt_opts.h"
#include "mg_mqtt_message.h"

#define MG_MQTT "mqtt"

typedef struct mg_mqtt_opts mqtt_opts;
typedef struct mg_mqtt_message mqtt_message;

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

void mg_open_mg_mqtt (lua_State *L);

#endif
