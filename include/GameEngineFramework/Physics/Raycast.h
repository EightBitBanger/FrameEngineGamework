#ifndef _PHYSICS_RAYCAST_
#define _PHYSICS_RAYCAST_

#include "../../../vendor/ReactPhysics3d/ReactPhysics3d.h"

#include <cstdlib>


class RaybackCastCaller : public rp3d::RaycastCallback {
    
public:
    
    bool isHit;
    
    glm::vec3 point;
    
    glm::vec3 normal;
    
    rp3d::CollisionBody* body;
    
    rp3d::Collider* collider;
    
    void* userData;
    
    RaybackCastCaller();
    
    virtual rp3d::decimal notifyRaycastHit( const rp3d::RaycastInfo& info ) {
        
        isHit = true;
        
        point.x = info.worldPoint.x;
        point.y = info.worldPoint.y;
        point.z = info.worldPoint.z;
        
        normal.x = info.worldNormal.x;
        normal.y = info.worldNormal.y;
        normal.z = info.worldNormal.z;
        
        body = info.body;
        collider = info.collider;
        
        userData = collider->getUserData();
        
        // Return a fraction of 1.0 to gather all hits
        return rp3d::decimal (1.0) ;
    }
    
};


class ENGINE_API Hit {
    
public:
    
    glm::vec3 point;
    
    glm::vec3 normal;
    
    rp3d::Collider* collider;
    
    void* gameObject;
    
};

#endif
