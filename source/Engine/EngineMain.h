#ifndef GAME_ENGINE_FRAMEWORK_FRONTEND
#define GAME_ENGINE_FRAMEWORK_FRONTEND

class EngineMainAPI {
    
    Scene* sceneMain;
    
public:
    
    void Initiate(void);
    
    Entity* CreateGameObject(float x, float y, float z, std::string meshName="", std::string materialName="", std::string colliderName="");
    void    DestroyGameObject(Entity* entityPtr);
    
};

#endif
