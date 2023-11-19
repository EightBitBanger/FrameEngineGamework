#ifndef APPLICATION_CONFIGURATION
#define APPLICATION_CONFIGURATION

//
// Window layout
//

#define  WINDOW_NAME  "Render window"
// Dimensions
#define  WINDOW_WIDTH       1330
#define  WINDOW_HEIGHT      1000
// MAX size
#define  WINDOW_WIDTH_MIN   300
#define  WINDOW_HEIGHT_MIN  200

#define  WINDOW_MOUSE_HIDDEN_ON_START

// Console window area
#define     WINDOW_CONSOLE_LEFT    5
#define     WINDOW_CONSOLE_TOP     50
#define     WINDOW_CONSOLE_WIDTH   700
#define     WINDOW_CONSOLE_HEIGHT  800

// Exit runtime with the ESCAPE key
//#define  ESCAPE_KEY_QUIT


//
// Event log
//

//#define  EVENT_LOG_DETAILED


//
// Tick update
//

#define  TICK_UPDATES_PER_SECOND           3


//
// Rendering
//

#define  RENDER_FRAMES_PER_SECOND          120
#define  RENDERER_CHECK_OPENGL_ERRORS

#define RENDER_NUMBER_OF_LIGHTS    30

#define  LOG_RENDER_DETAILS


//
// Physics
//

#define PHYSICS_UPDATES_PER_SECOND         60


//
// Unit testing
//

#define RUN_UNIT_TESTS



#endif
