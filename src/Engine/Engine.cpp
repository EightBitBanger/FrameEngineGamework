#include <GameEngineFramework/Engine/Engine.h>

ENGINE_API EngineComponents  Components;
ENGINE_API ColorPreset       Colors;
ENGINE_API NumberGeneration  Random;
ENGINE_API Logger            Log;
ENGINE_API Timer             PhysicsTime;
ENGINE_API Timer             Time;
ENGINE_API ProfilerTimer     Profiler;

ENGINE_API Serialization     Serializer;
ENGINE_API ResourceManager   Resources;
ENGINE_API ScriptSystem      Scripting;
ENGINE_API RenderSystem      Renderer;
ENGINE_API PhysicsSystem     Physics;
ENGINE_API NetworkSystem     Network;
ENGINE_API AudioSystem       Audio;
ENGINE_API InputSystem       Input;
ENGINE_API MathCore          Math;
ENGINE_API ActorSystem       AI;

ENGINE_API ApplicationLayer      Application;
ENGINE_API EngineSystemManager   Engine;

ENGINE_API FileSystemDir     Directory;

ENGINE_API StringType        String;
ENGINE_API FloatType         Float;
ENGINE_API DoubleType        Double;
ENGINE_API IntType           Int;
ENGINE_API UintType          Uint;


EngineSystemManager::EngineSystemManager(void) : 
    sceneMain(nullptr),
    
    mDataStreamIndex(0),
    mObjectIndex(0),
    mStreamSize(0)
{
}

Button* EngineSystemManager::CreateOverlayButtonCallback(int x, int y, int width, int height, ButtonCallBack callback) {
    Button* newButton = mButtons.Create();
    newButton->x = x;
    newButton->y = y;
    newButton->w = width;
    newButton->h = height;
    newButton->callback = callback;
    return newButton;
}

bool EngineSystemManager::DestroyOverlayButtonCallback(Button* button) {
    return mButtons.Destroy( button );
}

void EngineSystemManager::AddMeshText(GameObject* overlayObject, float xPos, float yPos, float width, float height, std::string text, Color textColor) {
    
    Mesh* meshPtr = overlayObject->GetComponent<MeshRenderer>()->mesh;
    if (meshPtr == nullptr) 
        return;
    
    for (unsigned int i=0; i < text.size(); i++)
        AddMeshSubSprite(overlayObject, xPos + i, yPos, width, height, text[i], textColor);
    
    meshPtr->UploadToGPU();
    
    return;
}

void EngineSystemManager::AddMeshSubSprite(GameObject* overlayObject, float xPos, float yPos, float width, float height, int index, Color meshColor) {
    
    Mesh* meshPtr = overlayObject->GetComponent<MeshRenderer>()->mesh;
    if (meshPtr == nullptr) 
        return;
    
    Text* textPtr = overlayObject->GetComponent<Text>();
    if (textPtr == nullptr) 
        return;
    
    // Sprite atlas layout
    float spriteStartX  = textPtr->sprite.subSpriteX;
    float spriteStartY  = textPtr->sprite.subSpriteY;
    float spriteWidth   = textPtr->sprite.subSpriteWidth;
    float spriteHeight  = textPtr->sprite.subSpriteHeight;
    
    float spacingWidth  = textPtr->width;
    float spacingHeight = textPtr->height;
    
    int mapWidth  = textPtr->sprite.width;
    int mapHeight = textPtr->sprite.height;
    
    // Calculate the sub sprite in the map grid
    int subWidth  = 0;
    int subHeight = 0;
    
    for (int i=0; i < index; i++) {
        
        subWidth++;
        
        if (subWidth > mapWidth) {
            subWidth=0;
            
            subHeight++;
            
            if (subHeight > mapHeight)
                return;
        }
    }
    
    meshPtr->AddPlain(yPos * spacingHeight, 
                      0, 
                      -(xPos * spacingWidth), 
                      width, height, 
                      meshColor, 
                      spriteWidth, spriteHeight, 
                      spriteStartX, spriteStartY, 
                      subWidth, subHeight);
    
    return;
}

GameObject* EngineSystemManager::GetGameObject(unsigned int index) {
    if (index < mGameObjects.Size()) 
        return mGameObjects[index];
    return nullptr;
}

unsigned int EngineSystemManager::GetGameObjectCount(void) {
    return mGameObjects.Size();
}

unsigned int EngineSystemManager::GetComponentCount(void) {
    return mComponents.Size();
}

void EngineSystemManager::Initiate() {
    
    // Load default shaders
    shaders.texture       = Resources.CreateShaderFromTag("texture");
    shaders.textureUnlit  = Resources.CreateShaderFromTag("textureUnlit");
    shaders.color         = Resources.CreateShaderFromTag("color");
    shaders.colorUnlit    = Resources.CreateShaderFromTag("colorUnlit");
    shaders.UI            = Resources.CreateShaderFromTag("UI");
    shaders.shadowCaster  = Resources.CreateShaderFromTag("shadowCaster");
    
    // Load default meshes
    meshes.cube  = Resources.CreateMeshFromTag("cube");
    meshes.plain = Resources.CreateMeshFromTag("plain");
    
    // Main world scene
    sceneMain = Create<Scene>();
    
    // Initiate render system defaults
    Renderer.shaders.texture      = shaders.texture;
    Renderer.shaders.textureUnlit = shaders.textureUnlit;
    Renderer.shaders.color        = shaders.color;
    Renderer.shaders.colorUnlit   = shaders.colorUnlit;
    Renderer.shaders.UI           = shaders.UI;
    Renderer.shaders.shadowCaster = shaders.shadowCaster;
    
    Renderer.meshes.cube   = meshes.cube;
    Renderer.meshes.plain  = meshes.plain;
    
    return;
}

void EngineSystemManager::Shutdown(void) {
    
    Destroy<Scene>(sceneMain);
    
    while (GetGameObjectCount() > 0) {
        DestroyGameObject( GetGameObject(0) );
    }
    
    Renderer.DestroyShader(shaders.texture);
    Renderer.DestroyShader(shaders.textureUnlit);
    Renderer.DestroyShader(shaders.color);
    Renderer.DestroyShader(shaders.colorUnlit);
    Renderer.DestroyShader(shaders.UI);
    Renderer.DestroyShader(shaders.shadowCaster);
    
    assert(mComponents.Size() == 0);
    assert(mGameObjects.Size() == 0);
    assert(mTextObjects.Size() == 0);
    
    return;
}

