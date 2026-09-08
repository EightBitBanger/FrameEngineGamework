#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/Plugins/plugins.h>

MeshRenderer* boundsRenderer = nullptr;

std::vector<std::pair<std::string, glm::vec3>> QueryPickups(const glm::vec3& position, float range) {return GameWorld.QueryPickupNames(position, range);}
bool PlacePickup(const std::string& type, const glm::vec3& position, const glm::vec3& rotation) {GameWorld.QueuePickupAt(type, position, rotation); return true;}
bool RemovePickup(const glm::vec3& position, std::string& collectedItem) {return GameWorld.RemovePickupAt(position, 0.1f, &collectedItem);}

std::vector<std::pair<std::string, glm::vec3>> QueryStatic(const glm::vec3& position, float range) {return GameWorld.QueryDecorNames(position, range);}
bool PlaceStatic(const std::string& type, const glm::vec3& position, const glm::vec3& rotation) {GameWorld.QueueDecorAt(type, position, rotation); return true;}
bool RemoveStatic(const glm::vec3& position, std::string& collectedItem) {return GameWorld.RemoveDecorAt(position, 0.1f); collectedItem = "";}

std::string GetName(int language, int namePart) {
    std::vector<std::string> germanPrefixes       = {"Eisen", "Stark", "Rosen", "Adel", "Wald", "Kaiser", "Nord", "Donner"};
    std::vector<std::string> germanNames          = {"berg", "stein", "burg", "feld", "gard", "muth", "brand", "hart", "vogel", "wolf"};
    std::vector<std::string> germanSuffixes       = {"er", "en", "ung", "ich", "heim", "stadt", "dorf", "mann", "dort"};
    std::vector<std::string> germanCoreSuffixes   = {"art", "old", "hard", "rich", "mann", "mund", "vald", "mar", "st", "cht"};
    
    std::vector<std::string> norsePrefixes        = {"Skar", "Varg", "Thor", "Grim", "Bjor", "Rune", "Hild", "Fen", "Krag", "Tyr"};
    std::vector<std::string> norseNames           = {"val", "mund", "gard", "geir", "kell", "ulf", "dorn", "var", "brand", "skald"};
    std::vector<std::string> norseSuffixes        = {"heim", "dottir", "son", "vir", "fjell", "rok", "vik"};
    std::vector<std::string> norseCoreSuffixes    = {"ar", "ur", "ir", "ald", "ing", "or"};
    NameGenerator generator;
    
    switch (namePart) {
    case 0: return generator.GenerateFirstName(germanPrefixes, germanNames, germanSuffixes, germanCoreSuffixes); break; // Shorter first name
    case 1: return generator.GenerateIncarnation(germanPrefixes, germanNames, germanSuffixes, germanCoreSuffixes); break; // Longer family name
    }
    return "";
}

void Start() {
    // Key bindings
    Input.BindKeyPressToFunction(VK_F4,     keyBindF4);
    Input.BindKeyPressToFunction(VK_F11,    keyBindFullscreen);
    Input.BindKeyPressToFunction(VK_ESCAPE, keyBindEscape);
    
    // Selection bounds renderer
    boundsRenderer = Engine.Create<MeshRenderer>();
    boundsRenderer->mesh = Engine.Create<Mesh>();
    boundsRenderer->material = Engine.Create<Material>();
    
    boundsRenderer->mesh->isShared = false;
    boundsRenderer->mesh->SetPrimitive(MESH_LINE_LOOP);
    boundsRenderer->material->isShared = false;
    boundsRenderer->material->shader = Resources.shaders.colorUnlit;
    
    boundsRenderer->material->ambient = Colors.MakeGrayScale(0.2f);
    
    Engine.sceneMain->AddMeshRendererToSceneRoot(boundsRenderer);
    
    // Initiate platform
    Platform.ShowMouseCursor();
    Platform.isPaused = true;
    
    // Event callback
    Platform.EventCallbackLoseFocus = EventLostFocus;
    Platform.EventCallbackResize = EventOnResize;
    
    // Initiate client version
    std::string version = "0.1.0";
    GameWorld.version = version;
    
    // User plug-in initiation
    GameWorld.Initiate();
    Weather.Initiate();
    Particle.Initiate();
    
    // Create a camera controller
    CreateCameraController(glm::vec3(0, 0, 0), CameraType::Free);
    
    // Disable mouse look on startup.
    Engine.sceneMain->camera->DisableMouseLook();
    
    // InspectorWindow is dynamically registered in OpenActorInspector() when an actor is clicked
    UI.AddUIFrameCallback(MainMenu);
    
    // Weather system
    GameWorld.worldMaterial->ambient = Colors.MakeGrayScale(0.9f);
    GameWorld.staticMaterial->ambient = Colors.MakeGrayScale(0.4f);
    
    GameWorld.worldMaterial->diffuse = Colors.MakeGrayScale(0.4f);
    GameWorld.staticMaterial->diffuse = Colors.MakeGrayScale(0.4f);
    
    Weather.SetPlayerObject(Engine.cameraController);
    Weather.SetWorldMaterial(GameWorld.worldMaterial);
    Weather.SetStaticMaterial(GameWorld.staticMaterial);
    Weather.SetWaterMaterial(GameWorld.waterMaterial);
    
    // Lighting levels
    Weather.SetWorldLightLevel(0.01f, 0.9f);
    Weather.SetSkyLightLevel(0.01f, 0.4f);
    Weather.SetWorldLightAmbience(Color(0.0f, 0.0f, 1.0f) + Colors.MakeGrayScale(0.3f));
    
    // World rendering
    GameWorld.chunkSize = 64;
    GameWorld.renderDistance = 11;
    GameWorld.staticDistance = 0.6f;
    GameWorld.actorDistance  = 0.3f;
    
    GameWorld.world.snowCapHeight = 20.0f;
    GameWorld.world.snowCapBias = 8.0f;
    GameWorld.world.waterLevel = -8.0f;
    
    Weather.SetTime(5000);
    
    // Set the world query callback
    AI.SetWorldPickupCallbacks(QueryPickups, PlacePickup, RemovePickup);
    AI.SetWorldStaticCallbacks(QueryStatic, PlaceStatic, RemoveStatic);
    AI.SetNameGenerator(GetName);
    
    // Physics
    Physics.SetWorldGravity(0.0f, -9.81f, 0.0f);
}
