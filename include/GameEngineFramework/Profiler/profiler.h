#ifndef CORE_PROFILER
#define CORE_PROFILER

#include "../../../vendor/codebaselibrary/timer.h"

class __declspec(dllexport) ProfilerTimer {
    
public:
    
    void Begin(void);
    
    float Query(void);
    
private:
    
    Timer timer;
    
};

#endif
