//
// User application code space
//

#include "Engine/Engine.h"

extern RandomGen            Random;
extern ColorPreset          Colors;
extern Timer                Time;
extern Timer                PhysicsTime;
extern Logger               Log;

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


// Globals
GameObject*  cameraController;









//
// Application entry point
//

void Framework::Start() {
    
    // Load some external resources
    Resources.LoadWaveFront("data/chunk.obj", "chunk");
    Resources.LoadTexture("data/chunk.png", "mat_chunk");
    
    Resources.LoadShaderGLSL("data/color.shader", "surface");
    Resources.LoadShaderGLSL("data/texture.shader", "texture");
    
    Resources.LoadWaveFront("data/sky/sky.obj", "skyBox");
    //Resources.LoadTexture("data/sky/sky.png",   "skyMaterial");
    
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
    cameraController = Engine.CreateCameraController(glm::vec3(0, 10, 0), glm::vec3(1, 8, 1));
    cameraController->transform.position = glm::vec3(0);
    // Attach sky to the camera
    skyObject->parent = cameraController;
    
    
    
    // AI testing
    Actor* actor = AI.CreateActor();
    
    
    
    
    
    
    float chunkSz = 100;
    
    
    
    
    float noiseX    = 0.008;
    float noiseZ    = 0.008;
    float noiseMul  = 70.0;
    
    int areaWidth  = 30;
    int areaHeight = 30;
    
    Color biomeHigh = Colors.green;
    Color biomeLow  = Colors.green;
    
    biomeHigh *= Color(0.1, 0.1, 0.1);
    biomeLow  *= Color(0.03, 0.03, 0.03);
    
    
    
    
    //
    // Generate some perlin noise chunks
    //
    
    for (int z=0; z < areaHeight; z++) {
        
        for (int x=0; x < areaWidth; x++) {
            
            //
            // Generate new chunk
            //
            
            Mesh* chunkMesh = Resources.CreateMeshFromTag("chunk");
            
            //Material* chunkMaterial = Renderer.CreateMaterial();
            Material* chunkMaterial = Resources.CreateMaterialFromTag("mat_chunk");
            
            chunkMaterial->SetShader( chunkShader );
            chunkMaterial->SetTextureFiltration(MATERIAL_FILTER_NONE);
            
            //chunkMaterial->SetTextureFiltration(MATERIAL_FILTER_ANISOTROPIC);
            //chunkMaterial->GenerateMipMaps();
            
            // Chunk position
            float chunkX = x * 100;
            float chunkZ = z * 100;
            
            unsigned int vertexCount = chunkMesh->GetNumberOfVertices();
            
            
            
            //
            // Perlin noise generation
            //
            
            for (unsigned int i=0; i < vertexCount; i++) {
                
                Vertex vertex = chunkMesh->GetVertex(i);
                
                float xCoord = (vertex.x * noiseX) + (chunkX * noiseX);
                float zCoord = (vertex.z * noiseZ) + (chunkZ * noiseZ);
                
                float noiseTotal = Random.Perlin(xCoord, 0, zCoord) * noiseMul;
                
                
                if (noiseTotal < 0) noiseTotal  = 0;
                if (noiseTotal > 50) noiseTotal = 50;
                
                
                vertex.y = Math.Round(noiseTotal);
                
                Color colorTotal;
                colorTotal.r = Math.Lerp(biomeLow.r, biomeHigh.r, noiseTotal * 0.01);
                colorTotal.g = Math.Lerp(biomeLow.g, biomeHigh.g, noiseTotal * 0.01);
                colorTotal.b = Math.Lerp(biomeLow.b, biomeHigh.b, noiseTotal * 0.01);
                
                
                vertex.r = colorTotal.r;
                vertex.g = colorTotal.g;
                vertex.b = colorTotal.b;
                
                chunkMesh->SetVertex(i, vertex);
                
                continue;
            }
            
            chunkMesh->UpdateMesh();
            
            
            
            //
            // Create chunk object
            GameObject* newChunk = Engine.CreateGameObject();
            newChunk->AddComponent( Engine.CreateComponentMeshRenderer(chunkMesh, chunkMaterial) );
            
            newChunk->isActive = true;
            
            // Add a physics component
            newChunk->AddComponent( Engine.CreateComponent(ComponentType::RigidBody) );
            
            // Lock in place
            newChunk->SetStatic();
            newChunk->SetLinearAxisLockFactor(0, 0, 0);
            newChunk->SetAngularAxisLockFactor(0, 0, 0);
            
            // Chunk collider
            BoxShape* chunkCollider = Physics.CreateColliderBox(chunkSz, 10, chunkSz);
            newChunk->AddColliderBox(chunkCollider, 0, -10, 0);
            
            //MeshCollider* meshCollider = Physics.CreateColliderFromMesh(chunkMesh);
            //newChunk->AddColliderMesh(meshCollider);
            
            newChunk->SetPosition((x * chunkSz) - ((areaWidth/2) * chunkSz), 0.0f, (z * chunkSz) - ((areaHeight/2) * chunkSz));
            newChunk->transform.SetScale(1, 1, 1);
            
        }
    }
    
    
    
    
    return;
}










// Projectile shoot force
float projectileSpeed = 700;

// Camera movement force
float cameraSpeed     = 40000;

float forceBuild    = 0;
float forceBuildMax = 1000;



//
// Application loop
//

void Framework::Run() {
    
    // Keyboard movement, WASD keys
    glm::vec3 force(0);
    if (Input.CheckKeyCurrent(VK_W)) {force += Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_S)) {force -= Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_A)) {force += Renderer.cameraMain->right;}
    if (Input.CheckKeyCurrent(VK_D)) {force -= Renderer.cameraMain->right;}
    
    // Space and shift for elevation
    if (Input.CheckKeyCurrent(VK_SPACE)) {force += Renderer.cameraMain->up;}
    if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= Renderer.cameraMain->up;}
    
    // Camera speed multiplier
    force *= cameraSpeed;
    
    cameraController->AddForce(force.x, force.y, force.z);
    
    
    
    
    
    // Escape key pause
    
    if (Input.CheckKeyPressed(VK_ESCAPE)) {
        
        Application.Pause();
        
        if (Application.isPaused) {
            
            Renderer.cameraMain->DisableMouseLook();
            ShowCursor(true);
            
            Input.ClearKeys();
        } else {
            
            Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
            Renderer.cameraMain->EnableMouseLook();
            ShowCursor(false);
            
            Time.Update();
            PhysicsTime.Update();
        }
        
    }
    
    return;
}



void Framework::TickUpdate(void) {
    
    
    
    
    
    
    
    
}





void Framework::Shutdown(void) {
    
    return;
}



