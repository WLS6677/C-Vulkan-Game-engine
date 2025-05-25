#ifndef WL_CORE
#define WL_CORE

#include <stdio.h>
#include <time.h>
#include <wlmath.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SIZEOFARRAY
#define SIZE_OF_ARRAY(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#ifdef WL_MATH

#define vec2u32 WLU32Vec2
#define vec3f WLFLOATVec3

#endif //WL_MATH

// for miscilanious random allocs so i only need to alloc in the code and not worry about free
typedef struct WLAllocator WLAllocator;

// the size input determines how many bytes the allocater will allocate and use
void wlSetupAllocator(uint32_t size);
void wlReleaseAlloc();
uint8_t* wlAlloc(uint32_t size);
void wlPrintAllocatorInfo();

// for reading files from disc
uint8_t* wlReadFile(const char* file_name, size_t* file_size);

// debug flags for wlLog
typedef enum {
    WL_LOG_FATAL,
    WL_LOG_WARNING,
    WL_LOG_TRACE,
    WL_LOG_PRINT
} WLResult;

void wlLog(WLResult result, const char* file, uint32_t line, const char* func, const char* msg);

//this is the debugging logging macro, it will either lead to a debug manager or do nothing on release
#ifdef WL_DEBUG
    #define WL_LOG(level, msg) wlLog(level, __FILE__, __LINE__, __func__, msg)
#else
    #define WL_LOG(level, msg) ((void)0)
#endif


//////////////////////////////////////
//                                  //
//           E N G I N E            //
//                                  //
//////////////////////////////////////


// this struct will be responsible for storing the data of the Engine
typedef struct WLEngine WLEngine;

// this will Init all the libraries needed for the engine to run.
// and initizialize all the data it will need
WLEngine* wlCreateEngine();
// frees all memory from the engine and all its components
void wlDestroyEngine(WLEngine* engine);

// updates all modoles and handles Syncronization and time and
void wlRunEngine(WLEngine* engine);
 

#ifdef __cplusplus
} //extern "C"
#endif

// these are the modules, i need to include them after the debugging defintions to prevent errors 
#include <renderer.h>
#include <window.h>

#endif // #ifndef WL_CORE
