#include <GameEngineFramework/Engine/Engine.h>

extern EngineComponents     Components;
extern ColorPreset          Colors;
extern NumberGeneration     Random;
extern Logger               Log;
extern Timer                PhysicsTime;
extern Timer                Time;

extern EngineSystemManager  Engine;
extern PlatformLayer        Platform;
extern ResourceManager      Resources;
extern Serialization        Serializer;
extern ScriptSystem         Scripting;
extern RenderSystem         Renderer;
extern PhysicsSystem        Physics;
extern InputSystem          Input;
extern MathCore             Math;
extern ActorSystem          AI;



class ENGINE_API WeatherSystem {
    
public:
    
    void Update(void) {
        
        return;
        
        GameObject* weatherObject = Engine.Create<GameObject>();
        Transform* transform = weatherObject->GetComponent<Transform>();
        
        glm::vec3 playerPosition = AI.GetPlayerWorldPosition();
        
        float posx = playerPosition.x;
        float posy = playerPosition.y;
        float posz = playerPosition.z;
        
        posx += Random.Range(0, 10) - Random.Range(0, 10);
        posy += Random.Range(0, 10) - Random.Range(0, 10);
        posz += Random.Range(0, 10) - Random.Range(0, 10);
        
        transform->position = glm::vec3(posx, posy, posz);
        
        Material* weatherMaterial = Engine.Create<Material>();
        
        weatherMaterial->shader = Engine.shaders.color;
        
        weatherObject->AddComponent( Engine.CreateComponentMeshRenderer( Engine.meshes.cube, weatherMaterial ) );
        MeshRenderer* renderer = weatherObject->GetComponent<MeshRenderer>();
        
        Engine.sceneMain->AddMeshRendererToSceneRoot( renderer );
        
        return;
    }
    
    
    
    
    
    
    
};
