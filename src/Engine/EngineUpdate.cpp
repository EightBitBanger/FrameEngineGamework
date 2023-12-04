// Engine game object related functions
//

#include <GameEngineFramework/Engine/Engine.h>

ENGINE_API extern EngineComponents  Components;
ENGINE_API extern ColorPreset       Colors;
ENGINE_API extern RandomGen         Random;
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

ENGINE_API extern ApplicationLayer      Application;
ENGINE_API extern EngineSystemManager   Engine;



void EngineSystemManager::Update(void) {
    
    // Update player position in the AI simulation
    if (sceneMain != nullptr) {
        
        Camera* activeCamera = sceneMain->camera;
        
        if (activeCamera != nullptr) 
            AI.SetPlayerWorldPosition( activeCamera->transform.position );
        
    }
    
    // Check to update the data stream
    if (doUpdateDataStream) {
        
        doUpdateDataStream = true;
        
        streamSize = mGameObjects.Size();
        
        for (unsigned int i=0; i < streamSize; i++ ) {
            
            streamBuffer[i].gameObject    = mGameObjects[i];
            
            streamBuffer[i].light         = mGameObjects[i]->mLightCache;
            streamBuffer[i].actor         = mGameObjects[i]->mActorCache;
            streamBuffer[i].camera        = mGameObjects[i]->mCameraCache;
            streamBuffer[i].rigidBody     = mGameObjects[i]->mRigidBodyCache;
            streamBuffer[i].meshRenderer  = mGameObjects[i]->mMeshRendererCache;
            
            streamBuffer[i].text          = mGameObjects[i]->mTextCache;
            streamBuffer[i].panel         = mGameObjects[i]->mPanelCache;
            
            continue;
        }
        
    }
    
    
    
    //
    // Run the game object list
    //
    
    for (unsigned int i=0; i < mGameObjects.Size(); i++ ) {
        
        if (!streamBuffer[i].gameObject->isActive) 
            continue;
        
        // Current transform
        Transform currentTransform;
        currentTransform.position    = streamBuffer[i].gameObject->transform.position;
        currentTransform.orientation = streamBuffer[i].gameObject->transform.orientation;
        currentTransform.scale       = streamBuffer[i].gameObject->transform.scale;
        
        // Calculate parent transforms
        GameObject* parent = streamBuffer[i].gameObject->parent;
        
        // Roll over the parent matrix transform chain
        while (parent != nullptr) {
            
            currentTransform.position    += parent->transform.position;
            currentTransform.scale       *= parent->transform.scale;
            currentTransform.orientation *= parent->transform.orientation;
            
            parent = parent->parent;
        }
        
        glm::mat4 translation = glm::translate(glm::mat4(1), currentTransform.position);
        glm::mat4 rotation    = glm::toMat4(currentTransform.orientation);
        glm::mat4 scale       = glm::scale(glm::mat4(1), currentTransform.scale);
        
        currentTransform.matrix = translation * rotation * scale;
        
        
        
        //
        // Rigid body
        //
        if (streamBuffer[i].rigidBody != nullptr) {
            
            // Use the rigid body as the source transform
            rp3d::Transform bodyTransform = streamBuffer[i].rigidBody->getTransform();
            rp3d::Vector3 bodyPosition = bodyTransform.getPosition();
            rp3d::Quaternion quaterion = bodyTransform.getOrientation();
            
            currentTransform.position.x = bodyPosition.x;
            currentTransform.position.y = bodyPosition.y;
            currentTransform.position.z = bodyPosition.z;
            
            currentTransform.orientation.w = quaterion.w;
            currentTransform.orientation.x = quaterion.x;
            currentTransform.orientation.y = quaterion.y;
            currentTransform.orientation.z = quaterion.z;
            
            // Source matrix
            bodyTransform.getOpenGLMatrix(&currentTransform.matrix[0][0]);
            
            // Update the game object transform
            streamBuffer[i].gameObject->transform.position    = currentTransform.position;
            streamBuffer[i].gameObject->transform.orientation = currentTransform.orientation;
            
            currentTransform.matrix = glm::scale(currentTransform.matrix, streamBuffer[i].gameObject->transform.scale);
            
        }
        
        
        
        //
        // Mesh renderer
        //
        if (streamBuffer[i].meshRenderer != nullptr) {
            
            streamBuffer[i].meshRenderer->transform.matrix = currentTransform.matrix;
            
            if (streamBuffer[i].rigidBody == nullptr) {
                streamBuffer[i].meshRenderer->transform.position     = currentTransform.position;
                streamBuffer[i].meshRenderer->transform.orientation  = currentTransform.orientation;
                streamBuffer[i].meshRenderer->transform.scale        = currentTransform.scale;
            }
            
        }
        
        
        
        
        
        //
        // Camera
        //
        
        if (streamBuffer[i].camera != nullptr) {
            
            // Update mouse looking
            if (streamBuffer[i].camera->useMouseLook) {
                
                float MouseDiffX = (Input.mouseX - Renderer.displayCenter.x) * streamBuffer[i].camera->MouseSensitivityYaw;
                float MouseDiffY = (Input.mouseY - Renderer.displayCenter.y) * streamBuffer[i].camera->MouseSensitivityPitch;
                
                Input.SetMousePosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
                
                streamBuffer[i].camera->lookAngle.x += MouseDiffX * 0.01;
                streamBuffer[i].camera->lookAngle.y -= MouseDiffY * 0.01;
                
                // Yaw limit
                if (streamBuffer[i].camera->lookAngle.x >= 0.109655) {streamBuffer[i].camera->lookAngle.x -= 0.109655;}
                if (streamBuffer[i].camera->lookAngle.x <= 0.109655) {streamBuffer[i].camera->lookAngle.x += 0.109655;}
                
                // Pitch limit
                if (streamBuffer[i].camera->lookAngle.y >  0.0274f) streamBuffer[i].camera->lookAngle.y =  0.0274f;
                if (streamBuffer[i].camera->lookAngle.y < -0.0274f) streamBuffer[i].camera->lookAngle.y = -0.0274f;
                
            }
            
            // Restore looking angle
            streamBuffer[i].camera->transform.orientation.x = streamBuffer[i].camera->lookAngle.x;
            streamBuffer[i].camera->transform.orientation.y = streamBuffer[i].camera->lookAngle.y;
            
            // Check camera panel
            if (streamBuffer[i].panel != nullptr) {
                
                // Align the camera with a panel canvas
                
            } else {
                
                streamBuffer[i].camera->transform.position = currentTransform.position;
                
            }
            
        }
        
        
        
        //
        // Lights
        //
        if (streamBuffer[i].light != nullptr) {
            
            streamBuffer[i].light->position    = currentTransform.position;
            
            streamBuffer[i].light->direction   = currentTransform.EulerAngles();
            
        }
        
        
        
        //
        // Actor
        //
        if (streamBuffer[i].actor != nullptr) {
            
            if (streamBuffer[i].actor->GetActive()) {
                
                if (streamBuffer[i].rigidBody != nullptr) {
                    
                    glm::vec3 actorVelocity = streamBuffer[i].actor->GetVelocity();
                    
                    // Set AI inputs
                    streamBuffer[i].actor->SetPosition( currentTransform.position );
                    
                    
                    // Get AI outputs
                    
                    // Apply force velocity
                    streamBuffer[i].rigidBody->applyLocalForceAtCenterOfMass( rp3d::Vector3(actorVelocity.x, actorVelocity.y, actorVelocity.z) );
                    
                }
            }
            
        }
        
        
        
        //
        // Panel canvas alignment
        //
        
        if (streamBuffer[i].panel != nullptr) {
            
            if (streamBuffer[i].meshRenderer != nullptr) {
                
                //
                // Anchor RIGHT
                //
                
                if (streamBuffer[i].panel->canvas.anchorRight) {
                    streamBuffer[i].gameObject->transform.position.z = Renderer.viewport.w + 
                                                                       streamBuffer[i].panel->width * 
                                                                       streamBuffer[i].panel->canvas.y;
                    
                } else {
                    
                    //
                    // Anchor LEFT by default
                    //
                    
                    streamBuffer[i].gameObject->transform.position.z  = (streamBuffer[i].panel->canvas.y * streamBuffer[i].panel->width);
                    streamBuffer[i].gameObject->transform.position.z += streamBuffer[i].panel->width;
                    
                    //
                    // Anchor CENTER horizontally
                    //
                    
                    if (streamBuffer[i].panel->canvas.anchorCenterHorz) {
                        
                        streamBuffer[i].gameObject->transform.position.z = (Renderer.viewport.w / 2) + (streamBuffer[i].panel->canvas.y * streamBuffer[i].panel->width);
                        
                    }
                    
                }
                
                //
                // Anchor TOP
                //
                
                if (streamBuffer[i].panel->canvas.anchorTop) {
                    int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h;
                    
                    topAnchorTotal += (streamBuffer[i].panel->height * streamBuffer[i].panel->height) / 2;
                    topAnchorTotal += streamBuffer[i].panel->height * streamBuffer[i].panel->canvas.x;
                    
                    streamBuffer[i].gameObject->transform.position.y = topAnchorTotal;
                } else {
                    
                    //
                    // Anchor BOTTOM by default
                    //
                    
                    streamBuffer[i].gameObject->transform.position.y  = Renderer.displaySize.y - streamBuffer[i].panel->height;
                    streamBuffer[i].gameObject->transform.position.y -= streamBuffer[i].panel->height * -(streamBuffer[i].panel->canvas.x);
                    
                    //
                    // Anchor CENTER vertically
                    //
                    
                    if (streamBuffer[i].panel->canvas.anchorCenterVert) {
                        int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h / 2;
                        
                        topAnchorTotal += (streamBuffer[i].panel->height * streamBuffer[i].panel->height) / 2;
                        topAnchorTotal += (streamBuffer[i].panel->height * streamBuffer[i].panel->canvas.x) - (streamBuffer[i].panel->height * 2);
                        
                        streamBuffer[i].gameObject->transform.position.y = topAnchorTotal;
                    }
                    
                }
                
            }
            
        }
        
        
        
        //
        // Text canvas alignment
        //
        
        if (streamBuffer[i].text != nullptr) {
            
            if (streamBuffer[i].meshRenderer != nullptr) {
                
                //
                // Anchor RIGHT
                //
                
                if (streamBuffer[i].text->canvas.anchorRight) {
                    streamBuffer[i].gameObject->transform.position.z = Renderer.viewport.w + 
                                                                       streamBuffer[i].text->size * 
                                                                       streamBuffer[i].text->canvas.x;
                    
                    // Keep text on screen when anchored right
                    streamBuffer[i].gameObject->transform.position.z -= streamBuffer[i].text->text.size() * // length of string
                                                                        streamBuffer[i].text->size;         // Size of font text
                    
                } else {
                    
                    //
                    // Anchor LEFT by default
                    //
                    
                    streamBuffer[i].gameObject->transform.position.z  = (streamBuffer[i].text->canvas.x * streamBuffer[i].text->size);
                    streamBuffer[i].gameObject->transform.position.z += streamBuffer[i].text->size;
                    
                    //
                    // Anchor CENTER horizontally
                    //
                    
                    if (streamBuffer[i].text->canvas.anchorCenterHorz) {
                        
                        streamBuffer[i].gameObject->transform.position.z = (Renderer.viewport.w / 2) + (streamBuffer[i].text->canvas.x * streamBuffer[i].text->size);
                        
                    }
                    
                }
                
                //
                // Anchor TOP
                //
                
                if (streamBuffer[i].text->canvas.anchorTop) {
                    int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h;
                    
                    topAnchorTotal += (streamBuffer[i].text->size * streamBuffer[i].text->size) / 2;
                    topAnchorTotal += streamBuffer[i].text->size * streamBuffer[i].text->canvas.y;
                    
                    streamBuffer[i].gameObject->transform.position.y = topAnchorTotal;
                } else {
                    
                    //
                    // Anchor BOTTOM by default
                    //
                    
                    streamBuffer[i].gameObject->transform.position.y  = Renderer.displaySize.y - streamBuffer[i].text->size;
                    streamBuffer[i].gameObject->transform.position.y -= streamBuffer[i].text->size * -(streamBuffer[i].text->canvas.y);
                    
                    //
                    // Anchor CENTER vertically
                    //
                    
                    if (streamBuffer[i].text->canvas.anchorCenterVert) {
                        int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h / 2;
                        
                        topAnchorTotal += (streamBuffer[i].text->size * streamBuffer[i].text->size) / 2;
                        topAnchorTotal += (streamBuffer[i].text->size * streamBuffer[i].text->canvas.y) - (streamBuffer[i].text->size * 2);
                        
                        streamBuffer[i].gameObject->transform.position.y = topAnchorTotal;
                    }
                    
                }
                
                // Flip height and width
                float textGlyphWidth  = streamBuffer[i].text->glyphHeight;
                float textGlyphHeight = streamBuffer[i].text->glyphWidth;
                
                streamBuffer[i].meshRenderer->mesh->ClearSubMeshes();
                Engine.AddMeshText(streamBuffer[i].gameObject, 0, 0, textGlyphWidth, textGlyphHeight, streamBuffer[i].text->text, streamBuffer[i].text->color);
            }
        }
        
        continue;
    }
    
    return;
}
