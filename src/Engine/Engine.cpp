#include <GameEngineFramework/Engine/Engine.h>

ENGINE_API EngineComponents  Components;
ENGINE_API ColorPreset       Colors;
ENGINE_API RandomGen         Random;
ENGINE_API Logger            Log;
ENGINE_API Timer             PhysicsTime;
ENGINE_API Timer             Time;
ENGINE_API ProfilerTimer     Profiler;

ENGINE_API Serialization     Serializer;
ENGINE_API ResourceManager   Resources;
ENGINE_API ScriptSystem      Scripting;
ENGINE_API RenderSystem      Renderer;
ENGINE_API PhysicsSystem     Physics;
ENGINE_API AudioSystem       Audio;
ENGINE_API InputSystem       Input;
ENGINE_API MathCore          Math;
ENGINE_API ActorSystem       AI;

ENGINE_API ApplicationLayer      Application;
ENGINE_API EngineSystemManager   Engine;

ENGINE_API StringType        String;
ENGINE_API FloatType         Float;
ENGINE_API DoubleType        Double;
ENGINE_API IntType           Int;
ENGINE_API UintType          Uint;


EngineSystemManager::EngineSystemManager(void) : 
    sceneMain(nullptr),
    
    mouseOldX(0),
    mouseOldY(0),
    
    doUpdateDataStream(true),
    streamSize(0)
{
}

Button* EngineSystemManager::CreateOverlayButtonCallback(int x, int y, int w, int h, ButtonCallBack callback) {
    Button* newButton = mButtons.Create();
    newButton->x = x;
    newButton->y = y;
    newButton->w = w;
    newButton->h = h;
    newButton->callback = callback;
    return newButton;
}

bool EngineSystemManager::DestroyOverlayButtonCallback(Button* button) {
    return mButtons.Destroy( button );
}

void EngineSystemManager::AddMeshText(GameObject* overlayObject, float xPos, float yPos, float scaleWidth, float scaleHeight, std::string text, Color textColor) {
    
    Mesh* meshPtr = overlayObject->GetComponent<MeshRenderer>()->mesh;
    if (meshPtr == nullptr) 
        return;
    
    for (unsigned int i=0; i < text.size(); i++)
        AddMeshSubSprite(overlayObject, xPos + i, yPos, scaleWidth, scaleHeight, text[i], textColor);
    
    meshPtr->UploadToGPU();
    
    return;
}

void EngineSystemManager::AddMeshSubSprite(GameObject* overlayObject, float xPos, float yPos, float scaleWidth, float scaleHeight, int index, Color meshColor) {
    
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
                      scaleWidth, scaleHeight, 
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
    
    shaders.texture       = Resources.CreateShaderFromTag("texture");
    shaders.textureUnlit  = Resources.CreateShaderFromTag("textureUnlit");
    shaders.color         = Resources.CreateShaderFromTag("color");
    shaders.colorUnlit    = Resources.CreateShaderFromTag("colorUnlit");
    shaders.UI            = Resources.CreateShaderFromTag("UI");
    
    return;
}

void EngineSystemManager::Shutdown(void) {
    
    while (GetGameObjectCount() > 0) {
        DestroyGameObject( GetGameObject(0) );
    }
    
    Renderer.DestroyShader(shaders.texture);
    Renderer.DestroyShader(shaders.textureUnlit);
    Renderer.DestroyShader(shaders.color);
    Renderer.DestroyShader(shaders.UI);
    
    assert(mComponents.Size() == 0);
    assert(mGameObjects.Size() == 0);
    assert(mTextObjects.Size() == 0);
    
    return;
}

