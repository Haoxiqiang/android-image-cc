cmake_minimum_required(VERSION 3.10.2)
cmake_policy(VERSION 3.10.2)

project(libjpeg C)
set(project_name libjpeg)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_VERBOSE_MAKEFILE ON)

if (CMAKE_ANDROID_ARCH_ABI STREQUAL armeabi-v7a)
    set(ANDROID_LLVM_TRIPLE armv7-none-linux-androideabi)
elseif (CMAKE_ANDROID_ARCH_ABI STREQUAL arm64-v8a)
    set(ANDROID_LLVM_TRIPLE aarch64-none-linux-android)
elseif (CMAKE_ANDROID_ARCH_ABI STREQUAL x86)
    set(ANDROID_LLVM_TRIPLE i686-none-linux-android)
elseif (CMAKE_ANDROID_ARCH_ABI STREQUAL x86_64)
    set(ANDROID_LLVM_TRIPLE x86_64-none-linux-android)
else ()
    message(FATAL_ERROR "Invalid Android ABI: ${ANDROID_ABI}.")
endif ()

if (CMAKE_EXECUTABLE_SUFFIX)
    set(CMAKE_EXECUTABLE_SUFFIX_TMP ${CMAKE_EXECUTABLE_SUFFIX})
endif ()

set(SOURCE_PATH ../../../../../external/libjpeg-turbo)

set(VERSION 2.1.2)

string(TIMESTAMP DEFAULT_BUILD "%Y%m%d")
set(BUILD ${DEFAULT_BUILD} CACHE STRING "Build string (default: ${DEFAULT_BUILD})")

message(STATUS "VERSION = ${VERSION}, BUILD = ${BUILD}")

# Detect CPU type and whether we're building 64-bit or 32-bit code
math(EXPR BITS "${CMAKE_SIZEOF_VOID_P} * 8")
string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} CMAKE_SYSTEM_PROCESSOR_LC)
set(COUNT 1)

foreach (ARCH ${CMAKE_OSX_ARCHITECTURES})
    if (COUNT GREATER 1)
        message(FATAL_ERROR "The libjpeg-turbo build system does not support multiple values in CMAKE_OSX_ARCHITECTURES.")
    endif ()
    math(EXPR COUNT "${COUNT}+1")
endforeach ()

if (CMAKE_SYSTEM_PROCESSOR_LC MATCHES "x86_64" OR
        CMAKE_SYSTEM_PROCESSOR_LC MATCHES "amd64" OR
        CMAKE_SYSTEM_PROCESSOR_LC MATCHES "i[0-9]86" OR
        CMAKE_SYSTEM_PROCESSOR_LC MATCHES "x86" OR
        CMAKE_SYSTEM_PROCESSOR_LC MATCHES "ia32")
    if (BITS EQUAL 64 OR CMAKE_C_COMPILER_ABI MATCHES "ELF X32")
        set(CPU_TYPE x86_64)
    else ()
        set(CPU_TYPE i386)
    endif ()
    if (NOT CMAKE_SYSTEM_PROCESSOR STREQUAL ${CPU_TYPE})
        set(CMAKE_SYSTEM_PROCESSOR ${CPU_TYPE})
    endif ()
elseif (CMAKE_SYSTEM_PROCESSOR_LC STREQUAL "aarch64" OR
        CMAKE_SYSTEM_PROCESSOR_LC MATCHES "^arm")
    if (BITS EQUAL 64)
        set(CPU_TYPE arm64)
    else ()
        set(CPU_TYPE arm)
    endif ()
elseif (CMAKE_SYSTEM_PROCESSOR_LC MATCHES "^ppc" OR
        CMAKE_SYSTEM_PROCESSOR_LC MATCHES "^powerpc")
    set(CPU_TYPE powerpc)
else ()
    set(CPU_TYPE ${CMAKE_SYSTEM_PROCESSOR_LC})
endif ()
if (CMAKE_OSX_ARCHITECTURES MATCHES "x86_64" OR
        CMAKE_OSX_ARCHITECTURES MATCHES "arm64" OR
        CMAKE_OSX_ARCHITECTURES MATCHES "i386")
    set(CPU_TYPE ${CMAKE_OSX_ARCHITECTURES})
endif ()
if (CMAKE_OSX_ARCHITECTURES MATCHES "ppc")
    set(CPU_TYPE powerpc)
endif ()
if (MSVC_IDE AND CMAKE_GENERATOR_PLATFORM MATCHES "arm64")
    set(CPU_TYPE arm64)
endif ()

message(STATUS "${BITS}-bit build (${CPU_TYPE})")

###############################################################################
# CONFIGURATION OPTIONS
###############################################################################
macro(boolean_number var)
    if (${var})
        set(${var} 1 ${ARGN})
    else ()
        set(${var} 0 ${ARGN})
    endif ()
endmacro()

macro(report_option var desc)
    if (${var})
        message(STATUS "${desc} enabled (${var} = ${${var}})")
    else ()
        message(STATUS "${desc} disabled (${var} = ${${var}})")
    endif ()
endmacro()

if (DEFINED CMAKE_POSITION_INDEPENDENT_CODE AND
        NOT CMAKE_POSITION_INDEPENDENT_CODE AND ENABLE_SHARED)
    unset(CMAKE_POSITION_INDEPENDENT_CODE CACHE)
endif ()

# Code Define.
set(BITS_IN_JSAMPLE 8)
set(D_ARITH_CODING_SUPPORTED 1)
# WITH_MEM_SRCDST FALSE
set(MEM_SRCDST_SUPPORTED 0)
set(MEM_SRCDST_FUNCTIONS "global:  jpeg_mem_dest;  jpeg_mem_src;")

# Versions.
set(JPEG_LIB_VERSION 80)
#set(JPEG_LIB_VERSION 70)
#set(JPEG_LIB_VERSION 62)

###############################################################################
# COMPILER SETTINGS
###############################################################################

string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UC)

set(EFFECTIVE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_UC}}")
message(STATUS "Compiler flags = ${EFFECTIVE_C_FLAGS}")

set(EFFECTIVE_LD_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_${CMAKE_BUILD_TYPE_UC}}")
message(STATUS "Linker flags = ${EFFECTIVE_LD_FLAGS}")

include(CheckCSourceCompiles)
include(CheckIncludeFiles)
include(CheckTypeSize)

check_type_size("size_t" SIZE_T)
check_type_size("unsigned long" UNSIGNED_LONG)

if (SIZE_T EQUAL UNSIGNED_LONG)
    check_c_source_compiles("int main(int argc, char **argv) { unsigned long a = argc;  return __builtin_ctzl(a); }"
            HAVE_BUILTIN_CTZL)
endif ()

set(INLINE_OPTIONS "__inline__;inline")
list(INSERT INLINE_OPTIONS 0 "inline __attribute__((always_inline))")
list(INSERT INLINE_OPTIONS 0 "__inline__ __attribute__((always_inline))")

foreach (inline ${INLINE_OPTIONS})
    check_c_source_compiles("${inline} static int foo(void) { return 0; } int main(void) { return foo(); }"
            INLINE_WORKS)
    if (INLINE_WORKS)
        set(INLINE ${inline})
        break()
    endif ()
endforeach ()
if (NOT INLINE_WORKS)
    message(FATAL_ERROR "Could not determine how to inline functions.")
endif ()

message(STATUS "INLINE = ${INLINE} ")

# Generate files
configure_file(${SOURCE_PATH}/jconfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/jconfig.h)
configure_file(${SOURCE_PATH}/jconfigint.h.in ${CMAKE_CURRENT_BINARY_DIR}/jconfigint.h)

# Include directories and compiler definitions
include_directories(
        ${SOURCE_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}
)

set(JPEG_SOURCES
        ${SOURCE_PATH}/jcapimin.c
        ${SOURCE_PATH}/jcapistd.c
        ${SOURCE_PATH}/jccoefct.c
        ${SOURCE_PATH}/jccolor.c
        ${SOURCE_PATH}/jcdctmgr.c
        ${SOURCE_PATH}/jchuff.c
        ${SOURCE_PATH}/jcicc.c
        ${SOURCE_PATH}/jcinit.c
        ${SOURCE_PATH}/jcmainct.c
        ${SOURCE_PATH}/jcmarker.c
        ${SOURCE_PATH}/jcmaster.c
        ${SOURCE_PATH}/jcomapi.c
        ${SOURCE_PATH}/jcparam.c
        ${SOURCE_PATH}/jcphuff.c
        ${SOURCE_PATH}/jcprepct.c
        ${SOURCE_PATH}/jcsample.c
        ${SOURCE_PATH}/jctrans.c
        ${SOURCE_PATH}/jdapimin.c
        ${SOURCE_PATH}/jdapistd.c
        ${SOURCE_PATH}/jdatadst.c
        ${SOURCE_PATH}/jdatasrc.c
        ${SOURCE_PATH}/jdcoefct.c
        ${SOURCE_PATH}/jdcolor.c
        ${SOURCE_PATH}/jddctmgr.c
        ${SOURCE_PATH}/jdhuff.c
        ${SOURCE_PATH}/jdicc.c
        ${SOURCE_PATH}/jdinput.c
        ${SOURCE_PATH}/jdmainct.c
        ${SOURCE_PATH}/jdmarker.c
        ${SOURCE_PATH}/jdmaster.c
        ${SOURCE_PATH}/jdmerge.c
        ${SOURCE_PATH}/jdphuff.c
        ${SOURCE_PATH}/jdpostct.c
        ${SOURCE_PATH}/jdsample.c
        ${SOURCE_PATH}/jdtrans.c
        ${SOURCE_PATH}/jerror.c
        ${SOURCE_PATH}/jfdctflt.c
        ${SOURCE_PATH}/jfdctfst.c
        ${SOURCE_PATH}/jfdctint.c
        ${SOURCE_PATH}/jidctflt.c
        ${SOURCE_PATH}/jidctfst.c
        ${SOURCE_PATH}/jidctint.c
        ${SOURCE_PATH}/jidctred.c
        ${SOURCE_PATH}/jquant1.c
        ${SOURCE_PATH}/jquant2.c
        ${SOURCE_PATH}/jutils.c
        ${SOURCE_PATH}/jmemmgr.c
        ${SOURCE_PATH}/jmemnobs.c)

# Add ARITH ENC/DEC Files.
set(JPEG_SOURCES ${JPEG_SOURCES}
        ${SOURCE_PATH}/jaricom.c
        ${SOURCE_PATH}/jcarith.c
        ${SOURCE_PATH}/jdarith.c
        )

include_directories(
        ${SOURCE_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}
)

# Add SIMD Support.
add_subdirectory(${SOURCE_PATH}/simd simd)
add_definitions(-DNEON_INTRINSICS)

add_library(${project_name} STATIC
        ${JPEG_SOURCES}
        $<TARGET_OBJECTS:simd>)
target_include_directories(${project_name} PUBLIC
        ${SOURCE_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}
        )
