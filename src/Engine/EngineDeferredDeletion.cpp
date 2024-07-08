//
// Deferred deletion algorithm

#include <GameEngineFramework/Engine/EngineSystems.h>


void EngineSystemManager::ProcessDeferredDeletion(void) {
    
    // Purge game objects
    int numberOfGameObjects = mGarbageGameObjects.size();
    if (numberOfGameObjects > 0) {
        
        unsigned int objectsPerCycle = glm::max(1, (numberOfGameObjects / 16));
        
        for (unsigned int i=0; i < objectsPerCycle; i++) {
            
            GameObject* objectPtr = mGarbageGameObjects[ i ];
            
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
        if (mFreeRigidBodies.size() < 100) {
            
            mFreeRigidBodies.push_back( rigidBody );
            
            return;
        }
        
        // Purge extra rigid bodies
        Physics.DestroyRigidBody( rigidBody );
        
    }
    
    return;
}


