#include <GameEngineFramework/Engine/EngineSystems.h>

#include <GameEngineFramework/plugins.h>


// Events

void EventLostFocus(void) {
    
    Engine.EnableConsole();
    
    Camera* mainCamera = Engine.cameraController->GetComponent<Camera>();
    
    mainCamera->DisableMouseLook();
    
    Input.ClearKeys();
    
    Platform.ShowMouseCursor();
    
    if (!Platform.isPaused) 
        Platform.Pause();
    
    return;
}


// List worlds
void FuncList(std::vector<std::string> args) {
    
    std::vector<std::string> dirList = fs.DirectoryGetList("worlds");
    
    if (dirList.size() == 0) {
        
        Engine.Print("No worlds");
        
        return;
    }
    
    for (unsigned int i=0; i < dirList.size(); i++) 
        Engine.Print( dirList[i] );
    
    return;
}

// Save world
void FuncSave(std::vector<std::string> args) {
    
    if (args[0] != "") 
        chunkManager.world.name = args[0];
    
    if (chunkManager.SaveWorld()) {
        
        Engine.Print("Saving: " + chunkManager.world.name);
        
        return;
    }
    
    Engine.Print("Error saving world: " + chunkManager.world.name);
    
    return;
}

// Load world
void FuncLoad(std::vector<std::string> args) {
    
    if (args[0] != "") 
        chunkManager.world.name = args[0];
    
    if (chunkManager.LoadWorld()) {
        
        Engine.Print("Loading: " + chunkManager.world.name);
        
        return;
    }
    
    Engine.Print("Generating: " + chunkManager.world.name);
    
    chunkManager.worldSeed = Random.Range(100, 10000000) - Random.Range(100, 10000000);
    
    chunkManager.SaveWorld();
    
    return;
}

// Delete world
void FuncRemove(std::vector<std::string> args) {
    
    if (args[0] == "") 
        return;
    
    if (chunkManager.DestroyWorld( args[0] )) {
        
        Engine.Print("Deleted: " + args[0]);
        
        return;
    }
    
    Engine.Print("Error deleting world: " + args[0]);
    
    return;
}

// Clear world
void FuncClear(std::vector<std::string> args) {
    
    chunkManager.ClearWorld();
    
    Engine.cameraController->SetPosition(0, 0, 0);
    Camera* camera = Engine.cameraController->GetComponent<Camera>();
    
    camera->mouseLookAngle = glm::vec2(0, 0);
    
    Engine.Print("World cleared");
    
    return;
}

// Set the world seed
void FuncSeed(std::vector<std::string> args) {
    
    if (args[0] == "") {
        
        Engine.Print("World seed: " + Int.ToString(chunkManager.worldSeed));
        
        return;
    }
    
    chunkManager.worldSeed = String.ToInt(args[0]);
    
    Engine.Print("World seed: " + Int.ToString(chunkManager.worldSeed));
    
    return;
}


// Summon an actor
void FuncSummon(std::vector<std::string> args) {
    
    for (uint8_t i=0; i < 24; i++) {
        
        unsigned int entityType = 0;
        
        if (args[0] == "Sheep")   {entityType = 1;}
        if (args[0] == "Bear")    {entityType = 2;}
        
        if (entityType == 0) {
            
            Engine.Print("Unknown actor type");
            
            return;
        }
        
        glm::vec3 randomOffset = Engine.sceneMain->camera->transform.GetPosition();
        
        randomOffset.x += Random.Range(0, 20) - Random.Range(0, 20);
        randomOffset.z += Random.Range(0, 20) - Random.Range(0, 20);
        
        GameObject* newActorObject = chunkManager.SpawnActor( randomOffset.x, randomOffset.y, randomOffset.z );
        
        Actor* newActor = newActorObject->GetComponent<Actor>();
        
        unsigned long int actorAge = 900 + (Random.Range(0, 500) - Random.Range(0, 500));
        
        newActor->SetAge( actorAge );
        
        switch (entityType) {
            
            default:
            case 1: AI.genomes.presets.Sheep( newActor ); break;
            case 2: AI.genomes.presets.Bear( newActor ); break;
            
        }
        
        continue;
    }
    
    Engine.Print("Actor summoned");
    
    return;
}


