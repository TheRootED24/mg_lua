cmake_minimum_required(VERSION 3.1...3.27)

#-------------------------------------------------------------------------------
# Symlink Macro
# usage: install_symlink(filepath sympath) -- for install only
macro(install_symlink filepath sympath)
    install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${filepath} ${sympath})")
    install(CODE "message(\"-- Created symlink: ${sympath} -> ${filepath}\")")
endmacro(install_symlink)

#-------------------------------------------------------------------------------
# Build Environment
#-------------------------------------------------------------------------------

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_definitions(-fpic -shared -O3)
  add_compile_options(-W -Wall -Werror -Wextra -DMG_TLS=MG_TLS_BUILTIN -O3 )
endif()

project(
  mg_lua
  VERSION 1.0
  LANGUAGES C
  )

set(CMAKE_SHARED_LIBRARY_PREFIX "")

add_library(mg_lua SHARED mg_lua.c mg_mgr.c mg_connection.c mg_http.c mg_http_message.c mg_http_header.c mg_http_part.c
			      mg_http_serve_opts.c mg_dns.c mg_addr.c mg_ws.c mg_ws_message.c mg_sntp.c mg_iobuf.c mg_mqtt.c mg_mqtt_opts.c
            mg_mqtt_message.c mg_tls.c mg_tls_opts.c mg_timer.c mg_time.c mg_json.c mg_rpc.c mg_rpc_req.c mg_utility.c
            mg_string.c mg_str.c mg_url.c mg_logging.c
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpic")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMG_TLS=MG_TLS_BUILTIN")

target_link_libraries(mg_lua libmongoose.so)
target_link_libraries(mg_lua m)
install(TARGETS mg_lua  DESTINATION /usr/lib/lua/mg_lua/)
install(DIRECTORY src/examples DESTINATION dir/usr/lib/lua/mg_lua/examples/)
install_symlink(mg_lua src/examples/http-server/)
install_symlink(src/mg_types.lua src/examples/http-server/)
