#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Engine/EngineSystems.h>


GameObject* EngineSystemManager::CreateGameObject(void) {
    GameObject* newGameObject = mGameObjects.Create();
    newGameObject->mComponentRegistry = &mComponentRegistry;
    mGameObjectActive.push_back(newGameObject);
    newGameObject->AddComponent( CreateComponent<Transform>() );
    return newGameObject;
}

bool EngineSystemManager::DestroyGameObject(GameObject* gameObjectPtr) {
    gameObjectPtr->isGarbage = true;
    return true;
}

unsigned int EngineSystemManager::GetNumberOfGameObjects(void) {
    return mGameObjects.Size();
}
