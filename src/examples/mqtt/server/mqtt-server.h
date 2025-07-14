#ifndef MQTT_SERV_H
#define MQTT_SERV_H

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
#include "../../../includes/libmongoose/src/mongoose.h"
#include "../../mg_mqtt_message.h"
#include "../../mg_lua.h"
#include "../../mg_str.h"
#include "../../mg_mqtt.h"


#ifdef __cplusplus
}
#endif


// dump lua stack function for debugging
static void dumpstack (lua_State *L);

// LUA FACING FUNCTIONS


#endif /* MQTT_SERV_H */