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



EngineSystemManager::EngineSystemManager(void) : 
    
    mSceneMain(nullptr) 
{
    
    // Preallocate some buffer space
    mGameObjectActive.reserve(64);
    
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
    Renderer.cameraMain = cameraMain;
    cameraMain->EnableMouseLook();
    cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    // Add a rigid body component
    Component* rigidBodyComponent = CreateComponent(Components.RigidBody);
    rp3d::RigidBody* rigidBody = (rp3d::RigidBody*)rigidBodyComponent->GetComponent();
    
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
    cameraController->SetMass(40);
    
    // Collider
    BoxShape* boxShape = Physics.CreateColliderBox(1, 8, 1);
    cameraController->AddColliderBox(boxShape, 0, 0, 0);
    cameraController->CalculatePhysics();
    cameraController->EnableGravity(false);
    cameraController->SetLinearDamping(3);
    
    return cameraController;
}

GameObject* EngineSystemManager::CreateSky(std::string meshTagName, std::string shaderTagName, Color colorLow, Color colorHigh, float biasMul) {
    
    Mesh* skyMesh = Resources.CreateMeshFromTag(meshTagName);
    if (skyMesh == nullptr) return nullptr;
    
    Material* skyMaterial = Renderer.CreateMaterial();
    
    skyMaterial->diffuse = Color(1, 1, 1);
    
    for (int i=0; i < skyMesh->GetNumberOfVertices(); i++) {
        Vertex vertex = skyMesh->GetVertex(i);
        
        vertex.r = Math.Lerp(colorLow.r, colorHigh.r, vertex.y * biasMul);
        vertex.g = Math.Lerp(colorLow.g, colorHigh.g, vertex.y * biasMul);
        vertex.b = Math.Lerp(colorLow.b, colorHigh.b, vertex.y * biasMul);
        
        skyMesh->SetVertex(i, vertex);
    }
    skyMesh->UpdateMesh();
    
    GameObject* skyObject = CreateGameObject();
    skyObject->name = "sky";
    Component* skyComponent = CreateComponentMeshRenderer(skyMesh, skyMaterial);
    skyObject->AddComponent(skyComponent);
    
    skyObject->transform.SetScale(10000, 2000, 10000);
    
    Shader* skyShader = Resources.CreateShaderFromTag(shaderTagName);
    if (skyShader != nullptr) 
        skyMaterial->SetShader(skyShader);
    
    return skyObject;
}

Component* EngineSystemManager::CreateComponentMeshRenderer(Mesh* meshPtr, Material* materialPtr) {
    Component* rendererComponent = CreateComponent(Components.MeshRenderer);
    MeshRenderer* entityRenderer = (MeshRenderer*)rendererComponent->GetComponent();
    entityRenderer->AttachMesh(meshPtr);
    entityRenderer->AttachMaterial(materialPtr);
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
    
    // Actor component
    Actor* ActorObject = newGameObject->GetComponent<Actor>();
    
    // Mesh renderer component
    MeshRenderer* entityRenderer = newGameObject->GetComponent<MeshRenderer>();
    
    Mesh* meshPtr = Resources.CreateMeshFromTag("cube");
    Material* materialPtr = Renderer.CreateMaterial();
    
    Color newColor(0, 0, 0);
    meshPtr->ChangeSubMeshColor(0, newColor);
    
    materialPtr->SetShader(Renderer.defaultShader);
    materialPtr->diffuse = Colors.MakeRandom();
    
    entityRenderer->AttachMesh(meshPtr);
    entityRenderer->AttachMaterial(materialPtr);
    
    // Rigid body component
    rp3d::RigidBody* rigidBody = newGameObject->GetComponent<rp3d::RigidBody>();
    rigidBody->setTransform( rp3d::Transform( rp3d::Vector3(position.x, position.y, position.z) , rp3d::Quaternion::identity()) );
    
    float randomScale = Random.Range(1, 10);
    
    // Collider
    BoxShape* boxShape = Physics.CreateColliderBox(randomScale, randomScale, randomScale);
    newGameObject->AddColliderBox(boxShape, 0, 0, 0);
    newGameObject->EnableGravity(true);
    
    newGameObject->SetMass(1);
    newGameObject->SetLinearDamping(0.1);
    newGameObject->SetAngularDamping(0.1);
    //newGameObject->CalculatePhysics();
    
    //newGameObject->SetLinearAxisLockFactor(0, 0, 0);
    newGameObject->SetAngularAxisLockFactor(0, 1, 0);
    
    newGameObject->transform.SetScale(randomScale, randomScale, randomScale);
    
    return newGameObject;
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
    mSceneMain = Renderer.CreateScene();
    Renderer.AddSceneToRenderQueue(mSceneMain);
    return;
}

Component* EngineSystemManager::CreateComponent(ComponentType type) {
    void* component_object = nullptr;
    
    switch (type) {
        
        case COMPONENT_TYPE_MESH_RENDERER: {
            MeshRenderer* meshRendererPtr = Renderer.CreateMeshRenderer();
            mSceneMain->AddMeshRendererToSceneRoot(meshRendererPtr);
            component_object = (void*)meshRendererPtr;
            break;
        }
        case COMPONENT_TYPE_CAMERA: {
            component_object = (void*)Renderer.CreateCamera();
            break;
        }
        case COMPONENT_TYPE_LIGHT: {
            Light* lightPtr = Renderer.CreateLight();
            mSceneMain->AddLightToSceneRoot(lightPtr);
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
            mSceneMain->RemoveMeshRendererFromSceneRoot(componentEntityRenderer);
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
            mSceneMain->RemoveLightFromSceneRoot(componentLight);
            Renderer.DestroyLight(componentLight);
            break;
        }
        case COMPONENT_TYPE_SCRIPT: {
            Script* componentScript = (Script*)componentPtr->GetComponent();
            Scripting.DestroyScript(componentScript);
            break;
        }
        case COMPONENT_TYPE_RIGID_BODY: {
            rp3d::RigidBody* componentRigidBody = (rp3d::RigidBody*)componentPtr->GetComponent();
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
        if (objectPtr->parent != nullptr) {
            
            GameObject* parent = objectPtr->parent;
            
            // Roll over the parent matrix transform chain
            for (unsigned int i=0; i < 100; i++) {
                
                currentTransform.position    += parent->transform.position;
                currentTransform.scale       *= parent->transform.scale;
                currentTransform.orientation *= parent->transform.orientation;
                
                parent = parent->parent;
                if (parent == nullptr) 
                    break;
            }
            
        }
        
        glm::mat4 translation = glm::translate(glm::mat4(1), currentTransform.position);
        glm::mat4 rotation    = glm::toMat4(currentTransform.orientation);
        glm::mat4 scale       = glm::scale(glm::mat4(1), currentTransform.scale);
        
        currentTransform.matrix = translation * rotation * scale;
        
        
        //
        // Sync with the rigid body
        //
        
        rp3d::RigidBody* componentRigidBody = objectPtr->GetComponent<rp3d::RigidBody>();
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
