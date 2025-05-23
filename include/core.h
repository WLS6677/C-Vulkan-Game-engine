#ifndef WL_CORE
#define WL_CORE

#include <stdio.h>
#include <time.h>
#include <wlmath.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
#define SIZE_OF_ARRAY(arr) (sizeof(arr) / sizeof((arr)[0]))

// for miscilanious random allocs so i only need to alloc in the code and not worry about free
typedef struct WLAllocator WLAllocator;

// the size input determines how many bytes the allocater will allocate and use
void wlSetupAllocator(uint32_t size);
void wlReleaseAlloc();
uint8_t* wlAlloc(uint32_t size);
void wlPrintAllocatorInfo();

// debug flags for wlLog
typedef enum {
    WL_FATAL,
    WL_TRACE,
    WL_WARNING,
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
