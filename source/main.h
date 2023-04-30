//#define _WIN32_WINNT  0x500
#include <SDKDDKVer.h>
#include <windows.h>

#include "configuration.h"
#include "plugins.h"


// My standard includes
#include "std/poolallocator.h"
#include "std/strings.h"
#include "std/types.h"
#include "std/random.h"
#include "std/logging.h"
#include "std/fileloader.h"
#include "std/timer.h"

#include "Input/InputSystem.h"
#include "Physics/PhysicsSystem.h"
#include "Renderer/RenderSystem.h"
#include "Resources/ResourceManager.h"
#include "Scripting/ScriptSystem.h"

#include "application/applicationlayer.h"

#include "engine/engine.h"

