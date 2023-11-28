#include "engine.h"

__declspec(dllexport) EngineComponents  Components;
__declspec(dllexport) ColorPreset       Colors;
__declspec(dllexport) RandomGen         Random;
__declspec(dllexport) Logger            Log;
__declspec(dllexport) Timer             PhysicsTime;
__declspec(dllexport) Timer             Time;

__declspec(dllexport) Serialization     Serializer;
__declspec(dllexport) ResourceManager   Resources;
__declspec(dllexport) ScriptSystem      Scripting;
__declspec(dllexport) RenderSystem      Renderer;
__declspec(dllexport) PhysicsSystem     Physics;
__declspec(dllexport) AudioSystem       Audio;
__declspec(dllexport) InputSystem       Input;
__declspec(dllexport) MathCore          Math;
__declspec(dllexport) ActorSystem       AI;

__declspec(dllexport) ApplicationLayer      Application;
__declspec(dllexport) EngineSystemManager   Engine;



EngineSystemManager::EngineSystemManager(void) : 
    sceneMain(nullptr),
    
    doUpdateDataStream(true),
    streamSize(0)
{
}

Button* EngineSystemManager::CreateButton(unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
    Button* newButton = mButtons.Create();
    newButton->x = x;
    newButton->y = y;
    newButton->w = w;
    newButton->h = h;
    return newButton;
}

bool EngineSystemManager::DestroyButton(Button* button) {
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

