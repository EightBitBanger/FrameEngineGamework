// User application code space
//

#include "Engine/Engine.h"

extern EngineComponents     Components;
extern ColorPreset          Colors;
extern RandomGen            Random;
extern Logger               Log;
extern Timer                PhysicsTime;
extern Timer                Time;

extern EngineSystemManager  Engine;
extern ApplicationLayer     Application;
extern ResourceManager      Resources;
extern Serialization        Serializer;
extern ScriptSystem         Scripting;
extern RenderSystem         Renderer;
extern PhysicsSystem        Physics;
extern InputSystem          Input;
extern MathCore             Math;
extern ActorSystem          AI;


// User globals
GameObject*  cameraController;

Material* skyMaterial;







// Application entry point
//

void Framework::Start() {
    
    // Create some objects from the loaded resources
    Shader* chunkShader = Resources.CreateShaderFromTag("surface");
    Shader* skyShader   = Resources.CreateShaderFromTag("texture");
    
    
    // World physics
    Physics.SetWorldGravity(0, 0, 0);
    
    
    
    //
    // Create a sky object
    Color skyTone(0.87f, 0.87f, 0.87f);
    
    // Calculate a gradient
    Color skyLow;
    Color skyHigh;
    skyLow = Colors.ltgray * skyTone;
    skyHigh = Colors.blue * skyTone * Colors.white;
    
    GameObject* skyObject = Engine.CreateSky("sky", "surface", skyLow, skyHigh, 0.127);
    
    skyMaterial = skyObject->GetComponent<MeshRenderer>()->GetMaterial();
    
    skyMaterial->diffuse = Color(0.0, 0.0, 0.0);
    
    //
    // Create a camera controller
    cameraController = Engine.CreateCameraController(glm::vec3(0, 50, 0), glm::vec3(1, 8, 1));
    cameraController->transform.position = glm::vec3(0);
    cameraController->EnableGravity(true);
    cameraController->AddComponent( Engine.CreateComponent( Components.Light ) );
    
    cameraController->GetComponent<Light>()->intensity   = 10.0f;
    cameraController->GetComponent<Light>()->range       = 100.0f;
    cameraController->GetComponent<Light>()->attenuation = 0.7f;
    
    
    
    
    // Attach sky to the camera
    skyObject->parent = cameraController;
    
    //Script* scriptPtr = cameraController->GetComponent<Script>("controller");
    
    
    return;
    
    
    // Basic chunk generation parameters
    
    float chunkSz = 100;
    
    float noiseX    = 0.01;
    float noiseZ    = 0.01;
    float noiseMul  = 100.0;
    
    int areaWidth  = 1;
    int areaHeight = 1;
    
    Color biomeHigh = Colors.green;
    Color biomeLow  = Colors.green;
    
    biomeHigh *= Color(0.1, 0.1, 0.1);
    biomeLow  *= Color(0.03, 0.03, 0.03);
    
    
    
    //
    // Generate a few perlin noise chunks
    //
    
    for (int z=0; z < areaHeight; z++) {
        
        for (int x=0; x < areaWidth; x++) {
            
            GameObject* newChunk = Engine.CreateGameObject();
            
            Mesh* chunkMesh = Resources.CreateMeshFromTag("chunk");
            Material* chunkMaterial = Renderer.CreateMaterial();
            
            newChunk->AddComponent( Engine.CreateComponentMeshRenderer(chunkMesh, chunkMaterial) );
            
            
            chunkMaterial->SetShader( chunkShader );
            
            chunkMaterial->SetTextureFiltration(MATERIAL_FILTER_LINEAR);
            
            
            //
            // Perlin noise generation
            //
            
            float chunkX = x * chunkSz;
            float chunkZ = z * chunkSz;
            
            //MeshCollider* meshCollider = Physics.meshCollider.Create();
            
            unsigned int vertexCount = chunkMesh->GetNumberOfVertices();
            
            for (unsigned int i=0; i < vertexCount; i++) {
                
                Vertex vertex = chunkMesh->GetVertex(i);
                
                // Base noise
                float xCoord = (vertex.x * noiseX) + (chunkX * noiseX);
                float zCoord = (vertex.z * noiseZ) + (chunkZ * noiseZ);
                
                float noiseTotal = Random.Perlin(xCoord, 0, zCoord) * noiseMul;
                
                // Height caps
                if (noiseTotal < 0) noiseTotal  = 0;
                if (noiseTotal > 50) noiseTotal = 50;
                
                // Height steps
                vertex.y = Math.Round(noiseTotal);
                
                // Fade color by height
                Color colorTotal;
                colorTotal.r = Math.Lerp(biomeLow.r, biomeHigh.r, noiseTotal * 0.01);
                colorTotal.g = Math.Lerp(biomeLow.g, biomeHigh.g, noiseTotal * 0.01);
                colorTotal.b = Math.Lerp(biomeLow.b, biomeHigh.b, noiseTotal * 0.01);
                
                // Apply changes to the mesh buffer
                vertex.r = colorTotal.r;
                vertex.g = colorTotal.g;
                vertex.b = colorTotal.b;
                
                chunkMesh->SetVertex(i, vertex);
                
                // Seggy McFault
                //meshCollider->heightMapBuffer[i] = vertex.y;
                
                continue;
            }
            
            chunkMesh->UpdateMesh();
            
            newChunk->AddComponent( Engine.CreateComponent(Components.RigidBody) );
            
            // Lock the chunk in place
            newChunk->SetStatic();
            newChunk->SetLinearAxisLockFactor(0, 0, 0);
            newChunk->SetAngularAxisLockFactor(0, 0, 0);
            //newChunk->SetMass(100);
            
            BoxShape* chunkCollider = Physics.CreateColliderBox(chunkSz, 100, chunkSz);
            newChunk->AddColliderBox(chunkCollider, 0, -100, 0);
            
            
            
            // Height field test
            /*
            meshCollider->heightFieldShape = 
            Physics.common.createHeightFieldShape(10, 10, 0, 1000, meshCollider->heightMapBuffer, rp3d::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE);
            
            rp3d::Transform offsetTransform;
            offsetTransform.setPosition(rp3d::Vector3(0, 0, 0));
            
            newChunk->GetComponent<RigidBody>()->addCollider(meshCollider->heightFieldShape, offsetTransform);
            */
            
            
            /*
            // Chunk collider
            
            
            
            
            //
            // Too stupid to figure out convex/concave mesh colliders...
            // crashing
            
            //MeshCollider* meshCollider = Physics.CreateColliderFromMesh(chunkMesh);
            //newChunk->AddColliderMesh(meshCollider);
            
            //MeshCollider* meshCollider = Physics.CreateColliderHeightMapFromMesh(chunkMesh);
            //newChunk->AddColliderMesh(meshCollider);
            
            
            
            */
            
            
            
            
            
            newChunk->SetPosition((x * chunkSz) - ((areaWidth/2) * chunkSz), 0.0f, (z * chunkSz) - ((areaHeight/2) * chunkSz));
            newChunk->transform.SetScale(1, 1, 1);
            
        }
    }
    
    
    return;
}










// Camera movement force
float cameraSpeed     = 100;



//
// Application loop
//

void Framework::Run() {
    
    //
    // AI system currently being implemented
    //
    
    if (Input.CheckMouseLeftPressed()) {
        
        //Gene newGene;
        
        GameObject* actorObject = Engine.CreateAIActor( (Renderer.GetCamera()->transform.position + (Renderer.GetCamera()->forward * 3.0f)) );
        
        Material* actorMaterial = actorObject->GetComponent<MeshRenderer>()->GetMaterial();
        actorMaterial->ambient = Color(0, 0, 0);
        rp3d::Vector3 position;
        
        rp3d::Vector3 force;
        Camera* currentCamera = Renderer.GetCamera();
        force.x = currentCamera->forward.x;
        force.y = currentCamera->forward.y;
        force.z = currentCamera->forward.z;
        
        actorObject->GetComponent<RigidBody>()->applyLocalForceAtCenterOfMass(force);
        
    }
    
    
    
    
    
    
    
    //
    // Day night cycle testing
    
    if (skyMaterial->diffuse < Color(1, 1, 1)) {
        
        Color skyShift(0.001f, 0.001f, 0.001f);
        skyMaterial->diffuse += skyShift;
        
        if (skyMaterial->diffuse > Color(0.87, 0.87, 0.87)) 
            skyMaterial->diffuse = Color(0.87, 0.87, 0.87);
        
    }
    
    
    glm::vec3 force(0);
    
    // Directional movement
    Camera* currentCamera = Renderer.GetCamera();
    if (Input.CheckKeyCurrent(VK_W)) {force += currentCamera->forward;}
    if (Input.CheckKeyCurrent(VK_S)) {force -= currentCamera->forward;}
    if (Input.CheckKeyCurrent(VK_A)) {force += currentCamera->right;}
    if (Input.CheckKeyCurrent(VK_D)) {force -= currentCamera->right;}
    
    // Elevation
    if (Input.CheckKeyCurrent(VK_SPACE)) {force += currentCamera->up;}
    if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= currentCamera->up;}
    
    if (Input.CheckKeyCurrent(VK_CONTROL)) force *= 2;
    
    force *= cameraSpeed;
    cameraController->AddForce(force.x, force.y, force.z);
    
    
    
    // Escape key pause
    
    if (Input.CheckKeyPressed(VK_ESCAPE)) {
        
        Application.Pause();
        
        if (Application.isPaused) {
            Renderer.GetCamera()->DisableMouseLook();
            Input.ClearKeys();
            
            Application.ShowMouseCursor();
            
        } else {
            Renderer.GetCamera()->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
            Renderer.GetCamera()->EnableMouseLook();
            
            Application.HideMouseCursor();
            
            Time.Update();
            PhysicsTime.Update();
        }
        
    }
    
    return;
}


//
// Called once every tick
//

void Framework::TickUpdate(void) {
    
    
    
    
    
    
    
    
}





void Framework::Shutdown(void) {
    
    return;
}



