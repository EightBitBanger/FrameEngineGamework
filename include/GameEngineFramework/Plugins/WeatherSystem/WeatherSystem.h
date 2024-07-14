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
    
    GameObject* sunObject;
    Light* sun;
    
    
    
    
    
    
    void Initiate(void) {
        
        sunObject = Engine.Create<GameObject>();
        sunObject->AddComponent( Engine.CreateComponent<Light>() );
        
        Transform* transform = sunObject->GetComponent<Transform>();
        transform->RotateEuler(0.0f, -1.0f, 2.0f);
        
        sun = sunObject->GetComponent<Light>();
        
        sun->type       = LIGHT_TYPE_DIRECTIONAL;
        sun->intensity  = 0.7f;
        sun->color      = Colors.white;
        
        Engine.sceneMain->AddLightToSceneRoot( sun );
        
    }
    
    
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
