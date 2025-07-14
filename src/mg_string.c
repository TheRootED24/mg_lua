#include "mg_string.h"

// int mg_casecmp(const char *s1, const char *s2);
static int _mg_casecmp(lua_State *L) {
	const char *s1, *s2;
	s1 = luaL_checkstring(L, 1);
	s2 = luaL_checkstring(L, 2);
	lua_pushinteger(L, mg_casecmp(s1, s2));
	return 1;
}

// int mg_strcmp(const struct mg_str str1, const struct mg_str str2);
static int _mg_strcmp(lua_State *L) {
	struct mg_str *s1, *s2;
	s1 = check_mg_str(L); lua_remove(L, 1); // pop the first one off the stack
	s2 = check_mg_str(L);
	lua_pushinteger(L, mg_strcmp(*s1, *s2));
	return 1;
}

// int mg_strcasecmp(const struct mg_str str1, const struct mg_str str2);
static int _mg_strcasecmp(lua_State *L) {
	struct mg_str *s1, *s2;
	s1 = check_mg_str(L); lua_remove(L, 1); // pop the first one off the stack
	s2 = check_mg_str(L);
	lua_pushinteger(L, mg_strcasecmp(*s1, *s2));
	return 1;
}

// struct mg_str mg_strdup(const struct mg_str s);
static int _mg_strdup(lua_State *L) {
	struct mg_str *str = check_mg_str(L);
	lua_pop(L, 1); new_mg_str(L);
	struct mg_str *s = check_mg_str(L);
	*s = mg_strdup(*str);
	return 1;
}

// bool mg_match(struct mg_str str, struct mg_str pattern, struct mg_str *caps);
static int _mg_match(lua_State *L) {
	int nargs = lua_gettop(L);
	struct mg_str *str, *pat, *caps;
	str = check_mg_str(L); lua_remove(L, 1); // pop the first one off the stack
	pat = check_mg_str(L); lua_remove(L, 1); // pop the next one off the stack
	caps = nargs == 3 ? check_mg_str(L) : NULL;

	lua_pushboolean(L, mg_match(*str, *pat, caps));

	return 1;
}

// bool mg_span(struct mg_str s, struct mg_str *a, struct mg_str *b, char delim);
static int _mg_span(lua_State *L) {
	struct mg_str *s, *a, *b;
	s = check_mg_str(L); lua_remove(L, 1); // pop the first one off the stack
	a = check_mg_str(L); lua_remove(L, 1); // pop the next one off the stack
	b = check_mg_str(L); lua_remove(L, 1); // pop the next one off the stack
	char d = (char) luaL_checkint(L, 1);

	lua_pushboolean(L, mg_span(*s, a, b, d));

	return 1;
}

// bool mg_str_to_num(struct mg_str s, int base, void *val, size_t val_len);
static int _mg_str_to_num(lua_State *L) {
	struct mg_str s;
	s = *check_mg_str(L);
	int base = luaL_checkinteger(L, 2);
	void *val = (void*)lua_topointer(L, 3);
	size_t vlen = luaL_checklong(L, 4);
	lua_pushboolean(L, mg_str_to_num(s, base, val, vlen));

	return 1;
}

// bool mg_path_is_sane(struct mg_str path);
static int _mg_path_is_sane(lua_State *L) {
	struct mg_str path;
	path = *check_mg_str(L);
	lua_pushboolean(L, mg_path_is_sane(path));
	return 1;
}

// void mg_pfn_iobuf(char ch, void *param);
static int _mg_pfn_iobuf(lua_State *L) {
	int ch = (char)luaL_checkint(L, 1);
	lua_remove(L, 1);
	mg_iobuf *io = checkiobuf(L);
	mg_pfn_iobuf(ch, io);

	return 0;
}

// bool mg_aton(struct mg_str str, struct mg_addr *addr);
static int _mg_aton(lua_State *L) {
	struct mg_str str;
	mg_addr *addr;
	str = *check_mg_str(L);
	lua_remove(L, 1);
	addr = checkaddr(L);
	lua_pushboolean(L, mg_aton(str, addr));

	return 1;
}

static void dumpstack (lua_State *L) {
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
}

static const struct luaL_reg mg_string_lib_m [] = {
	{"casecmp",			_mg_casecmp			},
	{"strcmp",			_mg_strcmp			},
	{"strcasecomp",		_mg_strcasecmp		},
	{"strdup", 			_mg_strdup			},
	{"match",			_mg_match			},
	{"span", 			_mg_span			},
	{"str2num",			_mg_str_to_num		},
	{"path_is_sane",	_mg_path_is_sane	},
	{"pfn_iobuf", 		_mg_pfn_iobuf		},
	{"aton",			_mg_aton			},
	{NULL, NULL}
};

void mg_open_mg_string(lua_State *L) {
	printf("START MG.STRING: \n"); dumpstack(L);
	lua_newtable(L);
	luaL_register(L, NULL, mg_string_lib_m);
	lua_setfield(L, -2, "string");
	// mg_string
	luaL_newmetatable(L, "LuaBook.mg_string");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);  /* pushes the metatable */
	lua_settable(L, -3);  /* metatable.__index = metatable */
	lua_pop(L, 1);

	printf("END MG.STRING: \n"); dumpstack(L);
}