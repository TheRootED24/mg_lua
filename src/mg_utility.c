#include "mg_utility.h"

// void mg_call(struct mg_connection *c, int ev, void *ev_data);
static int _mg_call(lua_State *L)
{
	mg_connection *conn = checkconn(L);
	int ev = luaL_checkinteger(L, 2);
	void *ev_data = (void*)lua_topointer(L, 3);
	ev_data = ev_data ? ev_data : NULL;
	mg_call(conn, ev, ev_data);

	return 0;
}

// void mg_error(struct mg_connection *c, const char *fmt, ...);
static int _mg_error (lua_State *L)
{
	mg_connection *conn = checkconn(L);
	const char *fmt = luaL_checkstring(L, 2);
	const char *arg_str = luaL_checkstring(L, 3);
	mg_error(conn, fmt, arg_str);
	return 0;
}

/*  MD% CTX */
int new_mg_md5_ctx(lua_State *L)
{
	mg_md5_ctx *md5 = (mg_md5_ctx *)lua_newuserdata(L, sizeof(mg_md5_ctx));

	luaL_getmetatable(L, "LuaBook.mg_md5_ctx");
	lua_setmetatable(L, -2);
	if (!md5)
		lua_pushnil(L);

	return 1; /* new userdatum is already on the stack */
}

mg_md5_ctx *check_mg_md5_ctx(lua_State *L)
{
	void *ud = luaL_checkudata(L, 1, "LuaBook.mg_md5_ctx");
	luaL_argcheck(L, ud != NULL, 1, "`mg_md5_ctx' expected");
	return (mg_md5_ctx *)ud;
}

// void mg_md5_init(mg_md5_ctx *c);
static int _mg_md5_init(lua_State *L)
{
	mg_md5_ctx *md5 = check_mg_md5_ctx(L);
	mg_md5_init(md5);

	return 0;
}

// void mg_md5_update(mg_md5_ctx *c, const unsigned char *data, size_t len);
static int _mg_md5_update(lua_State *L)
{
	mg_md5_ctx *md5 = check_mg_md5_ctx(L);
	const unsigned char *data = (unsigned char *)luaL_checkstring(L, 2);
	size_t len = luaL_checkinteger(L, 3);
	mg_md5_update(md5, data, len);

	return 0;
}

// void mg_md5_final(mg_md5_ctx *c, unsigned char buf[16]);
static int _mg_md5_final(lua_State *L)
{
	mg_md5_ctx *md5 = check_mg_md5_ctx(L);
	unsigned char buf[16] = {0};
	mg_md5_final(md5, buf);

	return 0;
}

/*  SHA1 CTX */
int new_mg_sha1_ctx(lua_State *L)
{
	mg_sha1_ctx *sha1 = (mg_sha1_ctx *)lua_newuserdata(L, sizeof(mg_sha1_ctx));

	luaL_getmetatable(L, "LuaBook.mg_sha1_ctx");
	lua_setmetatable(L, -2);
	if (!sha1)
		lua_pushnil(L);

	return 1; /* new userdatum is already on the stack */
}

mg_sha1_ctx *check_mg_sha1_ctx(lua_State *L)
{
	void *ud = luaL_checkudata(L, 1, "LuaBook.mg_sha1_ctx");
	luaL_argcheck(L, ud != NULL, 1, "`mg_sha1_ctx' expected");
	return (mg_sha1_ctx *)ud;
}

// void mg_sha1_init(mg_sha1_ctx *c);
static int _mg_sha1_init(lua_State *L)
{
	mg_sha1_ctx *sha1 = check_mg_sha1_ctx(L);
	mg_sha1_init(sha1);

	return 0;
}

// void mg_sha1_update(mg_sha1_ctx *c, const unsigned char *data, size_t len);
static int _mg_sha1_update(lua_State *L)
{
	mg_sha1_ctx *sha1 = check_mg_sha1_ctx(L);
	const unsigned char *data = (unsigned char *)luaL_checkstring(L, 2);
	size_t len = luaL_checkinteger(L, 3);
	mg_sha1_update(sha1, data, len);

	return 0;
}

// void mg_sha1_final(unsigned char digest[20], mg_sha1_ctx *c);
static int _mg_sha1_final(lua_State *L)
{
	mg_sha1_ctx *sha1 = check_mg_sha1_ctx(L);
	unsigned char buf[20] = {0};
	mg_sha1_final(buf, sha1);

	return 0;
}

/* mg_base64 */
// size_t mg_base64_update(unsigned char p, char *buf, size_t len);
static int _mg_base64_update (lua_State *L)
{
	unsigned char p = luaL_checkint(L, 1);
	char *buf = (char*)luaL_checkstring(L, 2);
	size_t len = luaL_checklong(L, 3);
	size_t ret =  mg_base64_update(p, buf, len);
	lua_pushinteger(L, luaL_optlong(L, ret, -1));

	return 1;
}

// size_t mg_base64_final(char *buf, size_t len);
static int _mg_base64_final (lua_State *L)
{
	char *buf = (char*)luaL_checkstring(L, 1);
	size_t len = luaL_checklong(L, 2);
	size_t ret =  mg_base64_final(buf, len);
	lua_pushinteger(L, (size_t)luaL_optlong(L, ret, -1));

	return 1;
}

// size_t mg_base64_encode(const unsigned char *p, size_t n, char *buf, size_t len);
static int _mg_base64_encode (lua_State *L)
{
	const unsigned char *p = (const unsigned char*)luaL_checkstring(L, 1);
	size_t n = luaL_checklong(L, 2);
	char *buf = (char*)luaL_checkstring(L, 3);
	size_t len = luaL_checklong(L, 4);
	size_t ret =  mg_base64_encode(p, n, buf, len);
	lua_pushinteger(L, luaL_optlong(L, ret, -1));

	return 1;
}

// size_t mg_base64_decode(const char *src, size_t n, char *dst, size_t len);
static int _mg_base64_decode (lua_State *L)
{
	const char *src = luaL_checkstring(L, 1);
	size_t n = luaL_checklong(L, 2);
	char *dst = (char*)luaL_checkstring(L, 3);
	size_t len = luaL_checklong(L, 4);
	size_t ret =  mg_base64_decode(src, n, dst, len);
	lua_pushinteger(L, luaL_optlong(L, ret, -1));

	return 1;
}

/* mg_random */

// bool mg_random(void *buf, size_t len);
static int _mg_random(lua_State *L) {
	void *buf = (void*)lua_topointer(L, 1);
	size_t len = luaL_checklong(L, 2);
	bool ok = mg_random(buf, len);

	if(ok) lua_pushlstring(L, (const char*)buf, len);
		else lua_pushnil(L);

	return ok;
}

// char *mg_random_str(char *buf, size_t len);
static int _mg_random_str (lua_State *L) {
	char *buf = (char*)luaL_checkstring(L, 1);
	size_t len = luaL_checklong(L, 1);
	const char *res = mg_random_str(buf, len);

	if(res) lua_pushstring(L, (char*)buf);
		else lua_pushnil(L);

	return 1;
}


// uint16_t mg_ntohs(uint16_t net);
static int _mg_ntohs (lua_State *L) {
	uint16_t net = (uint16_t)luaL_checkinteger(L, 1);
	lua_pushinteger(L, net);

	return 1;
}

// uint32_t mg_ntohl(uint32_t net);
static int _mg_ntohl (lua_State *L) {
	uint32_t net = (uint32_t)luaL_checkint32(L, 1);
	lua_pushinteger(L, net);

	return 1;
}

// uint16_t mg_htons(uint16_t h);
static int _mg_htons (lua_State *L) {
	uint16_t h = (uint16_t)luaL_checkinteger(L, 1);
	lua_pushinteger(L, h);

	return 1;
}

// uint32_t mg_ntohl(uint32_t h);
static int _mg_htonl (lua_State *L) {
	uint32_t h = (uint32_t)luaL_checkinteger(L, 1);
	lua_pushinteger(L, h);

	return 1;
}

// uint32_t mg_crc32(uint32_t crc, const char *buf, size_t len);
static int _mg_crc32 (lua_State *L) {
	uint32_t crc = (uint32_t)luaL_checkinteger(L, 1);
	const char *buf = luaL_checkstring(L, 1);
	size_t len = luaL_checklong(L, 1);
	lua_pushinteger(L, (uint32_t)mg_crc32(crc, buf, len));

	return 1;
}

// int mg_check_ip_acl(struct mg_str acl, struct mg_addr *remote_ip);
static int _mg_check_ip_acl (lua_State *L)
{
	struct mg_str acl = mg_str(luaL_checkstring(L, 1));
	lua_remove(L, 1);
	mg_addr *remote_ip = checkaddr(L);
	int ret = luaL_checkinteger(L, mg_check_ip_acl(acl, remote_ip));
	lua_pushinteger(L, ret);

	return 1;
}

// int mg_url_decode(const char *src, size_t src_len, char *dst, size_t dst_len, int form);
static int _mg_url_decode (lua_State *L)
{
	const char *src = luaL_checkstring(L, 1);
	size_t src_len = luaL_checklong(L, 2);
	char *dst = (char*)luaL_checkstring(L, 3);
	size_t dst_len = luaL_checklong(L, 4);
	int form = luaL_checkinteger(L, 5);
	int ret =  mg_url_decode(src, src_len, dst, dst_len, form);
	lua_pushinteger(L, luaL_optlong(L, ret, -1));

	return 1;
}

// size_t mg_url_encode(const char *s, size_t n, char *buf, size_t len);
static int _mg_url_encode (lua_State *L)
{
	const char *src = luaL_checkstring(L, 1);
	size_t n = luaL_checklong(L, 2);
	char *buf = (char*)luaL_checkstring(L, 3);
	size_t len = luaL_checklong(L, 4);
	size_t ret =  mg_url_encode(src, n, buf, len);
	lua_pushinteger(L, luaL_optlong(L, ret, -1));

	return 1;
}

// size_t mg_print_base64(void (*out)(char, void *), void *param, va_list *ap);
static int _mg_print_base64(lua_State *L) {
	if(L)
		return 0;

	return 0;
}

// size_t mg_print_esc(void (*out)(char, void *), void *param, va_list *ap);
static int _mg_print_esc(lua_State *L) {
	if(L)
		return 0;

	return 0;
}


// size_t mg_print_hex(void (*out)(char, void *), void *param, va_list *ap);
static int _mg_print_hex(lua_State *L) {
	if(L)
		return 0;

	return 0;
}

// size_t mg_print_ip(void (*out)(char, void *), void *param, va_list *ap);
static int _mg_print_ip(lua_State *L) {
	mg_addr *addr = (mg_addr*)lua_topointer(L, 1);
	char buf[12] = {0};
	size_t ret = mg_snprintf(buf, sizeof(buf), "%M", mg_print_ip4, &addr->ip);
	lua_pushlstring(L, buf, ret);

	return 1;
}

// size_t mg_print_ip_port(void (*out)(char, void *), void *param, va_list *ap);
static int _mg_print_ip_port(lua_State *L) {
	if(L)
		return 0;

	return 0;
}

// size_t mg_print_ip4(void (*out)(char, void *), void *param, va_list *ap);
static int _mg_print_ip4(lua_State *L) {
	if(L)
		return 0;

	return 0;
}

// size_t mg_print_ip6(void (*out)(char, void *), void *param, va_list *ap);
static int _mg_print_ip6 (lua_State *L) {
	if(L)
		return 0;

	return 0;
}

// size_t mg_print_mac(void (*out)(char, void *), void *param, va_list *ap);
static int _mg_print_mac (lua_State *L) {
	if(L)
		return 0;

	return 0;
}

/*static void dumpstack (lua_State *L) {
  int top=lua_gettop(L);
  for (int i = 1; i <= top; i++) {
    printf("%d\t%s\t", i, luaL_typename(L,i));
    switch (lua_type(L, i)) {
      case LUA_TNUMBER:
        printf("%g\n",lua_tonumber(L,i));
        break;
      case LUA_TSTRING:
        printf("%s\n",lua_tostring(L,i));
        break;
      case LUA_TBOOLEAN:
        printf("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
        break;
      case LUA_TNIL:
        printf("%s\n", "nil");
        break;
      default:
        printf("%p\n",lua_topointer(L,i));
        break;
    }
  }
}*/

static const struct luaL_reg mg_util_lib_m [] = {
	{"call",		_mg_call		},
	{"error",		_mg_error		},
	{"new_md5_ctx",		new_mg_md5_ctx		},
	{"md5_init",		_mg_md5_init		},
	{"md5_update",		_mg_md5_update		},
	{"md5_final",		_mg_md5_final		},
	{"new_sha1_ctx",	new_mg_sha1_ctx		},
	{"sha1_init",		_mg_sha1_init		},
	{"sha1_update",		_mg_sha1_update		},
	{"sha1_final",		_mg_sha1_final		},
	{"base64_update",	_mg_base64_update	},
	{"base64_final",	_mg_base64_final	},
	{"base64_encode",	_mg_base64_encode	},
	{"base64_decode",	_mg_base64_decode	},
	{"base64_encode",	_mg_base64_encode	},
	{"random",		_mg_random		},
	{"random_str",		_mg_random_str		},
	{"ntohs",		_mg_ntohs		},
	{"htonl",		_mg_ntohl		},
	{"htons",		_mg_htons		},
	{"htonl",		_mg_htonl		},
	{"crc32",		_mg_crc32		},
	{"check_ip_acl",	_mg_check_ip_acl	},
	{"url_decode",		_mg_url_decode		},
	{"url_encode",		_mg_url_encode		},
	{"print_base64",	_mg_print_base64	},
	{"print_esc",		_mg_print_esc		},
	{"print_hex",		_mg_print_hex		},
	{"print_ip",		_mg_print_ip		},
	{"print_ip_port",	_mg_print_ip_port	},
	{"print_ip4",		_mg_print_ip4		},
	{"print_ip6",		_mg_print_ip6		},
	{"print_mac",		_mg_print_mac		},
	{NULL, NULL}
};

void mg_open_mg_util(lua_State *L) {
	//printf("START MG.UTIL: \n"); dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, mg_util_lib_m);
	lua_setfield(L, -2, "util");
	// mg_util
	luaL_newmetatable(L, "LuaBook.mg_util");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	lua_pop(L, 1);
	//printf("END MG.UTIL: \n"); dumpstack(L);
}