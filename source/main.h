//#define _WIN32_WINNT  0x500
#include <SDKDDKVer.h>
#include <windows.h>

#include <thread>
#include <mutex>

#include "configuration.h"
#include "plugins.h"


// My standard includes
#include "../vendor/my-std-lib/poolallocator.h"
#include "../vendor/my-std-lib/strings.h"
#include "../vendor/my-std-lib/types.h"
#include "../vendor/my-std-lib/random.h"
#include "../vendor/my-std-lib/logging.h"
#include "../vendor/my-std-lib/fileloader.h"
#include "../vendor/my-std-lib/timer.h"

#include "Input/InputSystem.h"
#include "Physics/PhysicsSystem.h"
#include "Renderer/RenderSystem.h"
#include "Resources/ResourceManager.h"
#include "Scripting/ScriptSystem.h"

#include "application/applicationlayer.h"

#include "engine/engine.h"

