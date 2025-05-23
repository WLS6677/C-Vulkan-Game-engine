#include <core.h>
#include <iostream>

int main() {

    #ifdef WL_DEBUG
    printf("debug mode...\n");
    #endif

    WL_LOG(WL_TRACE, "HELLO");

    WLEngine* Dave = wlCreateEngine();
    wlRunEngine(Dave);
    wlDestroyEngine(Dave);

    return 0;
}