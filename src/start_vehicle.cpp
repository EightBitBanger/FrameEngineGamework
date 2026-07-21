#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/Plugins/plugins.h>

float playerHeight = 1.1f;
float playerRadius = 0.087f;

std::vector<GameObject*> vehicles;

void Start() {
    // Key bindings
    Input.BindKeyPressToFunction(VK_F4, keyBindF4);
    Input.BindKeyPressToFunction(VK_F11, keyBindFullscreen);
    Input.BindKeyPressToFunction(VK_ESCAPE, keyBindEscape);
    
    // Load command functions
    Engine.console.Enable();
    
    Engine.console.RegisterCommand("summon",  FuncSummon);
    Engine.console.RegisterCommand("list",    FuncList);
    
    Engine.console.RegisterCommand("save",    FuncSave);
    Engine.console.RegisterCommand("load",    FuncLoad);
    Engine.console.RegisterCommand("remove",  FuncRemove);
    
    Engine.console.RegisterCommand("clear",   FuncClear);
    Engine.console.RegisterCommand("seed",    FuncSeed);
    
    Engine.console.RegisterCommand("time",    FuncTime);
    Engine.console.RegisterCommand("weather", FuncWeather);
    
    /*
    // Audio test
    Sound* sound = Audio.CreateSound();
    sound->sample = Audio.CreateAudioSample();
    
    VocalParameters param;
    param.seconds        = 0.1;
    param.pitchStartHz   = 2500.0f;
    param.pitchEndHz     = 1000.0f;
    param.attackSeconds  = 0.005f;
    param.holdSeconds    = 0.03f;
    param.releaseSeconds = 0.1f;
    param.brightness     = 0.8f;
    param.growl          = 0.15f;
    param.noiseAmount    = 0.05f;
    
    Audio.Presets.RenderAnimalVocal(sound->sample, param);
    
    Playback* playback = Audio.Play(sound);
    */
    
    // Initiate platform
    //Platform.EnableVSync();
    Platform.ShowMouseCursor();
    Platform.isPaused = true;
    
    //Engine.EnablePhysicsDebugRenderer();
    
    // Event callback
    Platform.EventCallbackLoseFocus = EventLostFocus;
    Platform.EventCallbackResize = EventOnResize;
    
    // Client version
    std::string version = "0.0.1";
    
    // User plug-in initiation
    Particle.Initiate();
    Weather.Initiate();
    Vehicular.Initiate();
    
    // Create a camera controller
    
    // Create a new camera controller object and 
    // assign it as the engines main camera.
    Engine.cameraController = Engine.CreateCameraController( glm::vec3(136.0f, 2.7f, 104.0f) );
    
    // Attach the sky to the player
    Weather.SetPlayerObject( Engine.cameraController );
    
    // Assign the camera controller's camera for rendering the main scene.
    Engine.sceneMain->camera = Engine.cameraController->GetComponent<Camera>();
    
    // Disable mouse look on startup
    Engine.sceneMain->camera->DisableMouseLook();
    
    // Third person camera angle
    Engine.sceneMain->camera->EnableThirdPersonLook();
    Engine.sceneMain->camera->centerPosition = glm::vec3(0.0f, -1.3f, 0.0f);
    Engine.sceneMain->camera->centerStandOff = 4.5f;
    Engine.sceneMain->camera->mouseLimitPitchMax = 80.0f;
    Engine.sceneMain->camera->mouseLimitPitchMin = -60.0f;
    
    // Create a box collider for collisions
    glm::vec3 playerScale(0.1f, playerHeight, 0.1f);
    
    rp3d::CapsuleShape* playerShape = Physics.CreateColliderCapsule(playerRadius, playerHeight);
    rp3d::Collider* playerCollider = Engine.cameraController->AddColliderCapsule(playerShape, 0.0f, 0.0f, 0.0f);
    rp3d::Material& physicsMaterial = playerCollider->getMaterial();
    physicsMaterial.setFrictionCoefficient(0.1f);
    Engine.cameraController->EnableGravity();
    
    Physics.SetWorldGravity(0.0f, -9.81f, 0.0f);
    
    Engine.cameraController->AddComponent(Engine.CreateComponent<MeshRenderer>());
    MeshRenderer* playerRenderer = Engine.cameraController->GetComponent<MeshRenderer>();
    Transform* transform = Engine.cameraController->GetComponent<Transform>();
    transform->SetScale(playerScale);
    
    playerRenderer->mesh = Engine.meshes.cube;
    playerRenderer->material = Engine.Create<Material>();
    playerRenderer->material->shader = Engine.shaders.color;
    playerRenderer->material->ambient = Colors.black;
    playerRenderer->material->diffuse = Colors.dkgray;
    
    Engine.sceneMain->AddMeshRendererToSceneRoot(playerRenderer);
    
    // Weather system
    
    Material* worldMaterial  = Engine.Create<Material>();
    Material* staticMaterial = Engine.Create<Material>();
    Material* waterMaterial  = Engine.Create<Material>();
    
    Weather.SetPlayerObject(Engine.cameraController);
    Weather.SetWorldMaterial(worldMaterial);
    Weather.SetStaticMaterial(staticMaterial);
    Weather.SetWaterMaterial(waterMaterial);
    
    // Lighting levels
    Weather.SetWorldLightLevel(0.1f, 0.4f);
    
    // Loading directories
    const std::string dataDir    = "data/";
    const std::string mapsDir    = dataDir + "maps/";
    const std::string genericDir = dataDir + "generic/";
    const std::string texDir     = dataDir + "texture/";
    
    std::vector<std::string> mapList      = fs.DirectoryGetList(mapsDir);
    std::vector<std::string> genericList  = fs.DirectoryGetList(genericDir);
    std::vector<std::string> textureList  = fs.DirectoryGetList(texDir);
    std::vector<std::string> itemList     = fs.DirectoryGetList(dataDir);
    std::vector<std::string> colliderList = fs.DirectoryGetList(dataDir);
    
    for (unsigned int i=0; i < mapList.size(); i++) {
        size_t pos = mapList[i].find(".obj");
        if (pos == std::string::npos) 
            continue;
        std::string name = mapList[i];
        name.erase(pos, 4);
        Resources.LoadWaveFront(mapsDir + mapList[i], name, true);
    }
    for (unsigned int i=0; i < genericList.size(); i++) {
        size_t pos = genericList[i].find(".obj");
        if (pos == std::string::npos) 
            continue;
        std::string name = genericList[i];
        name.erase(pos, 4);
        Resources.LoadWaveFront(genericDir + genericList[i], name, true);
    }
    
    for (unsigned int i=0; i < textureList.size(); i++) {
        size_t pos = textureList[i].find(".png");
        if (pos == std::string::npos) 
            continue;
        std::string name = textureList[i];
        name.erase(pos, 4);
        Resources.LoadTexture(texDir + textureList[i], name, true);
    }
    
    //
    // Generate material texture sets
    struct TextureGroup {
        glm::vec2 uv_position;
        glm::vec2 uv_scale;
    };
    
    std::unordered_map<std::string, TextureGroup> textureSet;
    std::vector<std::pair<std::string, Material*>> materials;
    
    for (unsigned int i = 0; i < textureList.size(); i++) {
        size_t pos = textureList[i].find(".tex");
        if (pos == std::string::npos)
            continue;
        
        std::string name = textureList[i];
        name.erase(pos, 4);
        
        FileLoader fileLoader(texDir + textureList[i]);
        
        unsigned int numberOfLines = fileLoader.rawData.size();
        for (unsigned int l = 0; l < numberOfLines; l++) {
            std::vector<std::string> params = String.Explode(fileLoader.rawData[l], ' ');
            if (params.size() < 3)
                continue;
            
            // Define per-material UV settings
            if (params[0] == "material") {
                std::string matName = params[1];
                
                float posx        = String.ToFloat(params[2]);
                float posy        = String.ToFloat(params[3]);
                float widthScale  = String.ToFloat(params[4]);
                float heightScale = String.ToFloat(params[5]);
                
                TextureGroup group;
                group.uv_position = { posx, posy };
                group.uv_scale    = { widthScale, heightScale };
                
                textureSet[matName] = group;
                continue;
            }
            
            // Create a material set
            if (params[0] == "set") {
                Material* material = Engine.Create<Material>();
                
                for (unsigned int p = 2; p < params.size(); p++) {
                    const std::string& tag = params[p];
                    
                    Resources.LoadMaterialFromTag(tag, material, MATERIAL_FILTER_TRILINEAR, MATERIAL_FILTER_LINEAR);
                    Texture* texture = material->GetTexture(p - 2);
                    
                    std::unordered_map<std::string, TextureGroup>::iterator it = textureSet.find(tag);
                    if (it != textureSet.end()) {
                        const TextureGroup& group = it->second;
                        texture->SetScaleUV(group.uv_scale);
                        texture->SetPositionUV(group.uv_position);
                    }
                }
                
                materials.push_back({ params[1], material });
                continue;
            }
        }
    }
    
    //
    // Collision shape
    struct CollisionShape {
        std::string shape;
        std::string type;
        
        glm::vec3 position;
        glm::vec3 scale;
    };
    
    std::unordered_map<std::string, CollisionShape> collisionShapes;
    
    for (unsigned int i=0; i < colliderList.size(); i++) {
        size_t pos = colliderList[i].find(".coll");
        if (pos == std::string::npos) 
            continue;
        std::string name = colliderList[i];
        name.erase(pos, 5);
        FileLoader fileLoader("data/" + colliderList[i]);
        
        unsigned int numberOfLines = fileLoader.rawData.size();
        for (unsigned int l=0; l < numberOfLines; l++) {
            std::vector<std::string> params = String.Explode(fileLoader.rawData[l], ' ');
            if (params.size() < 8) 
                continue;
            
            glm::vec3 position;
            position.x = String.ToFloat(params[1]);
            position.y = String.ToFloat(params[2]);
            position.z = String.ToFloat(params[3]);
            
            glm::vec3 scale;
            scale.x = String.ToFloat(params[4]);
            scale.y = String.ToFloat(params[5]);
            scale.z = String.ToFloat(params[6]);
            
            CollisionShape shape;
            shape.shape     = params[0];
            shape.type      = params[7];
            shape.position  = position;
            shape.scale     = scale;
            
            collisionShapes[name] = shape;
        }
        
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    // Vehicle testing
    
    glm::vec3 spawnPoint   = {138.0f, 2.7f, 114.0f};
    
    // Weight distribution
    float     massKg       = 9800.0f;
    glm::vec3 dimensions   = {0.65f, 0.24f, 1.4f};
    glm::vec3 centerOfMass = {0.0f, -0.17f, -0.2f};
    
    // Wheel dimensions
    float wheelTrack     =  0.6f;
    float wheelBase      =  0.94f;
    float wheelRadius    =  0.48f;
    float wheelCamber    = -2.0f;
    float wheelHeight    = -0.19f;
    
    glm::vec3 wheelScale =  {0.9f, 0.6f, 0.9f};
    
    // Suspension
    float suspensionHeight  = 0.03f;
    float suspensionMin     = 0.01f;
    float suspensionMax     = 0.07f;
    
    float springStiffness   = 70000.0f;
    float springCompression = 14000.0f;
    float springRelaxation  = 20000.0f;
    
    // Steering
    float steeringRadius       = 0.4f;
    float steeringCurveHigh    = 400.0f;
    float steeringCurveMin     = 20.0f;
    float steeringSmoothing    = 8000.0f;
    
    // Engine & breaking
    float maxEnginePower       = 67000.0f;
    float maxBreakPower        = 74000.0f;
    float breakBias            = 0.65f;
    
    // Traction
    float tractionLoss         = 17.0f;
    float tractionMul          = 4.3f;
    
    
    
    
    GameObject* carObject = Engine.Create<GameObject>();
    vehicles.push_back(carObject);
    
    carObject->AddComponent( Engine.CreateComponent<Vehicle>() );
    
    Vehicle* car = carObject->GetComponent<Vehicle>();
    Vehicular.AddVehicleToActiveList(car);
    
    rp3d::RigidBody* chassisRigidBody = Physics.CreateRigidBody(spawnPoint);
    chassisRigidBody->setType(rp3d::BodyType::DYNAMIC);
    chassisRigidBody->enableGravity(true);
    
    rp3d::BoxShape* vehicleShape = Physics.CreateColliderBox(dimensions);
    rp3d::Collider* chassisCollider = chassisRigidBody->addCollider(vehicleShape, rp3d::Transform::identity());
    
    rp3d::Material& matCollider = chassisCollider->getMaterial();
    matCollider.setBounciness(0.4f);
    
    VehicleSystem::SetupVehicleChassis(chassisRigidBody, massKg);
    chassisRigidBody->setLinearDamping(1.4f);
    chassisRigidBody->setAngularDamping(0.08f);
    
    rp3d::Vector3 rpCenterOfMass = chassisRigidBody->getLocalCenterOfMass();
    rpCenterOfMass.x += centerOfMass.x;
    rpCenterOfMass.y += centerOfMass.y;
    rpCenterOfMass.z += centerOfMass.z;
    chassisRigidBody->setLocalCenterOfMass(rpCenterOfMass);
    
    MeshRenderer* chassisRenderer = Renderer.CreateMeshRenderer();
    
    chassisRenderer->mesh = Renderer.CreateMesh();
    
    MeshTag* chassisTag = Resources.FindMeshTag("vehicle");
    chassisRenderer->mesh->AddSubMesh(0.0f, 0.0f, 0.0f, chassisTag->subMeshes[0], false);
    chassisRenderer->mesh->Load();
    
    
    
    chassisRenderer->material = Renderer.CreateMaterial();
    chassisRenderer->material->shader = Engine.shaders.color;
    Engine.sceneMain->AddMeshRendererToSceneRoot(chassisRenderer);
    
    chassisRenderer->material->EnableCulling();
    chassisRenderer->SetBoundingBox({-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f});
    
    chassisRenderer->material->diffuse = Colors.dkgray;
    chassisRenderer->material->ambient = Colors.dkgray;
    
    chassisRenderer->isActive = true;
    chassisRenderer->transform.scale = glm::vec3(1.0f);
    chassisRenderer->transform.UpdateMatrix();
    
    car->SetRigidBody(chassisRigidBody);
    car->AddMeshRenderer(chassisRenderer);
    
    
    
    // Transfer the camera to the vehicle
    
    MeshRenderer* controllerRenderer = Engine.cameraController->GetComponent<MeshRenderer>();
    RigidBody* controllerBody = Engine.cameraController->GetComponent<RigidBody>();
    Engine.sceneMain->camera->centerStandOff = 5.0f;
    controllerBody->setIsActive(false);
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot(controllerRenderer, RENDER_QUEUE_GEOMETRY);
    
    car->SetPlayerController(Engine.cameraController);
    
    
    
    VehicleWheel wheel;
    wheel.radius = wheelRadius;
    wheel.mountHeight = wheelHeight;
    
    Mesh*     wheelMesh     = Renderer.CreateMesh();
    Material* wheelMaterial = Renderer.CreateMaterial();
    
    Resources.LoadMaterialFromTag("wheel", wheelMaterial, MATERIAL_FILTER_TRILINEAR, MATERIAL_FILTER_LINEAR);
    wheelMaterial->shader = Engine.shaders.texture;
    
    MeshTag* meshTag = Resources.FindMeshTag("wheel");
    wheelMesh->AddSubMesh(0.0f, 0.0f, 0.0f, meshTag->subMeshes[0], false);
    wheelMesh->Load();
    
    // Suspension and grip
    
    wheel.suspensionRestLength = suspensionHeight;
    wheel.suspensionMinLength  = suspensionMin;
    wheel.suspensionMaxLength  = suspensionMax;
    wheel.suspensionRestLength = glm::clamp(wheel.suspensionRestLength, wheel.suspensionMinLength, wheel.suspensionMaxLength);
    
    wheel.suspensionStiffness = springStiffness;
    wheel.suspensionDampingCompression = springCompression;
    wheel.suspensionDampingRelaxation  = springRelaxation;
    
    wheel.gripLongitudinal = 0.8f;
    wheel.gripLateral      = 1.10f;
    wheel.corneringStiffness = 100000.0f;
    
    
    //
    // Attach wheel points
    wheel.mountPoint = glm::vec3(-wheelTrack, -suspensionHeight,  wheelBase);
    wheel.isSteering = true;
    wheel.isPowered  = false;
    car->AddWheel(wheel);
    
    wheel.mountPoint = glm::vec3( wheelTrack, -suspensionHeight,  wheelBase);
    wheel.isSteering = true;
    wheel.isPowered  = false;
    car->AddWheel(wheel);
    
    wheel.mountPoint = glm::vec3(-wheelTrack, -suspensionHeight, -wheelBase);
    wheel.isSteering = false;
    wheel.isPowered  = true;
    car->AddWheel(wheel);
    
    wheel.mountPoint = glm::vec3( wheelTrack, -suspensionHeight, -wheelBase);
    wheel.isSteering = false;
    wheel.isPowered  = true;
    car->AddWheel(wheel);
    
    // Assemble wheel renderers
    unsigned int numberOfWheels = car->GetNumberOfWheels();
    for (unsigned int w=0; w < numberOfWheels; w++) {
        VehicleWheel& wheelRef = car->GetWheel(w);
        wheelRef.wheelRenderer = Engine.Create<MeshRenderer>();
        wheelRef.wheelRenderer->isActive = true;
        wheelRef.wheelRenderer->mesh = wheelMesh;
        wheelRef.wheelRenderer->material = wheelMaterial;
        
        wheelRef.mountScale = wheelScale;
        
        if (wheelRef.mountPoint.x > 0.0f) {
            wheelRef.mountRotation = glm::vec3(90.0f, 90.0f, -wheelCamber);
        } else {
            wheelRef.mountRotation = glm::vec3(90.0f, 90.0f,  wheelCamber);
        }
        
        Engine.sceneMain->AddMeshRendererToSceneRoot(wheelRef.wheelRenderer);
    }
    
    //
    // Acceleration & deceleration / traction & steering
    
    car->SetSteeringRadius(steeringRadius);
    car->SetSteeringSpeedCurve(steeringCurveHigh, steeringCurveMin);
    car->SetSteeringSmoothingRate(steeringSmoothing);
    
    car->SetMaxEnginePower(maxEnginePower);
    car->SetMaxBreakingForce(maxBreakPower);
    
    car->SetBrakeBias(breakBias);
    car->SetSuspensionClamp(6.0f);
    car->EnableAckermann();
    car->SetAckermann(wheelBase * 2.0f, wheelTrack * 2.0f);
    car->SetAero(0.0f, 0.0f);
    car->SetYawStability(150.0f);
    
    car->SetAntiRoll(0.0f, 0.0f);
    
    car->SetTractionLoss(tractionLoss);
    car->SetTractionMultiplier(tractionMul);
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    //
    // Item placement
    
    for (unsigned int i=0; i < itemList.size(); i++) {
        size_t pos = itemList[i].find(".ipl");
        if (pos == std::string::npos) 
            continue;
        std::string name = itemList[i];
        name.erase(pos, 4);
        FileLoader fileLoader("data/" + itemList[i]);
        
        unsigned int numberOfLines = fileLoader.rawData.size();
        for (unsigned int l=0; l < numberOfLines; l++) {
            std::vector<std::string> params = String.Explode(fileLoader.rawData[l], ' ');
            if (params.size() != 10) 
                continue;
            
            std::string modelName      = params[0];
            std::string materialName   = params[1];
            std::string textureMap     = params[2];
            std::string collisionShape = params[3];
            
            float posx = String.ToFloat(params[4]);
            float posy = String.ToFloat(params[5]);
            float posz = String.ToFloat(params[6]);
            glm::vec3 position = {posx, posy, posz};
            
            float rotx = String.ToFloat(params[7]);
            float roty = String.ToFloat(params[8]);
            float rotz = String.ToFloat(params[9]);
            glm::vec3 rotation = {rotx, roty, rotz};
            
            // Generate item object
            MeshTag* mapTag = Resources.FindMeshTag(modelName);
            
            GameObject* gameObject = Engine.Create<GameObject>();
            gameObject->renderDistance = 1000.0f;
            
            Transform* transform = gameObject->GetComponent<Transform>();
            transform->position = position;
            transform->rotation = glm::radians(rotation);
            
            gameObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
            MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
            Engine.sceneMain->AddMeshRendererToSceneRoot(meshRenderer);
            
            meshRenderer->mesh     = Engine.Create<Mesh>();
            meshRenderer->material = Engine.Create<Material>();
            
            meshRenderer->EnableFrustumCulling();
            meshRenderer->SetBoundingBox({-50.0f, -30.0f, -50.0f}, {50.0f, 30.0f, 50.0f});
            
            for (unsigned int s=0; s < mapTag->subMeshes.size(); s++) {
                meshRenderer->mesh->RemoveDegenerateTriangles(mapTag->subMeshes[s]);
                meshRenderer->mesh->AddSubMesh(0, 0, 0, mapTag->subMeshes[s].vertexBuffer, mapTag->subMeshes[s].indexBuffer, false);
            }
            meshRenderer->mesh->Load();
            
            std::unordered_map<std::string, CollisionShape>::iterator it = collisionShapes.find(collisionShape);
            if (it != collisionShapes.end()) {
                const CollisionShape& shape = it->second;
                
                if (shape.shape == "box") {
                    rp3d::BoxShape* boxShape = Physics.CreateColliderBox(shape.scale);
                    
                    rp3d::RigidBody* rigidBody = Physics.CreateRigidBody(position);
                    rigidBody->setType(rp3d::BodyType::STATIC);
                    rp3d::Collider* collider = rigidBody->addCollider(boxShape, rp3d::Transform::identity());
                    
                    collider->setCollisionCategoryBits( (unsigned short)LayerMask::Ground );
                }
                
                if (shape.shape == "sphere") {
                    rp3d::SphereShape* sphereShape = Physics.CreateColliderSphere(shape.scale.x);
                    
                    rp3d::RigidBody* rigidBody = Physics.CreateRigidBody(position);
                    rigidBody->setType(rp3d::BodyType::STATIC);
                    rp3d::Collider* collider = rigidBody->addCollider(sphereShape, rp3d::Transform::identity());
                    
                    collider->setCollisionCategoryBits( (unsigned short)LayerMask::Ground );
                }
                
            } else if (collisionShape == "concave") {
                rp3d::RigidBody* rigidBody = Physics.CreateRigidBody(position);
                rigidBody->setType(rp3d::BodyType::STATIC);
                
                MeshCollider* meshCollider = Physics.CreateConcaveMeshCollider(mapTag->subMeshes[0]);
                if (!meshCollider || !meshCollider->concaveShape) {
                    Physics.DestroyRigidBody(rigidBody);
                    continue;
                }
                
                rp3d::Collider* collider = rigidBody->addCollider(meshCollider->concaveShape, rp3d::Transform::identity());
                
                rp3d::Material& material = collider->getMaterial();
                material.setFrictionCoefficient(0.3f);
                
                collider->setCollisionCategoryBits( (unsigned short)LayerMask::Ground );
                
                rigidBody->updateMassPropertiesFromColliders();
            }
            
            
            /*
            // Generate levels of detail
            SubMesh submesh;
            meshRenderer->mesh->GenerateSimplifiedLOD(0, 0.3f, submesh);
            
            LevelOfDetail low;
            low.mesh = Engine.Create<Mesh>();
            low.mesh->AddSubMesh(0, 0, 0, submesh.vertexBuffer, submesh.indexBuffer, false);
            low.distance = 100;
            low.mesh->CalculateNormals();
            low.mesh->Load();
            
            LevelOfDetail high;
            high.mesh = Engine.Create<Mesh>();
            high.mesh->AddSubMesh(0, 0, 0, mapTag->subMeshes[0].vertexBuffer, mapTag->subMeshes[0].indexBuffer, false);
            high.distance = 300;
            high.mesh->CalculateNormals();
            high.mesh->Load();
            
            meshRenderer->AddLevelOfDetail(high);
            meshRenderer->AddLevelOfDetail(low);
            */
            
            // Apply the material and assign texture set
            for (unsigned int m=0; m < materials.size(); m++) {
                if (materials[m].first != materialName) 
                    continue;
                Material* material = materials[m].second;
                // Assign texture set
                unsigned int numberOfTextures = material->GetNumberOfTextures();
                for (unsigned int t=0; t < numberOfTextures; t++) 
                    meshRenderer->material->AddTexture( material->GetTexture(t) );
                
                // Check to apply a multi-texture map
                if (textureMap != "none") {
                    Resources.LoadMaterialFromTag(textureMap, meshRenderer->material, MATERIAL_FILTER_TRILINEAR, MATERIAL_FILTER_LINEAR);
                    Texture* biasMap = meshRenderer->material->GetTexture(meshRenderer->material->GetNumberOfTextures()-1);
                    biasMap->SetScaleUV({1.0f, 1.0f});
                    biasMap->SetPositionUV({1.0f, 1.0f});
                }
                break;
            }
            
            meshRenderer->material->ambient = Colors.black;
            meshRenderer->material->diffuse = Colors.white;
            meshRenderer->material->shader = Engine.shaders.texture;
            
            // Shadow volume
            //meshRenderer->material->EnableShadowVolumePass();
            //meshRenderer->material->SetShadowVolumeColor(Colors.black);
            //meshRenderer->material->SetShadowVolumeColorIntensity(0.4f);
            //meshRenderer->material->SetShadowVolumeIntensityLow(0.01f);
            //meshRenderer->material->SetShadowVolumeIntensityHigh(0.2f);
            
            //meshRenderer->material->SetShadowVolumeLength(1.0f);
            
        }
    }
    
    
    // Generate a new world
    Weather.SetTime(9000);
    Weather.SetWeather(WeatherType::Clear);
    
}

