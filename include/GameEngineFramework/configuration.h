#ifndef APPLICATION_CONFIGURATION
#define APPLICATION_CONFIGURATION

//
// Application layout
//

#define  WINDOW_NAME  "Render window"

// Starting window dimensions relative to display size
#define  WINDOW_WIDTH       0.7
#define  WINDOW_HEIGHT      0.8

// Minimum window size (In pixels)
#define  WINDOW_WIDTH_MIN   300
#define  WINDOW_HEIGHT_MIN  200

//#define  APPLICATION_CURSOR_HIDDEN_ON_START
//#define  APPLICATION_ESCAPE_KEY_PAUSE

// Console window area
#define     WINDOW_CONSOLE_LEFT    5
#define     WINDOW_CONSOLE_TOP     50
#define     WINDOW_CONSOLE_WIDTH   700
#define     WINDOW_CONSOLE_HEIGHT  800



//
// Profiling
//

//#define PROFILE_ENGINE_CORE



//
// Event log
//

//#define  EVENT_LOG_DETAILED



//
// Rendering
//

#define  RENDER_FRAMES_PER_SECOND          60

//#define  RENDERER_CHECK_OPENGL_ERRORS

#define RENDER_NUMBER_OF_LIGHTS    50

#define RENDER_NUMBER_OF_SHADOWS   8

#define  LOG_RENDER_DETAILS



//
// Physics
//

#define PHYSICS_UPDATES_PER_SECOND         60



//
// Unit testing
//

//#define RUN_UNIT_TESTS



//
// Tick update
//

#define  TICK_UPDATES_PER_SECOND           60


//
// AI
//

#define NEURAL_LAYER_WIDTH  5







#ifdef BUILD_CORE
    #define ENGINE_API  __declspec(dllexport)
#endif

#ifdef BUILD_APPLICATION
    #define ENGINE_API  __declspec(dllexport)
#endif

#ifdef BUILD_RUNTIME
    #define ENGINE_API  __declspec(dllimport)
#endif


#endif
