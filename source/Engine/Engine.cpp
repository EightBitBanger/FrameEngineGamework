#include "engine.h"


EngineComponents  Components;
ColorPreset       Colors;
RandomGen         Random;
Logger            Log;
Timer             PhysicsTime;
Timer             Time;

Serialization     Serializer;
ResourceManager   Resources;
ScriptSystem      Scripting;
RenderSystem      Renderer;
PhysicsSystem     Physics;
AudioSystem       Audio;
InputSystem       Input;
MathCore          Math;
ActorSystem       AI;

ApplicationLayer      Application;
EngineSystemManager   Engine;



EngineSystemManager::EngineSystemManager(void) {
    
    return;
}

GameObject* EngineSystemManager::CreateGameObject(void) {
    GameObject* newGameObject = mGameObjects.Create();
    mGameObjectActive.push_back(newGameObject);
    return newGameObject;
}

bool EngineSystemManager::DestroyGameObject(GameObject* gameObjectPtr) {
    assert(gameObjectPtr != nullptr);
    
    // Remove the game object from the active list
    for (std::vector<GameObject*>::iterator it = mGameObjectActive.begin(); it != mGameObjectActive.end(); ++it) {
        GameObject* thisGameObjectPtr = *it;
        if (gameObjectPtr == thisGameObjectPtr) {
            mGameObjectActive.erase(it);
            break;
        }
    }
    
    // Remove all components
    for (unsigned int i=0; i < gameObjectPtr->GetComponentCount(); i++) {
        
        Component* componentPtr = gameObjectPtr->GetComponentIndex(i);
        DestroyComponent(componentPtr);
        continue;
    }
    
    mGameObjects.Destroy(gameObjectPtr);
    return true;
}

GameObject* EngineSystemManager::CreateCameraController(glm::vec3 position, glm::vec3 scale) {
    
    GameObject* cameraController = CreateGameObject();
    cameraController->name = "camera";
    cameraController->transform.position = position;
    
    // Add a camera component
    Component* cameraComponent = CreateComponent(Components.Camera);
    Camera* cameraMain = (Camera*)cameraComponent->GetComponent();
    cameraMain->EnableMouseLook();
    cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    // Add a rigid body component
    Component* rigidBodyComponent = CreateComponent(Components.RigidBody);
    RigidBody* rigidBody = (RigidBody*)rigidBodyComponent->GetComponent();
    
    rp3d::Vector3 bodyPosition(position.x, position.y, position.z);
    rp3d::Quaternion quat = rp3d::Quaternion::identity();
    
    rp3d::Transform bodyTransform(bodyPosition, quat);
    rigidBody->setTransform(bodyTransform);
    
    // Add a scripting component
    Component* scriptComponent = CreateComponent(Components.Script);
    Script* script = (Script*)scriptComponent->GetComponent();
    script->name = "controller";
    script->gameObject = cameraController;
    script->isActive = true;
    
    cameraController->AddComponent(cameraComponent);
    cameraController->AddComponent(rigidBodyComponent);
    cameraController->AddComponent(scriptComponent);
    
    cameraController->SetAngularAxisLockFactor(0, 0, 0);
    cameraController->SetLinearDamping(3);
    cameraController->SetAngularDamping(1);
    cameraController->SetMass(10);
    
    // Collider
    BoxShape* boxShape = Physics.CreateColliderBox(scale.x, scale.y, scale.z);
    cameraController->AddColliderBox(boxShape, 0, 0, 0);
    
    return cameraController;
}

GameObject* EngineSystemManager::CreateSky(std::string meshTagName, std::string shaderTagName, Color colorLow, Color colorHigh, float biasMul) {
    
    Mesh* skyMesh = Resources.CreateMeshFromTag(meshTagName);
    if (skyMesh == nullptr) return nullptr;
    
    Material* skyMaterial = Renderer.CreateMaterial();
    
    skyMaterial->diffuse = Color(1, 1, 1);
    skyMaterial->DisableDepthTest();
    
    for (int i=0; i < skyMesh->GetNumberOfVertices(); i++) {
        Vertex vertex = skyMesh->GetVertex(i);
        
        if (vertex.y > 0) {
            vertex.r = Math.Lerp(colorHigh.r, colorLow.r, vertex.y * biasMul);
            vertex.g = Math.Lerp(colorHigh.g, colorLow.g, vertex.y * biasMul);
            vertex.b = Math.Lerp(colorHigh.b, colorLow.b, vertex.y * biasMul);
        } else {
            vertex.r = Math.Lerp(colorLow.r, colorHigh.r, vertex.y * biasMul);
            vertex.g = Math.Lerp(colorLow.g, colorHigh.g, vertex.y * biasMul);
            vertex.b = Math.Lerp(colorLow.b, colorHigh.b, vertex.y * biasMul);
        }
        
        skyMesh->SetVertex(i, vertex);
    }
    skyMesh->UploadToGPU();
    
    GameObject* skyObject = CreateGameObject();
    skyObject->name = "sky";
    skyObject->AddComponent( CreateComponentMeshRenderer(skyMesh, skyMaterial) );
    
    skyObject->transform.SetScale(10000, 2000, 10000);
    
    Shader* skyShader = Resources.CreateShaderFromTag(shaderTagName);
    if (skyShader != nullptr) 
        skyMaterial->shader = skyShader;
    
    return skyObject;
}

Component* EngineSystemManager::CreateComponentMeshRenderer(Mesh* meshPtr, Material* materialPtr) {
    Component* rendererComponent = CreateComponent(Components.MeshRenderer);
    MeshRenderer* entityRenderer = (MeshRenderer*)rendererComponent->GetComponent();
    entityRenderer->mesh = meshPtr;
    entityRenderer->material = materialPtr;
    return rendererComponent;
}

Component* EngineSystemManager::CreateComponentLight(glm::vec3 position) {
    Component* lightComponent = CreateComponent(Components.Light);
    Light* lightPoint = (Light*)lightComponent->GetComponent();
    lightPoint->transform.position = position;
    return lightComponent;
}

GameObject* EngineSystemManager::CreateAIActor(glm::vec3 position) {
    
    GameObject* newGameObject = CreateGameObject();
    newGameObject->AddComponent( CreateComponent(Components.Actor) );
    newGameObject->AddComponent( CreateComponent(Components.RigidBody) );
    newGameObject->AddComponent( CreateComponent(Components.MeshRenderer) );
    
    Mesh* meshPtr = Resources.CreateMeshFromTag("cube");
    Material* materialPtr = Renderer.CreateMaterial();
    
    // Actor component
    //Actor* ActorObject = newGameObject->GetComponent<Actor>();
    
    //materialPtr->shader = Renderer.defaultShader;
    
    // Mesh renderer component
    MeshRenderer* entityRenderer = newGameObject->GetComponent<MeshRenderer>();
    
    entityRenderer->mesh = meshPtr;
    entityRenderer->material = materialPtr;
    
    // Rigid body component
    RigidBody* rigidBody = newGameObject->GetComponent<RigidBody>();
    rigidBody->setTransform( rp3d::Transform( rp3d::Vector3(position.x, position.y, position.z) , rp3d::Quaternion::identity()) );
    
    float randomScale = Random.Range(1, 10);
    
    // Collider
    BoxShape* boxShape = Physics.CreateColliderBox(randomScale, randomScale, randomScale);
    newGameObject->AddColliderBox(boxShape, 0, 0, 0);
    newGameObject->EnableGravity();
    
    // Physics
    newGameObject->SetMass(1);
    newGameObject->SetLinearDamping(3);
    newGameObject->SetAngularDamping(1);
    newGameObject->CalculatePhysics();
    
    newGameObject->SetLinearAxisLockFactor(1, 1, 1);
    newGameObject->SetAngularAxisLockFactor(0, 1, 0);
    
    newGameObject->transform.SetScale(randomScale, randomScale, randomScale);
    
    return newGameObject;
}

GameObject* EngineSystemManager::CreateOverlayRenderer(void) {
    GameObject* overlayObject = Create<GameObject>();
    overlayObject->transform.RotateAxis(-180, Vector3(0, 1, 0));
    
    overlayObject->transform.RotateAxis(90, Vector3(0, 0, 1));
    overlayObject->transform.scale = Vector3(0.01, 0.01, 0.01);
    overlayObject->transform.position = Vector3(1.0, 0, 0);
    
    Mesh*     overlayMesh     = Create<Mesh>();
    Material* overlayMaterial = Create<Material>();
    
    //overlayMaterial->shader = Renderer.defaultShader;
    overlayMaterial->ambient = Colors.black;
    
    overlayMaterial->SetDepthFunction(MATERIAL_DEPTH_ALWAYS);
    overlayMaterial->SetTextureFiltration(MATERIAL_FILTER_NONE);
    overlayMaterial->DisableCulling();
    
    overlayObject->AddComponent( CreateComponent<MeshRenderer>(overlayMesh, overlayMaterial) );
    
    return overlayObject;
}

void EngineSystemManager::AddMeshText(Mesh* meshPtr, float xPos, float yPos, std::string text, Color textColor) {
    int spriteMapWidth = 15;
    int spriteMapHeight = 15;
    
    for (int i=0; i < text.size(); i++)
        AddMeshSubSprite(meshPtr, xPos + i, yPos, text[i], textColor, spriteMapWidth, spriteMapHeight);
    
    meshPtr->UploadToGPU();
    return;
}

void EngineSystemManager::AddMeshSubSprite(Mesh* meshPtr, float xPos, float yPos, int index, Color meshColor, int mapWidth, int mapHeight) {
    
    // Sprite atlas parameters
    float glyfWidth  = 0.03127;
    float glyfHeight = 0.0274;
    float mapStartX  = 0;
    float mapStartY  = -0.003;
    
    float spacingWidth  = 0.6;
    float spacingHeight = 0.9;
    
    
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
    
    meshPtr->AddPlain(yPos * spacingHeight, 0, -(xPos * spacingWidth), 1, 1, meshColor, glyfWidth, glyfHeight, mapStartX, mapStartY, subWidth, subHeight);
    
    return;
}

GameObject* EngineSystemManager::GetGameObject(unsigned int index) {
    if (index < mGameObjectActive.size()) 
        return mGameObjectActive[index];
    return nullptr;
}

unsigned int EngineSystemManager::GetGameObjectCount(void) {
    return mGameObjects.Size();
}

unsigned int EngineSystemManager::GetComponentCount(void) {
    return mComponents.Size();
}

void EngineSystemManager::Initiate() {
    return;
}

Component* EngineSystemManager::CreateComponent(ComponentType type) {
    void* component_object = nullptr;
    
    switch (type) {
        
        case COMPONENT_TYPE_MESH_RENDERER: {
            MeshRenderer* meshRendererPtr = Renderer.CreateMeshRenderer();
            component_object = (void*)meshRendererPtr;
            break;
        }
        case COMPONENT_TYPE_CAMERA: {
            component_object = (void*)Renderer.CreateCamera();
            break;
        }
        case COMPONENT_TYPE_LIGHT: {
            Light* lightPtr = Renderer.CreateLight();
            component_object = (void*)lightPtr;
            break;
        }
        case COMPONENT_TYPE_SCRIPT: {
            component_object = (void*)Scripting.CreateScript();
            break;
        }
        case COMPONENT_TYPE_RIGID_BODY: {
            component_object = (void*)Physics.CreateRigidBody();
            break;
        }
        case COMPONENT_TYPE_ACTOR: {
            component_object = (void*)AI.CreateActor();
            break;
        }
        
        default:
            break;
    }
    
    Component* newComponent = mComponents.Create();
    newComponent->SetComponent(type, component_object);
    return newComponent;
}

bool EngineSystemManager::DestroyComponent(Component* componentPtr) {
    assert(componentPtr != nullptr);
    
    ComponentType componentType = componentPtr->GetType();
    
    switch (componentType) {
        
        case COMPONENT_TYPE_MESH_RENDERER: {
            MeshRenderer* componentEntityRenderer = (MeshRenderer*)componentPtr->GetComponent();
            Renderer.DestroyMeshRenderer(componentEntityRenderer);
            break;
        }
        case COMPONENT_TYPE_CAMERA: {
            Camera* componentCamera = (Camera*)componentPtr->GetComponent();
            Renderer.DestroyCamera(componentCamera);
            break;
        }
        case COMPONENT_TYPE_LIGHT: {
            Light* componentLight = (Light*)componentPtr->GetComponent();
            Renderer.DestroyLight(componentLight);
            break;
        }
        case COMPONENT_TYPE_SCRIPT: {
            Script* componentScript = (Script*)componentPtr->GetComponent();
            Scripting.DestroyScript(componentScript);
            break;
        }
        case COMPONENT_TYPE_RIGID_BODY: {
            RigidBody* componentRigidBody = (RigidBody*)componentPtr->GetComponent();
            Physics.DestroyRigidBody(componentRigidBody);
            break;
        }
        case COMPONENT_TYPE_ACTOR: {
            Actor* actorObject = (Actor*)componentPtr->GetComponent();
            AI.DestroyActor(actorObject);
            break;
        }
        
        default:
            return false;
    }
    mComponents.Destroy(componentPtr);
    return true;
}

void EngineSystemManager::Update(void) {
    
    // Run through the game objects
    for (int i=0; i < mGameObjects.Size(); i++ ) {
        
        GameObject* objectPtr = mGameObjects[i];
        
        if (!objectPtr->isActive) 
            continue;
        
        // Current transform
        Transform currentTransform;
        currentTransform.position    = objectPtr->transform.position;
        currentTransform.orientation = objectPtr->transform.orientation;
        currentTransform.scale       = objectPtr->transform.scale;
        
        // Calculate parent transforms
        GameObject* parent = objectPtr->parent;
        
        // Roll over the parent matrix transform chain
        while (parent != nullptr) {
            
            currentTransform.position    += parent->transform.position;
            currentTransform.scale       *= parent->transform.scale;
            currentTransform.orientation *= parent->transform.orientation;
            
            parent = parent->parent;
        }
        
        glm::mat4 translation = glm::translate(glm::mat4(1), currentTransform.position);
        glm::mat4 rotation    = glm::toMat4(currentTransform.orientation);
        glm::mat4 scale       = glm::scale(glm::mat4(1), currentTransform.scale);
        
        currentTransform.matrix = translation * rotation * scale;
        
        
        //
        // Sync with the rigid body
        //
        
        RigidBody* componentRigidBody = objectPtr->GetComponent<RigidBody>();
        if (componentRigidBody != nullptr) {
            
            // Use the rigid body as the source transform
            rp3d::Transform bodyTransform = componentRigidBody->getTransform();
            rp3d::Vector3 bodyPosition = bodyTransform.getPosition();
            rp3d::Quaternion quaterion = bodyTransform.getOrientation();
            
            currentTransform.position.x = bodyPosition.x;
            currentTransform.position.y = bodyPosition.y;
            currentTransform.position.z = bodyPosition.z;
            
            currentTransform.orientation.w = quaterion.w;
            currentTransform.orientation.x = quaterion.x;
            currentTransform.orientation.y = quaterion.y;
            currentTransform.orientation.z = quaterion.z;
            
            // Source matrix
            bodyTransform.getOpenGLMatrix(&currentTransform.matrix[0][0]);
            
            // Update the game object transform
            objectPtr->transform.position    = currentTransform.position;
            objectPtr->transform.orientation = currentTransform.orientation;
            
            // Scale the transform
            currentTransform.matrix = glm::scale(currentTransform.matrix, objectPtr->transform.scale);
        }
        
        MeshRenderer* componentMeshRenderer = objectPtr->GetComponent<MeshRenderer>();
        if (componentMeshRenderer != nullptr) {
            componentMeshRenderer->transform.position    = currentTransform.position;
            componentMeshRenderer->transform.orientation = currentTransform.orientation;
            componentMeshRenderer->transform.scale       = currentTransform.scale;
            componentMeshRenderer->transform.matrix      = currentTransform.matrix;
        }
        
        Light* componentLight = objectPtr->GetComponent<Light>();
        if (componentLight != nullptr) {
            componentLight->transform.position    = currentTransform.position;
            componentLight->transform.orientation = currentTransform.orientation;
        }
        
        Camera* componentCamera = objectPtr->GetComponent<Camera>();
        if (componentCamera != nullptr) {
            componentCamera->transform.position = currentTransform.position;
            if (!componentCamera->useMouseLook) 
                componentCamera->transform.orientation = currentTransform.orientation;
        }
        
        continue;
    }
    
    return;
}

void EngineSystemManager::Shutdown(void) {
    
    while (GetGameObjectCount() > 0) {
        DestroyGameObject( GetGameObject(0) );
    }
    
    assert(GetGameObjectCount() == 0);
    assert(mComponents.Size() == 0);
    
    return;
}

