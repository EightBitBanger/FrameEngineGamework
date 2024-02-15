//
// Deferred deletion algorithm

#include <GameEngineFramework/Engine/Engine.h>

ENGINE_API extern EngineComponents  Components;
ENGINE_API extern ColorPreset       Colors;
ENGINE_API extern NumberGeneration  Random;
ENGINE_API extern StringType        String;
ENGINE_API extern Logger            Log;
ENGINE_API extern Timer             PhysicsTime;
ENGINE_API extern Timer             Time;

ENGINE_API extern Serialization     Serializer;
ENGINE_API extern ResourceManager   Resources;
ENGINE_API extern ScriptSystem      Scripting;
ENGINE_API extern RenderSystem      Renderer;
ENGINE_API extern PhysicsSystem     Physics;
ENGINE_API extern AudioSystem       Audio;
ENGINE_API extern InputSystem       Input;
ENGINE_API extern MathCore          Math;
ENGINE_API extern ActorSystem       AI;

ENGINE_API extern PlatformLayer     Platform;



void EngineSystemManager::ProcessDeferredDeletion(void) {
    
    // Purge game objects
     int numberOfGameObjects = mGarbageGameObjects.size();
    if (numberOfGameObjects > 0) {
        
        unsigned int objectsPerCycle = glm::min(1, (numberOfGameObjects / 2));
        
        for (unsigned int i=0; i < objectsPerCycle; i++) {
            
            GameObject* objectPtr = mGarbageGameObjects[i];
            
            DestroyGameObject( objectPtr );
            
            continue;
        }
        
    }
    
    mGarbageGameObjects.clear();
    
    
    
    // Rigid bodies
    if (mGarbageRigidBodies.size() > 0) {
        
        rp3d::RigidBody* rigidBody = mGarbageRigidBodies[ mGarbageRigidBodies.size() - 1 ];
        
        mGarbageRigidBodies.erase( mGarbageRigidBodies.end() - 1 );
        
        //
        // Cleanse the rigid body before handing it back to the user
        //
        
        rigidBody->setUserData(nullptr);
        
        // Clear colliders
        unsigned int numberOfColliders = rigidBody->getNbColliders();
        for (unsigned int i=0; i < numberOfColliders; i++) {
            rp3d::Collider* collider = rigidBody->getCollider( i );
            rigidBody->removeCollider( collider );
        }
        
        // Reset physics
        rigidBody->resetForce();
        rigidBody->resetTorque();
        
        // Defaults
        rigidBody->setMass(1);
        
        rigidBody->setLinearDamping( 0 );
        rigidBody->setAngularDamping( 0 );
        
        rigidBody->setLinearLockAxisFactor( rp3d::Vector3(0, 0, 0) );
        rigidBody->setAngularLockAxisFactor( rp3d::Vector3(0, 0, 0) );
        
        rigidBody->setLinearVelocity( rp3d::Vector3(0, 0, 0) );
        
        rigidBody->setLocalCenterOfMass( rp3d::Vector3(0, 0, 0) );
        
        rigidBody->setType( rp3d::BodyType::DYNAMIC );
        
        rigidBody->setIsAllowedToSleep(true);
        rigidBody->setIsActive(false);
        
        rigidBody->enableGravity(false);
        
        rigidBody->updateLocalCenterOfMassFromColliders();
        
        // Add the clean rigid body to a free list
        mFreeRigidBodies.push_back( rigidBody );
        
        //if (rigidBody != nullptr) 
        //    Physics.DestroyRigidBody( rigidBody );
        
    }
    
    return;
}


