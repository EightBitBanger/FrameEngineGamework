

struct Transform {
    
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    
    Transform* parent;
    
    std::vector<Transform*> child;
    
    Transform() {
        
        position = glm::vec3(0, 0, 0);
        rotation = glm::vec3(0, 0, 0);
        scale    = glm::vec3(1, 1, 1);
        
        parent = nullptr;
        
    }
    
    
    void ChildAdd(Transform* transform) {
        child.push_back(transform);
    }
    
    bool ChildRemove(Transform* transform) {
        for (std::vector<Transform*>::iterator it = child.begin(); it != child.end(); ++it) {
            Transform* transformPtr = *it;
            if (transform == transformPtr) {
                child.erase(it);
                return true;
            }
        }
        return false;    
    }
    
    
    Transform Identity(void) {
        Transform identity;
        return identity;
    }
    
};




