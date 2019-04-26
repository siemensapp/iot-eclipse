#Set sources
ADD_LIBRARY(zf_log OBJECT zf_log.c)

#PIC option is required in order to build zf_log with shared library target
SET_TARGET_PROPERTIES(zf_log PROPERTIES POSITION_INDEPENDENT_CODE ON)

#Settings for zf_log
SET(ZF_LOG_LIBRARY_PREFIX CACHE STRING "Prefix for all linker symbols exported by the library")
OPTION(ZF_LOG_USE_DEBUGSTRING "Use OutputDebugString (Windows) by default when available" OFF)
OPTION(ZF_LOG_OPTIMIZE_SIZE "Optimize for size (prefer size over speed)" OFF)

IF(ZF_LOG_LIBRARY_PREFIX)
    TARGET_COMPILE_DEFINITIONS(zf_log PRIVATE "ZF_LOG_LIBRARY_PREFIX=${ZF_LOG_LIBRARY_PREFIX}")
ENDIF()
IF(ZF_LOG_USE_DEBUGSTRING)
    TARGET_COMPILE_DEFINITIONS(zf_log PRIVATE "ZF_LOG_USE_DEBUGSTRING")
ENDIF()
IF(ZF_LOG_OPTIMIZE_SIZE)
    TARGET_COMPILE_DEFINITIONS(zf_log PRIVATE "ZF_LOG_OPTIMIZE_SIZE")
ENDIF()

IF(MSVC90)
MESSAGE(STATUS "Compiler: Visual Studio 2008 (set /TP option for zf_log)")
SET_SOURCE_FILES_PROPERTIES(zf_log.c PROPERTIES COMPILE_FLAGS "/TP")
ENDIF()
