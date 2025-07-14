cmake_minimum_required(VERSION 3.1...3.27)

#-------------------------------------------------------------------------------
# Build Environment
#-------------------------------------------------------------------------------

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_definitions(-fpic -W -DMG_ENABLE_LINES -D MG_ENABLE_MBEDTLS)
  add_compile_options(-W -Wall -Werror -Wextra -DMG_TLS=MG_TLS_BUILTIN)
endif()

project(
  mongoose
  VERSION 7.1.9
  LANGUAGES C
  )

# Add OpenSSL include directories
include_directories(${MBEDTLS_INCLUDE_DIR})

#set(OS_LIBS mbedTLS::SSL mbedTLS::Crypto Threads::Threads)

if(${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
  list(APPEND OS_LIBS socket)
endif()

##set(CMAKE_SHARED_LIBRARY_PREFIX "")
add_library(mongoose SHARED mongoose.c)
set_target_properties(mongoose PROPERTIES VERSION ${PROJECT_VERSION} )
set_target_properties (mongoose PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR} )
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpic -D MG_TLS_MBED")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -DMG_TLS=MG_TLS_BUILTIN")
#target_link_libraries(mongoose mbedtls mbedx509 mbedcrypto)

set_target_properties(mongoose PROPERTIES PUBLIC_HEADER "mongoose.h")
INSTALL(TARGETS mongoose 
        LIBRARY DESTINATION /usr/lib/
        PUBLIC_HEADER DESTINATION /usr/include/libmongoose/
)
