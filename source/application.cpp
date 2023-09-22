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









// Application entry point
//

void Framework::Start() {
    
    
    Gene newGene;
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    // Load some external resources
    Resources.LoadWaveFront("data/cube.obj", "cube");
    
    Resources.LoadWaveFront("data/chunk.obj", "chunk");
    Resources.LoadTexture("data/chunk.png", "mat_chunk");
    
    
    Resources.LoadWaveFront("data/sky/sky.obj", "skyBox");
    //Resources.LoadTexture("data/sky/sky.png", "skyMaterial");
    
    Resources.LoadShaderGLSL("data/color.shader", "surface");
    Resources.LoadShaderGLSL("data/texture.shader", "texture");
    
    Shader* chunkShader = Resources.CreateShaderFromTag("surface");
    Shader* skyShader   = Resources.CreateShaderFromTag("texture");
    
    
    //
    // Set the gravity vector for the simulation
    Physics.SetWorldGravity(0.0f, -9.81f * 2 * 2, 0);
    
    
    //
    // Create a sky object
    Color skyLow  = Colors.ltgray;
    Color skyHigh = Colors.blue;
    skyHigh *= Colors.white;
    
    GameObject* skyObject = Engine.CreateSky("skyBox", "surface", skyLow, skyHigh, 0.127);
    
    
    //
    // Create a camera controller
    cameraController = Engine.CreateCameraController(glm::vec3(0, 50, 0), glm::vec3(1, 8, 1));
    cameraController->transform.position = glm::vec3(0);
    // Attach sky to the camera
    skyObject->parent = cameraController;
    
    Script* scriptPtr = cameraController->GetComponent<Script>("controller");
    
    if (scriptPtr == nullptr) 
        Application.isActive = false;
    
    
    //
    // AI system currently being implemented (possible state machine)
    //
    
    Actor* actor = AI.CreateActor();
    
    
    
    // Basic chunk generation parameters
    
    float chunkSz = 100;
    
    float noiseX    = 0.008;
    float noiseZ    = 0.008;
    float noiseMul  = 100.0;
    
    int areaWidth  = 8;
    int areaHeight = 8;
    
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
            //chunkMaterial->GenerateMipMaps();
            
            
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
                vertex.y = 0;
                
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
                
                //meshCollider->heightMapBuffer[i] = vertex.y;
                
                continue;
            }
            
            chunkMesh->UpdateMesh();
            
            // Add a physics component
            newChunk->AddComponent( Engine.CreateComponent(Components.RigidBody) );
            
            // Lock in place
            newChunk->SetStatic();
            newChunk->SetLinearAxisLockFactor(0, 0, 0);
            newChunk->SetAngularAxisLockFactor(0, 0, 0);
            
            
            BoxShape* chunkCollider = Physics.CreateColliderBox(chunkSz, 10, chunkSz);
            newChunk->AddColliderBox(chunkCollider, 0, -10, 0);
            
            
            
            // Height field test
            
            //meshCollider->heightFieldShape = 
            //Physics.common.createHeightFieldShape(10, 10, 100, 500, meshCollider->heightMapBuffer, rp3d::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE);
            
            /*
            rp3d::Transform offsetTransform;
            offsetTransform.setPosition(rp3d::Vector3(0, 0, 0));
            
            newChunk->GetComponent<rp3d::RigidBody>()->addCollider(meshCollider->heightFieldShape, offsetTransform);
            */
            
            
            /*
            rp3d::HeightFieldShape* heightFieldShape = 
            Physics.common.createHeightFieldShape(10, 10, 100, 500,gridArray, rp3d::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE);
            
            
            
            // Chunk collider
            
            //
            // Too stupid to figure out convex/concave mesh colliders...
            //
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
float cameraSpeed     = 80000;



//
// Application loop
//

void Framework::Run() {
    
    glm::vec3 force(0);
    
    // Directional movement
    if (Input.CheckKeyCurrent(VK_W)) {force += Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_S)) {force -= Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_A)) {force += Renderer.cameraMain->right;}
    if (Input.CheckKeyCurrent(VK_D)) {force -= Renderer.cameraMain->right;}
    
    // Elevation
    if (Input.CheckKeyCurrent(VK_SPACE)) {force += Renderer.cameraMain->up;}
    if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= Renderer.cameraMain->up;}
    
    
    force *= cameraSpeed;
    cameraController->AddForce(force.x, force.y, force.z);
    
    
    
    
    
    // Escape key pause
    
    if (Input.CheckKeyPressed(VK_ESCAPE)) {
        
        Application.Pause();
        
        if (Application.isPaused) {
            Renderer.cameraMain->DisableMouseLook();
            Input.ClearKeys();
            
            Application.ShowMouseCursor();
            
        } else {
            Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
            Renderer.cameraMain->EnableMouseLook();
            
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



