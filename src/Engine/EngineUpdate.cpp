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
    
    for (unsigned int i=0; i < streamSize; i++ ) {
        
        if (!streamBuffer[i].gameObject->isActive) 
            continue;
        
        // Current transform
        Transform currentTransform;
        currentTransform.position    = streamBuffer[i].gameObject->mTransformCache->position;
        currentTransform.orientation = streamBuffer[i].gameObject->mTransformCache->orientation;
        currentTransform.scale       = streamBuffer[i].gameObject->mTransformCache->scale;
        
        // Calculate parent transforms
        GameObject* parent = streamBuffer[i].gameObject->parent;
        
        // Roll over the parent matrix transform chain
        while (parent != nullptr) {
            
            //currentTransform.position    += parent->transform.position;
            //currentTransform.scale       *= parent->transform.scale;
            //currentTransform.orientation *= parent->transform.orientation;
            
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
            streamBuffer[i].gameObject->mTransformCache->position    = currentTransform.position;
            streamBuffer[i].gameObject->mTransformCache->orientation = currentTransform.orientation;
            
            currentTransform.matrix = glm::scale(currentTransform.matrix, streamBuffer[i].gameObject->mTransformCache->scale);
            
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
            
            streamBuffer[i].actor->mVelocity = glm::vec3(0, 0.01, 0);
            
            // Update actor
            
            if (streamBuffer[i].actor->mIsActive) {
                
                // Update genetic expression
                
                if (streamBuffer[i].actor->mDoUpdateGenetics) {
                    
                    int numberOfGenes = streamBuffer[i].actor->GetNumberOfGenes();
                    
                    // Clear the old mesh renderers
                    for (int a=0; a < streamBuffer[i].actor->mGeneticRenderers.size(); a++) {
                        
                        MeshRenderer* geneRenderer = streamBuffer[i].actor->mGeneticRenderers[a];
                        
                        if (geneRenderer->material != nullptr) 
                            Renderer.DestroyMaterial(geneRenderer->material);
                        
                        sceneMain->RemoveMeshRendererFromSceneRoot( geneRenderer );
                        
                        Renderer.DestroyMeshRenderer( geneRenderer );
                        
                        continue;
                    }
                    streamBuffer[i].actor->mGeneticRenderers.clear();
                    streamBuffer[i].actor->mGeneticOffsets.clear();
                    
                    // Express genetic elements
                    for (int a=0; a < numberOfGenes; a++) {
                        
                        if (!streamBuffer[i].actor->mGenes[a].doExpress) 
                            continue;
                        
                        Material* newMaterial = Renderer.CreateMaterial();
                        newMaterial->shader = shaders.color;
                        newMaterial->diffuse.r = streamBuffer[i].actor->mGenes[a].color.x;
                        newMaterial->diffuse.g = streamBuffer[i].actor->mGenes[a].color.y;
                        newMaterial->diffuse.b = streamBuffer[i].actor->mGenes[a].color.z;
                        
                        MeshRenderer* newRenderer = Renderer.CreateMeshRenderer();
                        newRenderer->mesh = meshes.cube;
                        newRenderer->material = newMaterial;
                        
                        // Position
                        newRenderer->transform.position.x = streamBuffer[i].actor->mGenes[a].position.x;
                        newRenderer->transform.position.y = streamBuffer[i].actor->mGenes[a].position.y;
                        newRenderer->transform.position.z = streamBuffer[i].actor->mGenes[a].position.z;
                        
                        // Offset
                        glm::vec3 offset(streamBuffer[i].actor->mGenes[a].offset.x, 
                                         streamBuffer[i].actor->mGenes[a].offset.y, 
                                         streamBuffer[i].actor->mGenes[a].offset.z);
                        
                        // Rotation
                        newRenderer->transform.RotateAxis(streamBuffer[i].actor->mGenes[a].rotation.x, glm::vec3(1, 0, 0));
                        newRenderer->transform.RotateAxis(streamBuffer[i].actor->mGenes[a].rotation.y, glm::vec3(0, 1, 0));
                        newRenderer->transform.RotateAxis(streamBuffer[i].actor->mGenes[a].rotation.z, glm::vec3(0, 0, 1));
                        
                        // Scale
                        newRenderer->transform.scale.x = streamBuffer[i].actor->mGenes[a].scale.x;
                        newRenderer->transform.scale.y = streamBuffer[i].actor->mGenes[a].scale.y;
                        newRenderer->transform.scale.z = streamBuffer[i].actor->mGenes[a].scale.z;
                        
                        streamBuffer[i].actor->mGeneticRenderers.push_back( newRenderer );
                        streamBuffer[i].actor->mGeneticOffsets.push_back( offset );
                        
                        newRenderer->transform.UpdateMatrix();
                        
                        sceneMain->AddMeshRendererToSceneRoot(newRenderer);
                        
                        continue;
                    }
                    
                    streamBuffer[i].actor->mDoUpdateGenetics = false;
                } else {
                    
                    // Update genetic renderers
                    for (int a=0; a < streamBuffer[i].actor->mGeneticRenderers.size(); a++) {
                        
                        streamBuffer[i].actor->mGeneticRenderers[a]->transform.position  = streamBuffer[i].actor->mPosition;
                        streamBuffer[i].actor->mGeneticRenderers[a]->transform.position += streamBuffer[i].actor->mGeneticOffsets[a];
                        
                        streamBuffer[i].actor->mGeneticRenderers[a]->transform.UpdateMatrix();
                        
                        continue;
                    }
                    
                }
                
                
                // Update actor physics
                
                if (streamBuffer[i].rigidBody != nullptr) {
                    
                    glm::vec3 actorVelocity = streamBuffer[i].actor->mVelocity;
                    
                    // Update actor
                    streamBuffer[i].actor->mPosition = currentTransform.position;
                    
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
                    streamBuffer[i].gameObject->mTransformCache->position.z = Renderer.viewport.w + 
                                                                              streamBuffer[i].panel->width * 
                                                                              streamBuffer[i].panel->canvas.y;
                    
                } else {
                    
                    //
                    // Anchor LEFT by default
                    //
                    
                    streamBuffer[i].gameObject->mTransformCache->position.z  = (streamBuffer[i].panel->canvas.y * streamBuffer[i].panel->width);
                    streamBuffer[i].gameObject->mTransformCache->position.z += streamBuffer[i].panel->width;
                    
                    //
                    // Anchor CENTER horizontally
                    //
                    
                    if (streamBuffer[i].panel->canvas.anchorCenterHorz) {
                        
                        streamBuffer[i].gameObject->mTransformCache->position.z = (Renderer.viewport.w / 2) + (streamBuffer[i].panel->canvas.y * streamBuffer[i].panel->width);
                        
                    }
                    
                }
                
                //
                // Anchor TOP
                //
                
                if (streamBuffer[i].panel->canvas.anchorTop) {
                    int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h;
                    
                    topAnchorTotal += (streamBuffer[i].panel->height * streamBuffer[i].panel->height) / 2;
                    topAnchorTotal += streamBuffer[i].panel->height * streamBuffer[i].panel->canvas.x;
                    
                    streamBuffer[i].gameObject->mTransformCache->position.y = topAnchorTotal;
                } else {
                    
                    //
                    // Anchor BOTTOM by default
                    //
                    
                    streamBuffer[i].gameObject->mTransformCache->position.y  = Renderer.displaySize.y - streamBuffer[i].panel->height;
                    streamBuffer[i].gameObject->mTransformCache->position.y -= streamBuffer[i].panel->height * -(streamBuffer[i].panel->canvas.x);
                    
                    //
                    // Anchor CENTER vertically
                    //
                    
                    if (streamBuffer[i].panel->canvas.anchorCenterVert) {
                        int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h / 2;
                        
                        topAnchorTotal += (streamBuffer[i].panel->height * streamBuffer[i].panel->height) / 2;
                        topAnchorTotal += (streamBuffer[i].panel->height * streamBuffer[i].panel->canvas.x) - (streamBuffer[i].panel->height * 2);
                        
                        streamBuffer[i].gameObject->mTransformCache->position.y = topAnchorTotal;
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
                    streamBuffer[i].gameObject->mTransformCache->position.z = Renderer.viewport.w + 
                                                                              streamBuffer[i].text->size * 
                                                                              streamBuffer[i].text->canvas.x;
                    
                    // Keep text on screen when anchored right
                    streamBuffer[i].gameObject->mTransformCache->position.z -= streamBuffer[i].text->text.size() * // length of string
                                                                               streamBuffer[i].text->size;         // Size of font text
                    
                } else {
                    
                    //
                    // Anchor LEFT by default
                    //
                    
                    streamBuffer[i].gameObject->mTransformCache->position.z  = (streamBuffer[i].text->canvas.x * streamBuffer[i].text->size);
                    streamBuffer[i].gameObject->mTransformCache->position.z += streamBuffer[i].text->size;
                    
                    //
                    // Anchor CENTER horizontally
                    //
                    
                    if (streamBuffer[i].text->canvas.anchorCenterHorz) {
                        
                        streamBuffer[i].gameObject->mTransformCache->position.z = (Renderer.viewport.w / 2) + (streamBuffer[i].text->canvas.x * streamBuffer[i].text->size);
                        
                    }
                    
                }
                
                //
                // Anchor TOP
                //
                
                if (streamBuffer[i].text->canvas.anchorTop) {
                    int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h;
                    
                    topAnchorTotal += (streamBuffer[i].text->size * streamBuffer[i].text->size) / 2;
                    topAnchorTotal += streamBuffer[i].text->size * streamBuffer[i].text->canvas.y;
                    
                    streamBuffer[i].gameObject->mTransformCache->position.y = topAnchorTotal;
                } else {
                    
                    //
                    // Anchor BOTTOM by default
                    //
                    
                    streamBuffer[i].gameObject->mTransformCache->position.y  = Renderer.displaySize.y - streamBuffer[i].text->size;
                    streamBuffer[i].gameObject->mTransformCache->position.y -= streamBuffer[i].text->size * -(streamBuffer[i].text->canvas.y);
                    
                    //
                    // Anchor CENTER vertically
                    //
                    
                    if (streamBuffer[i].text->canvas.anchorCenterVert) {
                        int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h / 2;
                        
                        topAnchorTotal += (streamBuffer[i].text->size * streamBuffer[i].text->size) / 2;
                        topAnchorTotal += (streamBuffer[i].text->size * streamBuffer[i].text->canvas.y) - (streamBuffer[i].text->size * 2);
                        
                        streamBuffer[i].gameObject->mTransformCache->position.y = topAnchorTotal;
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
