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
    
    SetCursorPos(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
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

GameObject* EngineSystemManager::CreateSky(std::string meshTagName, Color colorLow, Color colorHigh, float biasMul) {
    
    Mesh* skyMesh = Resources.CreateMeshFromTag(meshTagName);
    if (skyMesh == nullptr) return nullptr;
    
    Material* skyMaterial = Renderer.CreateMaterial();
    
    skyMaterial->diffuse = Color(1, 1, 1);
    skyMaterial->DisableDepthTest();
    skyMaterial->shader = shaders.color;
    
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
    
    materialPtr->shader = shaders.color;
    
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
    overlayObject->transform.RotateAxis( -90, Vector3(0, 0, 1));
    
    Mesh*     overlayMesh     = Create<Mesh>();
    Material* overlayMaterial = Create<Material>();
    
    overlayMaterial->shader = shaders.color;
    overlayMaterial->ambient = Colors.black;
    
    overlayMaterial->SetDepthFunction(MATERIAL_DEPTH_ALWAYS);
    overlayMaterial->SetTextureFiltration(MATERIAL_FILTER_NONE);
    overlayMaterial->DisableCulling();
    
    overlayObject->AddComponent( CreateComponent<MeshRenderer>(overlayMesh, overlayMaterial) );
    
    return overlayObject;
}

GameObject* EngineSystemManager::CreateOverlayTextRenderer(std::string text, unsigned int textSize, Color color, std::string materialTag) {
    
    GameObject* overlayObject = CreateOverlayRenderer();
    overlayObject->AddComponent( CreateComponent<Text>() );
    
    overlayObject->GetComponent<Text>()->text  = text;
    overlayObject->GetComponent<Text>()->color = color;
    
    overlayObject->GetComponent<Text>()->canvas.width  = Renderer.displayCenter.x;
    overlayObject->GetComponent<Text>()->canvas.height = Renderer.displayCenter.y;
    
    overlayObject->transform.scale = Vector3(textSize, 1, textSize);
    
    MeshRenderer* overlayRenderer = overlayObject->GetComponent<MeshRenderer>();
    
    // Sprite sheet material
    Destroy<Material>( overlayRenderer->material );
    overlayRenderer->material = Resources.CreateMaterialFromTag( materialTag );
    overlayRenderer->material->ambient  = Colors.black;
    overlayRenderer->material->shader = shaders.UI;
    
    overlayRenderer->material->SetDepthFunction(MATERIAL_DEPTH_ALWAYS);
    overlayRenderer->material->SetTextureFiltration(MATERIAL_FILTER_NONE);
    
    overlayRenderer->material->DisableCulling();
    return overlayObject;
}

void EngineSystemManager::AddMeshText(GameObject* overlayObject, float xPos, float yPos, std::string text, Color textColor) {
    
    Mesh* meshPtr = overlayObject->GetComponent<MeshRenderer>()->mesh;
    if (meshPtr == nullptr) 
        return;
    
    for (int i=0; i < text.size(); i++)
        AddMeshSubSprite(overlayObject, xPos + i, yPos, text[i], textColor);
    
    meshPtr->UploadToGPU();
    return;
}

void EngineSystemManager::AddMeshSubSprite(GameObject* overlayObject, float xPos, float yPos, int index, Color meshColor) {
    
    Mesh* meshPtr = overlayObject->GetComponent<MeshRenderer>()->mesh;
    if (meshPtr == nullptr) 
        return;
    
    Text* textPtr = overlayObject->GetComponent<Text>();
    if (textPtr == nullptr) 
        return;
    
    // Sprite atlas parameters
    float spriteStartX  = textPtr->sprite.spriteStartX;
    float spriteStartY  = textPtr->sprite.spriteStartY;
    float spriteWidth   = textPtr->sprite.spriteWidth;
    float spriteHeight  = textPtr->sprite.spriteHeight;
    
    float spacingWidth  = textPtr->spacingWidth;
    float spacingHeight = textPtr->spacingHeight;
    
    int mapWidth  = textPtr->sprite.textureWidth;
    int mapHeight = textPtr->sprite.textureHeight;
    
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
                      1, 1, 
                      meshColor, 
                      spriteWidth, spriteHeight, 
                      spriteStartX, spriteStartY, 
                      subWidth, subHeight);
    
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
    
    shaders.texture       = Resources.CreateShaderFromTag("texture");
    shaders.textureUnlit  = Resources.CreateShaderFromTag("textureUnlit");
    shaders.color         = Resources.CreateShaderFromTag("color");
    shaders.UI            = Resources.CreateShaderFromTag("UI");
    
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
        case COMPONENT_TYPE_TEXT: {
            component_object = (void*)mTextObjects.Create();
            Text* textComponent = (Text*)component_object;
            textComponent->canvas.width  = Renderer.displaySize.x;
            textComponent->canvas.height = Renderer.displaySize.y;
            break;
        }
        
        default:
            return nullptr;
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
        case COMPONENT_TYPE_TEXT: {
            Text* textObject = (Text*)componentPtr->GetComponent();
            mTextObjects.Destroy(textObject);
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
        
        GameObject* gameObject = mGameObjects[i];
        
        if (!gameObject->isActive) 
            continue;
        
        // Current transform
        Transform currentTransform;
        currentTransform.position    = gameObject->transform.position;
        currentTransform.orientation = gameObject->transform.orientation;
        currentTransform.scale       = gameObject->transform.scale;
        
        // Calculate parent transforms
        GameObject* parent = gameObject->parent;
        
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
        
        RigidBody* componentRigidBody = gameObject->GetComponent<RigidBody>();
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
            gameObject->transform.position    = currentTransform.position;
            gameObject->transform.orientation = currentTransform.orientation;
            
            // Scale the transform
            currentTransform.matrix = glm::scale(currentTransform.matrix, gameObject->transform.scale);
        }
        
        MeshRenderer* componentMeshRenderer = gameObject->GetComponent<MeshRenderer>();
        if (componentMeshRenderer != nullptr) {
            componentMeshRenderer->transform.position    = currentTransform.position;
            componentMeshRenderer->transform.orientation = currentTransform.orientation;
            componentMeshRenderer->transform.scale       = currentTransform.scale;
            componentMeshRenderer->transform.matrix      = currentTransform.matrix;
        }
        
        Light* componentLight = gameObject->GetComponent<Light>();
        if (componentLight != nullptr) {
            componentLight->transform.position    = currentTransform.position;
            componentLight->transform.orientation = currentTransform.orientation;
        }
        
        Camera* componentCamera = gameObject->GetComponent<Camera>();
        if (componentCamera != nullptr) {
            
            // Update mouse looking
            if (componentCamera->useMouseLook) {
                
                POINT cursorPos;
                GetCursorPos(&cursorPos);
                SetCursorPos(Renderer.displayCenter.x, Renderer.displayCenter.y);
                
                //glm::vec2 cursorPos(0);
                //cursorPos = Input.CursorGetPosition();
                //int mouseX = cursorPos.x;
                //int mouseY = cursorPos.y;
                //Input.CursorSetPosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
                
                float MouseDiffX = (cursorPos.x - Renderer.displayCenter.x) * componentCamera->MouseSensitivityYaw;
                float MouseDiffY = (cursorPos.y - Renderer.displayCenter.y) * componentCamera->MouseSensitivityPitch;
                
                componentCamera->lookAngle.x += MouseDiffX * 0.01;
                componentCamera->lookAngle.y -= MouseDiffY * 0.01;
                
                // Yaw limit
                if (componentCamera->lookAngle.x >= 0.109655) {componentCamera->lookAngle.x -= 0.109655;}
                if (componentCamera->lookAngle.x <= 0.109655) {componentCamera->lookAngle.x += 0.109655;}
                
                // Pitch limit
                if (componentCamera->lookAngle.y >  0.0274f) componentCamera->lookAngle.y =  0.0274f;
                if (componentCamera->lookAngle.y < -0.0274f) componentCamera->lookAngle.y = -0.0274f;
                
                componentCamera->transform.orientation.x = componentCamera->lookAngle.x;
                componentCamera->transform.orientation.y = componentCamera->lookAngle.y;
                
            }
            
            componentCamera->transform.position = currentTransform.position;
            
            if (!componentCamera->useMouseLook) 
                componentCamera->transform.orientation = currentTransform.orientation;
            
        }
        
        Text* componentText = gameObject->GetComponent<Text>();
        if (componentText != nullptr) {
            if (componentMeshRenderer != nullptr) {
                
                if (componentText->canvas.anchorRight) 
                    gameObject->transform.position.z = Renderer.viewport.w - componentText->canvas.width;
                
                if (componentText->canvas.anchorBottom) 
                    gameObject->transform.position.y = Renderer.viewport.h - componentText->canvas.height;
                
                componentMeshRenderer->mesh->ClearSubMeshes();
                Engine.AddMeshText(gameObject, 
                                    componentText->canvas.position.x, 
                                   -componentText->canvas.position.y, 
                                    componentText->text, 
                                    componentText->color);
                
                componentMeshRenderer->mesh->UploadToGPU();
            }
            
        }
        
        continue;
    }
    
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
    
    assert(GetGameObjectCount() == 0);
    assert(mComponents.Size() == 0);
    
    return;
}

