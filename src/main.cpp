#include <core.h>
#include <iostream>

int main() {

    #ifdef WL_DEBUG
    printf("debug mode...\n");
    #endif

    WLWindow* pWindow;


    pWindow = wlCreateWindow("goo goo gaa gaa",  WL_WINDOWED);
    while(!wlWindowShouldClose(pWindow)){
        wlPollWindow(pWindow);
    }
    wlCloseWindow(pWindow);

    return 0;
}