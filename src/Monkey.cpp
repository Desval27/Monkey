#include <Monkey.h>

#ifdef USE_DEBUG
// #include <cstdarg>
// #include <cstdio>
DebugFunc debugFunc = nullptr;
void setDebugFunc(DebugFunc f)
{
    debugFunc = f;
}

void dprintf(const char* format, ...)
{
    if (debugFunc != nullptr)
    {
        va_list args;
        va_start(args, format);
        debugFunc(format, args);
        va_end(args);
    }
}
#endif
