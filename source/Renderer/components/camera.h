


struct Camera {
    
    Transform transform;
    
    glm::vec3 up;
    glm::vec3 forward;
    glm::vec3 right;
    
    Script* script;
    
    bool useMovementKeys;
    bool useMouseLook;
    
    float cameraSpeed;
    
    float fov;
    float aspect;
    float clipNear;
    float clipFar;
    
    int MouseLimitPitchMax;
    int MouseLimitPitchMin;
    
    float MouseSensitivityYaw;
    float MouseSensitivityPitch;
    
    
    Camera() {
        
        up      = glm::vec3(0, 1, 0);
        forward = glm::vec3(0, 0, 0);
        right   = glm::vec3(0, 0, 0);
        
        script = nullptr;
        
        useMovementKeys = false;
        useMouseLook = false;
        
        cameraSpeed = 200;
        
        fov       = 60;
        aspect    = 1.33;
        clipNear  = 0.03;
        clipFar   = 1000000;
        
        MouseLimitPitchMax = 180;
        MouseLimitPitchMin = 180;
        
        MouseSensitivityYaw   = 0.004;
        MouseSensitivityPitch = 0.0038;
        
    }
    
    
    void Update(void) {
        if (!useMovementKeys) return;
        
        if (Input.CheckKeyCurrent(VK_W)) transform.position += forward * cameraSpeed * Time.delta * glm::vec3(1, 0.2, 1);
        if (Input.CheckKeyCurrent(VK_S)) transform.position -= forward * cameraSpeed * Time.delta * glm::vec3(1, 0.2, 1);
        if (Input.CheckKeyCurrent(VK_A)) transform.position += right * cameraSpeed * Time.delta;
        if (Input.CheckKeyCurrent(VK_D)) transform.position -= right * cameraSpeed * Time.delta;
        
        if (Input.CheckKeyCurrent(VK_SPACE)) transform.position += up * cameraSpeed * Time.delta;
        if (Input.CheckKeyCurrent(VK_SHIFT)) transform.position -= up * cameraSpeed * Time.delta;
        
        return;
    }
    
    void MouseLook(float delta, int reset_x, int reset_y);
    
    void SetMouseCenter(int reset_x, int reset_y) {CursorSetPosition(reset_x, reset_y);}
    
    void EnableMouseLook(void)  {ShowCursor(false); useMouseLook = true;}
    void DisableMouseLook(void) {ShowCursor(true); useMouseLook = false;}
    
    void EnableMovementInput(void)  {useMovementKeys = true;}
    void DisableMovementInput(void) {useMovementKeys = false;}
    
    
    glm::mat4 CalculateView(void);
    glm::mat4 CalculatePerspectiveMatrix(void);
    glm::mat4 CalculateOrthiographicMatrix(Viewport viewport);
    
};


void Camera::MouseLook(float delta, int reset_x, int reset_y) {
    
    glm::vec2 MousePos = CursorGetPosition();
    CursorSetPosition(reset_x, reset_y);
    
    float MouseDiffX = (MousePos.x - reset_x) * MouseSensitivityYaw * delta;
    float MouseDiffY = (MousePos.y - reset_y) * MouseSensitivityPitch * delta;
    
    transform.rotation.x += (float)MouseDiffX;
    transform.rotation.y -= (float)MouseDiffY;
    
    if (transform.rotation.x >= 0.109655)  {transform.rotation.x -= 0.109655;}
    if (transform.rotation.x <= 0.109655)  {transform.rotation.x += 0.109655;}
    
    if (transform.rotation.y >  0.0274f) transform.rotation.y =  0.0274f;
    if (transform.rotation.y < -0.0274f) transform.rotation.y = -0.0274f;
    
    return;
}

glm::mat4 Camera::CalculateView(void) {
    
    glm::vec3 pos;
    pos.x = transform.position.x;
    pos.y = transform.position.y;
    pos.z = transform.position.z;
    
    forward.x = cos( transform.rotation.x * 180 / glm::pi<float>() );
    forward.y = tan( transform.rotation.y * 180 / glm::pi<float>() );
    forward.z = sin( transform.rotation.x * 180 / glm::pi<float>() );
    
    forward = glm::normalize(forward);
    
    glm::vec3 angle;
    angle.x = transform.position.x + forward.x;
    angle.y = transform.position.y + forward.y;
    angle.z = transform.position.z + forward.z;
    
    right = glm::normalize(glm::cross(up, forward));
    
    glm::mat4 view = glm::lookAt(pos, angle, up);
    
    return view;
}

glm::mat4 Camera::CalculatePerspectiveMatrix(void) {
    
    glm::mat4 projection = glm::perspective( glm::radians( fov ), aspect, clipNear, clipFar);
    
    return projection;
}

glm::mat4 Camera::CalculateOrthiographicMatrix(Viewport viewport) {
    
    
    // \/ sus* O_o \/
    
    glm::mat4 projection = glm::ortho(viewport.x, viewport.w, viewport.h, viewport.y, -1, 1);
    
    return projection;
}






