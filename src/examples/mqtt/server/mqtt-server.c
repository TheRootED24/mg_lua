#include "mqtt-server.h"
#include "../../mg_mqtt_message.c"


// A list of subscription, held in memory

#define MQTT "mqtt"

struct sub {
	struct sub *next;
	struct mg_connection *c;
	struct mg_str topic;
	uint8_t qos;
};

static struct sub *s_subs = NULL;

int new_sub (lua_State *L) {
	struct sub *sub;
	int nargs = lua_gettop(L);

	if(nargs > 0)
		sub = (struct sub*)lua_touserdata(L, 1);
	else
		sub = (struct sub*)lua_newuserdata(L, sizeof(struct sub));

	luaL_getmetatable(L, "LuaBook.mg_connection");
	lua_setmetatable(L, -2);
	if(!sub) lua_pushnil(L);

	return 1;  /* new userdatum is already on the stack */
}


struct sub *checksub(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, "LuaBook.sub");
	luaL_argcheck(L, ud != NULL, 1, "`struct sub' expected");

	return (struct sub*)ud;
}

static int _sub_next(lua_State *L) {
	struct sub *sub = checksub(L);
	if(sub->next != NULL)
		lua_pushlightuserdata(L, sub->next);
	else
		lua_pushnil(L);

	return 1;
}

static int _sub_connection(lua_State *L) {
	struct sub *sub = checksub(L);
	if(sub->c != NULL)
		lua_pushlightuserdata(L, sub->c);
	else
		lua_pushnil(L);

	return 1;
}

static int _sub_topic(lua_State *L) {
	int nargs = lua_gettop(L);
	struct sub *sub = checksub(L);
	if(nargs > 1)
		sub->topic = mg_str(luaL_checkstring(L, 2));

	lua_pushlstring(L, sub->topic.buf, sub->topic.len);

	return 1;
}

static int _sub_qos(lua_State *L) {
	int nargs = lua_gettop(L);
	struct sub *sub = checksub(L);
	if(nargs > 1)
		sub->qos = (uint8_t)luaL_checkinteger(L, 2);

	lua_pushinteger(L, sub->qos);

	return 1;
}

static size_t mg_mqtt_next_topic(struct mg_mqtt_message *msg, struct mg_str *topic, uint8_t *qos, size_t pos) {
	unsigned char *buf = (unsigned char *) msg->dgram.buf + pos;
	size_t new_pos;
	if (pos >= msg->dgram.len) return 0;

	topic->len = (size_t) (((unsigned) buf[0]) << 8 | buf[1]);
	topic->buf = (char *) buf + 2;
	new_pos = pos + 2 + topic->len + (qos == NULL ? 0 : 1);
	if ((size_t) new_pos > msg->dgram.len) return 0;
	if (qos != NULL) *qos = buf[2 + topic->len];
	return new_pos;
}

size_t mg_mqtt_next_sub(struct mg_mqtt_message *msg, struct mg_str *topic, uint8_t *qos, size_t pos) {
	uint8_t tmp;

	return mg_mqtt_next_topic(msg, topic, qos == NULL ? &tmp : qos, pos);
}

size_t mg_mqtt_next_unsub(struct mg_mqtt_message *msg, struct mg_str *topic, size_t pos) {
	return mg_mqtt_next_topic(msg, topic, NULL, pos);
}

static int _mqtt_next_sub(lua_State *L) {
	mqtt_message *mm = check_mqtt_message(L);
	struct mg_str topic = mg_str(luaL_checkstring(L, 2));
	uint8_t qos = (uint8_t)luaL_checkinteger(L, 3);
	size_t pos = (size_t)luaL_checkinteger(L, 4);

	size_t ret = mg_mqtt_next_sub(mm, &topic, &qos, pos);
	lua_pushinteger(L, ret);

	return 1;
}

static int _mqtt_next_unsub(lua_State *L) {
	mqtt_message *mm = check_mqtt_message(L);
	struct mg_str topic = mg_str(luaL_checkstring(L, 2));
	size_t pos = (size_t)luaL_checkinteger(L, 3);

	size_t ret = mg_mqtt_next_unsub(mm, &topic, pos);
	lua_pushinteger(L, ret);

	return 1;
}

static int _LIST_ADD_HEAD(lua_State *L) {
	struct sub *sub = (struct sub *)lua_topointer(L, 1);

	LIST_ADD_HEAD(struct sub, &s_subs, sub);

	return 0;
}

static int _LIST_ADD_TAIL(lua_State *L) {
	struct sub *sub = (struct sub *)lua_topointer(L, 1);

	LIST_ADD_TAIL(struct sub, &s_subs, sub);

	return 0;
}

static int _LIST_DELETE(lua_State *L) {
	struct sub *sub = (struct sub *)lua_topointer(L, 1);

	LIST_DELETE(struct sub, &s_subs, sub);

	return 0;
}


// GAURDS FOR g++ to prevent name mangling during linking
#ifdef __cplusplus
extern "C" {
#endif

static const luaL_reg mqtt_serv_methods[] = {
{ "new",		new_sub		},
{ "next",		_sub_next	},
{ "c",			_sub_connection	},
{ "topic",		_sub_topic	},
{ "qos",		_sub_qos	},
{ "list_add_head",	_LIST_ADD_HEAD	},
{ "list_add_tail",	_LIST_ADD_TAIL	},
{ "list_del",		_LIST_DELETE	},
{ "next_sub",		_mqtt_next_sub	},
{ "next_unsub",		_mqtt_next_unsub},
{ }
};

int luaopen_mqtt(lua_State *L)
{
	luaL_register(L, MQTT, mqtt_serv_methods);

	return 1;
}

#ifdef __cplusplus
}
#endif