#ifndef _FRUSTUM_CULLING__
#define _FRUSTUM_CULLING__

struct Frustum {
    
    /// Frustum sides - Left, Right, Bottom, Top, Near, Far
    glm::vec4 planes[6];
};


#endif
