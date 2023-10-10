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
    
    std::vector<std::string> shaderDirectoryList = DirectoryGetList(".\\core\\shaders\\");
    std::vector<std::string> modelsDirectoryList = DirectoryGetList(".\\core\\models\\");
    
    for (int i=0; i < shaderDirectoryList.size(); i++) 
        Resources.LoadShaderGLSL("core/shaders/" + shaderDirectoryList[i], StringGetNameFromFilenameNoExt( shaderDirectoryList[i] ));
    
    for (int i=0; i < modelsDirectoryList.size(); i++) 
        Resources.LoadWaveFront("core/models/" + modelsDirectoryList[i], StringGetNameFromFilenameNoExt( modelsDirectoryList[i] ));
    
    // Load some core resources
    //Resources.LoadShaderGLSL("core/shaders/color.shader",    "surface");
    //Resources.LoadShaderGLSL("core/shaders/texture.shader",  "texture");
    //Resources.LoadWaveFront("core/sky/sky.obj", "skyBox");
    //Resources.LoadWaveFront("core/chunk.obj",   "chunk");
    //Resources.LoadWaveFront("core/cube.obj",    "cube");
    
    
    // Load some external resources
    //Resources.LoadTexture("data/chunk.png", "mat_chunk");
    
    
    Shader* chunkShader = Resources.CreateShaderFromTag("surface");
    Shader* skyShader   = Resources.CreateShaderFromTag("texture");
    
    Physics.SetWorldGravity(0, 0, 0);
    
    
    
    
    //
    // Create a sky object
    Color skyLow;
    Color skyHigh;
    Color skyTone(0.87f, 0.87f, 0.87f);
    skyLow = Colors.ltgray * skyTone;
    skyHigh = Colors.blue * skyTone * Colors.white;
    
    GameObject* skyObject = Engine.CreateSky("sky", "surface", skyLow, skyHigh, 0.127);
    skyMaterial = skyObject->GetComponent<MeshRenderer>()->GetAttachedMaterial();
    skyMaterial->diffuse = Color(0.0, 0.0, 0.0);
    
    
    //
    // Create a camera controller
    cameraController = Engine.CreateCameraController(glm::vec3(0, 50, 0), glm::vec3(1, 8, 1));
    cameraController->transform.position = glm::vec3(0);
    // Attach sky to the camera
    skyObject->parent = cameraController;
    
    Script* scriptPtr = cameraController->GetComponent<Script>("controller");
    
    cameraController->EnableGravity(true);
    
    
    
    
    // Basic chunk generation parameters
    
    float chunkSz = 100;
    
    float noiseX    = 0.008;
    float noiseZ    = 0.008;
    float noiseMul  = 100.0;
    
    int areaWidth  = 20;
    int areaHeight = 20;
    
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
            
            // Add a physics component
            newChunk->AddComponent( Engine.CreateComponent(Components.RigidBody) );
            
            // Lock in place
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
            
            newChunk->GetComponent<rp3d::RigidBody>()->addCollider(meshCollider->heightFieldShape, offsetTransform);
            */
            
            
            /*
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
float cameraSpeed     = 80;



//
// Application loop
//

void Framework::Run() {
    
    //
    // AI system currently being implemented (possible state machine)
    //
    if (Input.CheckMouseLeftPressed()) {
        
        //Gene newGene;
        
        GameObject* ActorObject = Engine.CreateAIActor( (Renderer.cameraMain->transform.position + (Renderer.cameraMain->forward * 3.0f)) );
        
        rp3d::Vector3 position;
        
        rp3d::Vector3 force;
        force.x = Renderer.cameraMain->forward.x;
        force.y = Renderer.cameraMain->forward.y;
        force.z = Renderer.cameraMain->forward.z;
        
        ActorObject->GetComponent<rp3d::RigidBody>()->applyLocalForceAtCenterOfMass(force);
        
    }
    
    
    
    
    
    
    
    //
    // Day night cycle testing
    
    if (skyMaterial->diffuse < Color(1, 1, 1)) {
        
        Color skyShift(0.01f, 0.01f, 0.01f);
        skyMaterial->diffuse += skyShift;
        
        if (skyMaterial->diffuse > Color(0.87, 0.87, 0.87)) 
            skyMaterial->diffuse = Color(0.87, 0.87, 0.87);
        
    }
    
    
    glm::vec3 force(0);
    
    // Directional movement
    if (Input.CheckKeyCurrent(VK_W)) {force += Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_S)) {force -= Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_A)) {force += Renderer.cameraMain->right;}
    if (Input.CheckKeyCurrent(VK_D)) {force -= Renderer.cameraMain->right;}
    
    // Elevation
    if (Input.CheckKeyCurrent(VK_SPACE)) {force += Renderer.cameraMain->up;}
    if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= Renderer.cameraMain->up;}
    
    if (Input.CheckKeyCurrent(VK_CONTROL)) force *= 2;
    
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



