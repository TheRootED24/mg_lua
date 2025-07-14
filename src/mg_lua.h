#ifndef MG_H
#define MG_H

#define MG "mg_lua"

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

#include "mg_lua_cb.h"
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
#include "mg_url.h"
#include "mg_logging.h"
#include "mg_fs.h"
#include "mg_fd.h"
#include "mg_fs_file.h"
#include "mg_queue.h"

#ifdef __cplusplus
}
#endif

extern struct mg_fs mg_fs_posix;   // POSIX open/close/read/write/seek
extern struct mg_fs mg_fs_packed;  // see tutorials/core/embedded-filesystem
extern struct mg_fs mg_fs_fat;     // FAT FS

#endif
