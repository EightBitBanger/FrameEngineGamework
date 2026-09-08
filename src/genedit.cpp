#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>
#include <GameEngineFramework/Plugins/plugins.h>

extern MeshRenderer* boundsRenderer;
extern Actor* actorTarget;

extern bool IsActorInspectorOpen();
extern void CloseActorInspector();
bool GeneEditorWindow(void);

static bool isGeneEditorOpen = false;
static int selectedGeneIndex = 0;

// Stored speeds to restore once the editor closes
static float savedSpeed      = 1.5f;
static float savedSpeedYouth = 0.8f;

bool IsGeneEditorOpen() {
    return isGeneEditorOpen;
}

static ActorState::Mode savedMode = ActorState::Mode::Idle;

void OpenGeneEditor(Actor* target) {
    if (target == nullptr) 
        return;
    
    if (IsActorInspectorOpen()) {
        CloseActorInspector();
    }
    
    if (actorTarget == nullptr) {
        UI.AddUIFrameCallback(GeneEditorWindow);
    }
    
    actorTarget = target;
    isGeneEditorOpen = true;
    selectedGeneIndex = 0;
    
    // Cache original attributes
    savedSpeed      = actorTarget->physical.GetSpeed();
    savedSpeedYouth = actorTarget->physical.GetSpeedYouth();
    savedMode       = actorTarget->state.mode;
    
    // Freeze actor locomotion and lock orientation
    actorTarget->physical.SetSpeed(0.0f);
    actorTarget->physical.SetSpeedYouth(0.0f);
    actorTarget->navigation.SetVelocity(glm::vec3(0.0f));
    actorTarget->navigation.SetTargetActor(nullptr);
    actorTarget->navigation.SetTargetPoint(actorTarget->navigation.GetPosition());
    actorTarget->state.mode = ActorState::Mode::Frozen;
    
    // Snap the head dead ahead
    actorTarget->SnapHeadStraight();
    
    Platform.ShowMouseCursor();
    if (Engine.sceneMain != nullptr && Engine.sceneMain->camera != nullptr) {
        Engine.sceneMain->camera->DisableMouseLook();
        Engine.sceneMain->camera->centerStandOff = 2.0f;
        Engine.sceneMain->camera->EnableThirdPersonLook();
    }
}

void CloseGeneEditor() {
    if (boundsRenderer != nullptr) {
        boundsRenderer->isActive = false;
    }
    
    if (Engine.sceneMain != nullptr && Engine.sceneMain->camera != nullptr) {
        RestoreCameraControllerToOrbitalView(actorTarget);
        Engine.sceneMain->camera->DisableThirdPersonLook();
        Input.SetMousePosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
        Engine.sceneMain->camera->EnableMouseLook();
    }
    
    // Restore locomotion speeds and state
    if (actorTarget != nullptr) {
        actorTarget->physical.SetSpeed(savedSpeed);
        actorTarget->physical.SetSpeedYouth(savedSpeedYouth);
        actorTarget->state.mode = (savedMode != ActorState::Mode::Frozen) ? savedMode : ActorState::Mode::Idle;
    }
    
    actorTarget = nullptr;
    isGeneEditorOpen = false;
    Platform.HideMouseCursor();
}

bool GeneEditorWindow(void) {
    if (!isGeneEditorOpen || actorTarget == nullptr) 
        return false;

    // Maintain Idle state and current anchor position each frame to override AI thinking updates
    actorTarget->state.mode = ActorState::Mode::Frozen;
    actorTarget->navigation.SetTargetPoint(actorTarget->navigation.GetPosition());
    
    ImGuiIO& io = ImGui::GetIO();
    const float windowWidth  = 380.0f;
    const float windowHeight = 620.0f;
    const float margin       = 24.0f;
    const ImVec4 labelColor(0.6f, 0.8f, 1.0f, 1.0f);

    const ImVec2 leftWindowPos(margin, margin);
    const ImVec2 rightWindowPos(io.DisplaySize.x - windowWidth - margin, margin);

    unsigned int numGenes = actorTarget->genetics.GetNumberOfGenes();

    if (selectedGeneIndex >= static_cast<int>(numGenes)) {
        selectedGeneIndex = (numGenes > 0) ? static_cast<int>(numGenes) - 1 : -1;
    }

    // ==========================================
    // Gene Selector
    // ==========================================
    ImGui::SetNextWindowPos(leftWindowPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Gene Selector", &isGeneEditorOpen)) {
        if (numGenes == 0) {
            ImGui::TextDisabled("No genetic blueprints present.");
        } else {
            ImGui::BeginChild("GeneListScroll", ImVec2(0, 0), true);
            for (unsigned int i = 0; i < numGenes; ++i) {
                Gene gene = actorTarget->genetics.GetGeneFromGenome(i);

                const char* formStr = "Base";
                if (gene.form == ActorState::Genetic::Male)        formStr = "Male";
                else if (gene.form == ActorState::Genetic::Female) formStr = "Female";

                const char* animStr = "Body";
                if (gene.animationType == ActorState::Animation::Head)             animStr = "Head";
                else if (gene.animationType == ActorState::Animation::Limb)        animStr = "Limb";
                else if (gene.animationType == ActorState::Animation::LimbHolding) animStr = "LimbHolding";

                char geneHeader[64];
                snprintf(geneHeader, sizeof(geneHeader), "Gene [%02u] (%s, %s)%s", 
                         i, formStr, animStr, gene.doExpress ? "" : " [Suppressed]");

                bool isSelected = (selectedGeneIndex == static_cast<int>(i));
                if (ImGui::Selectable(geneHeader, isSelected)) {
                    selectedGeneIndex = static_cast<int>(i);
                }
            }
            ImGui::EndChild();
        }
    }
    ImGui::End();

    // ==========================================
    // Gene Editor Interface
    // ==========================================
    ImGui::SetNextWindowPos(rightWindowPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Gene Editor", &isGeneEditorOpen)) {
        if (numGenes == 0) {
            ImGui::TextDisabled("No genetic blueprints available to edit.");
        } else if (selectedGeneIndex >= 0 && selectedGeneIndex < static_cast<int>(numGenes)) {
            Gene gene = actorTarget->genetics.GetGeneFromGenome(static_cast<unsigned int>(selectedGeneIndex));

            ImGui::TextColored(labelColor, "Editing: Gene [%02d]", selectedGeneIndex);
            ImGui::TextDisabled("Tip: Slide thumb or Ctrl+Click to type values.");
            ImGui::Separator();

            bool changed = false;

            // --- Surface Color & Expression ---
            if (ImGui::CollapsingHeader("Color & Expression", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::Checkbox("Express Gene", &gene.doExpress)) {
                    changed = true;
                }

                float col[3] = { gene.color.x, gene.color.y, gene.color.z };
                if (ImGui::ColorEdit3("Color", col)) {
                    gene.color.x = col[0];
                    gene.color.y = col[1];
                    gene.color.z = col[2];
                    changed = true;
                }

                int currentForm = static_cast<int>(gene.form);
                const char* forms[] = { "Base (All)", "Male Only", "Female Only" };
                if (ImGui::Combo("Gender Form", &currentForm, forms, IM_ARRAYSIZE(forms))) {
                    gene.form = static_cast<ActorState::Genetic>(currentForm);
                    changed = true;
                }
            }

            // --- Transform Attributes ---
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                float pos[3] = { gene.position.x, gene.position.y, gene.position.z };
                if (ImGui::SliderFloat3("Position", pos, -5.0f, 5.0f, "%.2f")) {
                    gene.position.x = pos[0];
                    gene.position.y = pos[1];
                    gene.position.z = pos[2];
                    changed = true;
                }

                float rot[3] = { gene.rotation.x, gene.rotation.y, gene.rotation.z };
                if (ImGui::SliderFloat3("Rotation", rot, -180.0f, 180.0f, "%.1f deg")) {
                    gene.rotation.x = rot[0];
                    gene.rotation.y = rot[1];
                    gene.rotation.z = rot[2];
                    changed = true;
                }

                float scale[3] = { gene.scale.x, gene.scale.y, gene.scale.z };
                if (ImGui::SliderFloat3("Scale", scale, 0.0f, 2.0f, "%.2f")) {
                    gene.scale.x = scale[0];
                    gene.scale.y = scale[1];
                    gene.scale.z = scale[2];
                    changed = true;
                }

                float offset[3] = { gene.offset.x, gene.offset.y, gene.offset.z };
                if (ImGui::SliderFloat3("Offset", offset, -5.0f, 5.0f, "%.2f")) {
                    gene.offset.x = offset[0];
                    gene.offset.y = offset[1];
                    gene.offset.z = offset[2];
                    changed = true;
                }
            }

            // --- Inheritance Settings ---
            if (ImGui::CollapsingHeader("Inheritance Indices")) {
                ImGui::TextDisabled("0 = Independent; 1..N = Inherits from Gene (Index - 1)");
                
                int colIdx = static_cast<int>(gene.colorIndex);
                if (ImGui::SliderInt("Color Index", &colIdx, 0, static_cast<int>(numGenes))) {
                    gene.colorIndex = static_cast<unsigned short>(colIdx);
                    changed = true;
                }

                int scaleIdx = static_cast<int>(gene.scaleIndex);
                if (ImGui::SliderInt("Scale Index", &scaleIdx, 0, static_cast<int>(numGenes))) {
                    gene.scaleIndex = static_cast<unsigned short>(scaleIdx);
                    changed = true;
                }

                int attachIdx = static_cast<int>(gene.attachmentIndex);
                if (ImGui::SliderInt("Attach Index", &attachIdx, 0, static_cast<int>(numGenes))) {
                    gene.attachmentIndex = static_cast<unsigned short>(attachIdx);
                    changed = true;
                }
            }

            // --- Animation Parameters ---
            if (ImGui::CollapsingHeader("Animation Setup")) {
                int animType = static_cast<int>(gene.animationType);
                const char* animTypes[] = { "Body (Static)", "Head", "Limb", "Limb Holding" };
                if (ImGui::Combo("Anim Type", &animType, animTypes, IM_ARRAYSIZE(animTypes))) {
                    gene.animationType = static_cast<ActorState::Animation>(animType);
                    changed = true;
                }

                if (ImGui::Checkbox("Inverse Phase", &gene.doInverseAnimation)) {
                    changed = true;
                }

                if (ImGui::SliderFloat("Swing Range", &gene.animationRange, 0.0f, 90.0f, "%.1f deg")) {
                    changed = true;
                }

                float axis[3] = { gene.animationAxis.x, gene.animationAxis.y, gene.animationAxis.z };
                if (ImGui::SliderFloat3("Swing Axis", axis, -1.0f, 1.0f, "%.2f")) {
                    gene.animationAxis.x = axis[0];
                    gene.animationAxis.y = axis[1];
                    gene.animationAxis.z = axis[2];
                    changed = true;
                }
            }

            // --- Growth & Expression Limits ---
            if (ImGui::CollapsingHeader("Growth & Phenotype Scaling")) {
                if (ImGui::SliderFloat("Factor", &gene.expressionFactor, 0.0f, 3.0f, "%.2f")) {
                    changed = true;
                }

                if (ImGui::SliderFloat("Max Scale", &gene.expressionMax, 0.0f, 5.0f, "%.2f")) {
                    changed = true;
                }

                int expAge = static_cast<int>(gene.expressionAge);
                if (ImGui::InputInt("Expression Age", &expAge)) {
                    gene.expressionAge = static_cast<unsigned int>(std::max(0, expAge));
                    changed = true;
                }
            }

            if (changed) {
                actorTarget->genetics.SetGeneFromGenome(static_cast<unsigned int>(selectedGeneIndex), gene);
                actorTarget->RebuildGeneticExpression();
                actorTarget->CalculateBoundingRegionFromGenome();
            }
        } else {
            ImGui::TextDisabled("Select a gene from the list on the left to edit.");
        }
    }
    ImGui::End();

    if (!isGeneEditorOpen) {
        CloseGeneEditor();
        return false;
    }

    return true;
}
