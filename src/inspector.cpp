#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>
#include <GameEngineFramework/Plugins/plugins.h>

extern MeshRenderer* boundsRenderer;
extern Actor* actorTarget;

enum MenuTab {
    TAB_NONE = 0,
    TAB_CURRENT,
    TAB_NEW_WORLD,
    TAB_LOAD_WORLD
};

static MenuTab activeTab = TAB_NEW_WORLD;
static MenuTab requestedTab = TAB_NEW_WORLD;
static bool isInspectorOpen = false;
extern bool IsGeneEditorOpen();
extern void CloseGeneEditor();

bool IsActorInspectorOpen() {
    return isInspectorOpen;
}

void OpenActorInspector(Actor* target) {
    if (target == nullptr) 
        return;
    
    // Close gene editor if it is currently open
    if (IsGeneEditorOpen()) {
        CloseGeneEditor();
    }
    
    if (actorTarget == nullptr) {
        UI.AddUIFrameCallback(InspectorWindow);
    }
    actorTarget = target;
    isInspectorOpen = true;
    
    Platform.ShowMouseCursor();
    if (Engine.sceneMain != nullptr && Engine.sceneMain->camera != nullptr) {
        Engine.sceneMain->camera->DisableMouseLook();
        Engine.sceneMain->camera->centerStandOff = 2.0f;
        Engine.sceneMain->camera->EnableThirdPersonLook();
    }
}

void CloseActorInspector() {
    if (boundsRenderer != nullptr) {
        boundsRenderer->isActive = false;
    }
    
    if (Engine.sceneMain != nullptr && Engine.sceneMain->camera != nullptr) {
        RestoreCameraControllerToOrbitalView(actorTarget);
        
        Engine.sceneMain->camera->DisableThirdPersonLook();
        Engine.sceneMain->camera->EnableMouseLook();
        Input.SetMousePosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
    }
    
    actorTarget = nullptr;
    isInspectorOpen = false;
    Platform.HideMouseCursor();
}

bool InspectorWindow(void) {
    // Ensure the window only executes if it is marked open and a target exists
    if (!isInspectorOpen || actorTarget == nullptr) 
        return false;
    
    ImGuiIO& io = ImGui::GetIO();
    const float windowWidth  = 320.0f;
    const float windowHeight = 520.0f;
    const float margin       = 24.0f;
    const ImVec4 headerColor(0.2f, 0.75f, 0.9f, 1.0f);
    const ImVec4 labelColor(0.6f, 0.8f, 1.0f, 1.0f);

    // Calculate left and right default positions
    const ImVec2 leftWindowPos(margin, margin);
    const ImVec2 rightWindowPos(io.DisplaySize.x - windowWidth - margin, margin);

    ImGui::SetNextWindowPos(leftWindowPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_FirstUseEver);
    
    if (ImGui::Begin("Actor Inspector", &isInspectorOpen)) {
        
        // Helper lambda for rendering horizontal stat bars
        auto RenderValueBar = [](const char* label, float value, const ImVec4& color) {
            const float barWidth = 90.0f;
            
            ImGui::Text("%s", label);
            ImGui::SameLine();
            
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - barWidth);
            
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.12f, 0.12f, 0.12f, 0.7f));
            
            char overlay[32];
            snprintf(overlay, sizeof(overlay), "%.2f", value);
            
            ImGui::ProgressBar(value, ImVec2(barWidth, 0.0f), overlay);
            
            ImGui::PopStyleColor(2);
        };
        
        // Match MainMenu tab visual theme
        ImGui::PushStyleColor(ImGuiCol_Tab,                ImVec4(0.12f, 0.12f, 0.12f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_TabHovered,         ImVec4(0.20f, 0.40f, 0.28f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_TabActive,          ImVec4(0.15f, 0.30f, 0.20f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_TabUnfocused,       ImVec4(0.10f, 0.10f, 0.10f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, ImVec4(0.15f, 0.25f, 0.18f, 1.00f));
        ImGui::PushStyleVar(ImGuiStyleVar_TabRounding,     0.0f);
        
        if (ImGui::BeginTabBar("ActorInspectorTabBar", ImGuiTabBarFlags_None)) {
            
            // ==========================================
            // Stats
            // ==========================================
            if (ImGui::BeginTabItem("Stats")) {
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                if (ImGui::Button("Kill Target")) {
                    actorTarget->biological.health = 0;
                }
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::TextColored(headerColor, "[%s]", actorTarget->GetName().c_str());
                ImGui::Text("Age  %d  |  %s", 
                            (int)actorTarget->physical.GetAge(), 
                            actorTarget->physical.GetSexualOrientation() ? "Male" : "Female");
                
                ImGui::Spacing();
                ImGui::TextColored(headerColor, "[Vitality]");
                ImGui::Separator();
                
                ImGui::Text("Health:     %.2f", actorTarget->biological.health);
                RenderValueBar("Hunger",     actorTarget->biological.hunger,     ImVec4(0.90f, 0.55f, 0.20f, 1.0f));
                RenderValueBar("Saturation", actorTarget->biological.saturation, ImVec4(0.25f, 0.75f, 0.85f, 1.0f));
                
                ImGui::Spacing();
                ImGui::TextColored(headerColor, "[Behavior]");
                ImGui::Separator();
                
                const char* mode = "unknown";
                switch (actorTarget->state.mode) {
                    case ActorState::Mode::Sleeping:      mode = "Sleeping"; break;
                    case ActorState::Mode::Idle:          mode = "Idling"; break;
                    case ActorState::Mode::MoveAttack:    mode = "Attacking"; break;
                    case ActorState::Mode::MoveFlee:      mode = "Fleeing"; break;
                    case ActorState::Mode::MoveRandom:    mode = "Wandering"; break;
                    case ActorState::Mode::MoveHunting:   mode = "Hunting"; break;
                    case ActorState::Mode::MoveSocialize: mode = "Socializing"; break;
                    case ActorState::Mode::MoveBreed:     mode = "Breeding"; break;
                    case ActorState::Mode::MoveTo:        mode = "Moving"; break;
                    case ActorState::Mode::RunTo:         mode = "Running"; break;
                    case ActorState::Mode::WalkTo:        mode = "Walking"; break;
                }
                ImGui::Text("State: %s", mode);
                
                ImGui::Spacing();
                
                if (ImGui::TreeNodeEx("Emotions", ImGuiTreeNodeFlags_DefaultOpen)) {
                    RenderValueBar("Anger",     actorTarget->emotions.current.anger,     ImVec4(0.90f, 0.25f, 0.25f, 1.0f));
                    RenderValueBar("Fear",      actorTarget->emotions.current.fear,      ImVec4(0.60f, 0.25f, 0.85f, 1.0f));
                    RenderValueBar("Comfort",   actorTarget->emotions.current.comfort,   ImVec4(0.25f, 0.85f, 0.45f, 1.0f));
                    RenderValueBar("Curiosity", actorTarget->emotions.current.curiosity, ImVec4(0.95f, 0.75f, 0.20f, 1.0f));
                    RenderValueBar("Fatigue",   actorTarget->emotions.current.fatigue,   ImVec4(0.40f, 0.50f, 0.70f, 1.0f));
                    RenderValueBar("Libido",    actorTarget->emotions.current.libido,    ImVec4(0.90f, 0.35f, 0.65f, 1.0f));
                    RenderValueBar("Stress",    actorTarget->emotions.current.stress,    ImVec4(0.95f, 0.50f, 0.15f, 1.0f));
                    RenderValueBar("Social",    actorTarget->emotions.current.social,    ImVec4(0.25f, 0.70f, 0.95f, 1.0f));
                    ImGui::TreePop();
                }
                
                if (ImGui::TreeNode("Cooldowns")) {
                    auto RenderCooldownStatus = [](const char* label, bool isCooling) {
                        ImGui::Text("%-10s", label);
                        ImGui::SameLine(100.0f);
                        if (isCooling) {
                            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Cooling Down");
                        } else {
                            ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Ready");
                        }
                    };
                    
                    RenderCooldownStatus("Attack",    actorTarget->counters.GetCoolDownAttack());
                    RenderCooldownStatus("Breeding",  actorTarget->counters.GetCoolDownBreeding());
                    RenderCooldownStatus("Movement",  actorTarget->counters.GetCoolDownMovement());
                    RenderCooldownStatus("Observe",   actorTarget->counters.GetCoolDownObservation());
                    RenderCooldownStatus("Socialize", actorTarget->counters.GetCoolDownSocial());
                    ImGui::TreePop();
                }
                
                if (ImGui::TreeNode("Locomotion State")) {
                    auto RenderState = [](const char* label, const char* condActive, const char* condInactive, bool isActive) {
                        ImGui::Text("%-10s", label);
                        ImGui::SameLine(100.0f);
                        if (isActive) {
                            ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "%s", condActive);
                        } else {
                            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "%s", condInactive);
                        }
                    };
                    
                    RenderState("Walking", "Walking", "Not walking", actorTarget->state.GetStateWalking());
                    RenderState("Running", "Running", "Not running", actorTarget->state.GetStateRunning());
                    RenderState("Facing",  "Facing",  "Not facing",  actorTarget->state.GetStateFacing());
                    ImGui::TreePop();
                }
                
                ImGui::EndTabItem();
            }
            
            // ==========================================
            // Traits
            // ==========================================
            if (ImGui::BeginTabItem("Traits")) {
                ImGui::Spacing();
                ImGui::TextColored(headerColor, "[Heritable Developmental Traits]");
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::Spacing();
                ImGui::TextColored(headerColor, "[Limits]");
                ImGui::Separator();
                
                ImGui::Text("  Youth Scale                   %.2f", actorTarget->physical.GetYouthScale());
                ImGui::Text("  Adult Scale                   %.2f", actorTarget->physical.GetAdultScale());
                
                ImGui::Spacing();
                ImGui::TextColored(headerColor, "[Maturity]");
                ImGui::Separator();
                
                ImGui::Text("  Adult Age                     %.0f ticks", actorTarget->physical.GetAdultAge());
                ImGui::Text("  Senior Age                    %.0f ticks", actorTarget->physical.GetSeniorAge());
                
                ImGui::Spacing();
                ImGui::TextColored(headerColor, "[Motion]");
                ImGui::Separator();
                
                ImGui::Text("  Youth Speed                   %.2f", actorTarget->physical.GetSpeedYouth());
                ImGui::Text("  Base Speed                    %.2f", actorTarget->physical.GetSpeed());
                ImGui::Text("  Run Multiplier                %.2f", actorTarget->physical.GetSpeedMultiplier());
                
                ImGui::Spacing();
                ImGui::TextColored(headerColor, "[Environmental Preferences]");
                ImGui::Separator();
                
                ImGui::Text("  Altitude min                  %.1f", actorTarget->behavior.GetHeightPreferenceMin());
                ImGui::Text("  Altitude max                  %.1f", actorTarget->behavior.GetHeightPreferenceMax());
                
                ImGui::EndTabItem();
            }
            
            // ==========================================
            // Genome
            // ==========================================
            if (ImGui::BeginTabItem("Genome")) {
                ImGui::Spacing();
                
                // Genome Actions
                if (ImGui::Button("Extract Genome")) {
                    Platform.SetClipboardText(AI.genomes.ExtractGenome(actorTarget));
                    std::string extractionMsg = actorTarget->GetName() + " genome extracted";
                    Engine.console.Print(extractionMsg);
                }
                ImGui::SameLine();
                if (ImGui::Button("Re-express Genetics")) {
                    actorTarget->RebuildGeneticExpression();
                    Engine.console.Print(actorTarget->GetName() + " genetic expression refreshed");
                }
                
                ImGui::Spacing();
                ImGui::TextColored(headerColor, "[Genetics Overview]");
                ImGui::Separator();
                
                unsigned int numGenes     = actorTarget->genetics.GetNumberOfGenes();
                unsigned int numPhens     = actorTarget->genetics.GetNumberOfPhens();
                unsigned int numRenderers = actorTarget->genetics.GetNumberOfMeshRenderers();
                
                ImGui::Text("Generation:        %u", actorTarget->genetics.GetGeneration());
                ImGui::Text("Gene Count:        %u", numGenes);
                ImGui::Text("Phenotype Count:   %u", numPhens);
                ImGui::Text("Active Renderers:  %u", numRenderers);
                
                ImGui::Spacing();
                ImGui::TextColored(headerColor, "[Blueprints & Expression]");
                ImGui::Separator();
                
                if (numGenes == 0) {
                    ImGui::TextDisabled("No genetic blueprints present.");
                } else {
                    // Fill the remaining tab area dynamically
                    ImGui::BeginChild("GeneListScroll", ImVec2(0, 0), true);
                    for (unsigned int i = 0; i < numGenes; ++i) {
                        Gene gene = actorTarget->genetics.GetGeneFromGenome(i);
                        Phen phen = actorTarget->genetics.GetPhenFromPhenotype(i);
                        
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
                        
                        if (ImGui::TreeNode(geneHeader)) {
                            ImGui::Text("Expressed:          %s", gene.doExpress ? "True" : "False");
                            ImGui::Text("Form Expression:    %s", formStr);
                            ImGui::Text("Animation Type:     %s", animStr);
                            
                            ImGui::Spacing();
                            ImGui::TextColored(labelColor, "Inheritance Links:");
                            ImGui::Text("  Attachment Index: %u %s", gene.attachmentIndex, 
                                        gene.attachmentIndex == 0 ? "(Root / Origin)" : "");
                            ImGui::Text("  Color Link Index: %u %s", gene.colorIndex, 
                                        gene.colorIndex == 0 ? "(Independent)" : "");
                            ImGui::Text("  Scale Link Index: %u %s", gene.scaleIndex, 
                                        gene.scaleIndex == 0 ? "(Independent)" : "");
                            
                            ImGui::Spacing();
                            ImGui::TextColored(labelColor, "Genotype Transform:");
                            ImGui::Text("  Position:         (%.2f, %.2f, %.2f)", gene.position.x, gene.position.y, gene.position.z);
                            ImGui::Text("  Rotation:         (%.2f, %.2f, %.2f)", gene.rotation.x, gene.rotation.y, gene.rotation.z);
                            ImGui::Text("  Scale:            (%.2f, %.2f, %.2f)", gene.scale.x, gene.scale.y, gene.scale.z);
                            ImGui::Text("  Joint Offset:     (%.2f, %.2f, %.2f)", gene.offset.x, gene.offset.y, gene.offset.z);
                            
                            ImGui::Spacing();
                            ImGui::TextColored(labelColor, "Color Specification:");
                            ImGui::Text("  Base (RGB):       (%.2f, %.2f, %.2f)", gene.color.x, gene.color.y, gene.color.z);
                            ImGui::SameLine();
                            ImVec4 colorPreview(gene.color.x, gene.color.y, gene.color.z, 1.0f);
                            ImGui::ColorButton("##GeneColorPreview", colorPreview, 
                                               ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker, 
                                               ImVec2(16, 16));
                            
                            ImGui::Spacing();
                            ImGui::TextColored(labelColor, "Phenotype Modifiers:");
                            ImGui::Text("  Phenotype Scale:  (%.2f, %.2f, %.2f)", phen.scale.x, phen.scale.y, phen.scale.z);
                            ImGui::Text("  Phenotype Tint:   (%.2f, %.2f, %.2f)", phen.color.x, phen.color.y, phen.color.z);
                            
                            ImGui::Spacing();
                            ImGui::TextColored(labelColor, "Expression & Dimorphism:");
                            ImGui::Text("  Expression Age:   %u ticks", gene.expressionAge);
                            ImGui::Text("  Factor / Max:     %.2f / %.2f", gene.expressionFactor, gene.expressionMax);
                            
                            ImGui::Spacing();
                            ImGui::TextColored(labelColor, "Animation Dynamics:");
                            ImGui::Text("  Swing Range:      %.1f deg", gene.animationRange);
                            ImGui::Text("  Swing Axis:       (%.2f, %.2f, %.2f)", gene.animationAxis.x, gene.animationAxis.y, gene.animationAxis.z);
                            ImGui::Text("  Invert Cycle:     %s", gene.doInverseAnimation ? "True" : "False");
                            
                            ImGui::TreePop();
                        }
                    }
                    ImGui::EndChild();
                }
                
                ImGui::EndTabItem();
            }
            
            // ==========================================
            // Memories
            // ==========================================
            if (ImGui::BeginTabItem("Memories")) {
                ImGui::Spacing();
                
                static int selectedMemoryIndex = -1;
                static Actor* lastActor = nullptr;
                static char newKey[64] = "";
                static char newVal[128] = "";
                static char editVal[256] = "";

                // Reset selection if target actor changes
                if (actorTarget != lastActor) {
                    lastActor = actorTarget;
                    selectedMemoryIndex = -1;
                    editVal[0] = '\0';
                }

                unsigned int memoryCount = actorTarget->memories.GetNumberOfMemories();
                if (selectedMemoryIndex >= (int)memoryCount) {
                    selectedMemoryIndex = -1;
                }

                bool hasSelection = (selectedMemoryIndex >= 0 && selectedMemoryIndex < (int)memoryCount);
                std::string selectedKey = hasSelection ? actorTarget->memories.GetMemoryNameByIndex(selectedMemoryIndex) : "";

                // Add Memory Section
                ImGui::TextColored(headerColor, "[Add Memory]");
                ImGui::SetNextItemWidth(110.0f);
                ImGui::InputTextWithHint("##NewKey", "Key", newKey, sizeof(newKey));
                ImGui::SameLine();
                ImGui::SetNextItemWidth(110.0f);
                ImGui::InputTextWithHint("##NewVal", "Value", newVal, sizeof(newVal));
                ImGui::SameLine();
                if (ImGui::Button("Add")) {
                    if (strlen(newKey) > 0) {
                        actorTarget->memories.Add(newKey, newVal);
                        newKey[0] = '\0';
                        newVal[0] = '\0';
                    }
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                // Selected Item Header & Modification Controls (Above the List)
                ImGui::TextColored(headerColor, "[Modify Selection]");
                if (hasSelection) {
                    ImGui::TextColored(labelColor, "Selected: %s", selectedKey.c_str());
                } else {
                    ImGui::TextDisabled("No memory selected");
                }

                const float deleteBtnWidth = 26.0f;
                const float saveBtnWidth   = 46.0f;
                const float spacing        = ImGui::GetStyle().ItemSpacing.x;
                const float inputWidth     = ImGui::GetContentRegionAvail().x - saveBtnWidth - deleteBtnWidth - (spacing * 2.0f);

                ImGui::SetNextItemWidth(inputWidth);
                if (hasSelection) {
                    bool enterPressed = ImGui::InputText("##EditVal", editVal, sizeof(editVal), ImGuiInputTextFlags_EnterReturnsTrue);
                    ImGui::SameLine();
                    if (ImGui::Button("Save", ImVec2(saveBtnWidth, 0.0f)) || enterPressed) {
                        actorTarget->memories.Add(selectedKey, std::string(editVal));
                    }
                } else {
                    char dummy[1] = "";
                    ImGui::InputTextWithHint("##EditValDisabled", "Select below to edit...", dummy, sizeof(dummy), ImGuiInputTextFlags_ReadOnly);
                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.18f, 0.18f, 0.18f, 0.50f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.18f, 0.18f, 0.18f, 0.50f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.18f, 0.18f, 0.18f, 0.50f));
                    ImGui::PushStyleColor(ImGuiCol_Text,          ImVec4(0.40f, 0.40f, 0.40f, 0.60f));
                    ImGui::Button("Save##Disabled", ImVec2(saveBtnWidth, 0.0f));
                    ImGui::PopStyleColor(4);
                }

                ImGui::SameLine();

                // Grayed-out 'X' that turns red when an entry is selected
                if (hasSelection) {
                    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.70f, 0.15f, 0.15f, 0.85f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.20f, 0.20f, 1.00f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.95f, 0.25f, 0.25f, 1.00f));
                    ImGui::PushStyleColor(ImGuiCol_Text,          ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
                    if (ImGui::Button("X##Delete", ImVec2(deleteBtnWidth, 0.0f))) {
                        actorTarget->memories.Remove(selectedKey);
                        selectedMemoryIndex = -1;
                        editVal[0] = '\0';
                        hasSelection = false;
                    }
                    ImGui::PopStyleColor(4);
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.18f, 0.18f, 0.18f, 0.50f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.18f, 0.18f, 0.18f, 0.50f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.18f, 0.18f, 0.18f, 0.50f));
                    ImGui::PushStyleColor(ImGuiCol_Text,          ImVec4(0.40f, 0.40f, 0.40f, 0.60f));
                    ImGui::Button("X##Disabled", ImVec2(deleteBtnWidth, 0.0f));
                    ImGui::PopStyleColor(4);
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                // Selectable Memory List (Fills the remaining tab space)
                ImGui::TextColored(headerColor, "[Stored Memories]");
                memoryCount = actorTarget->memories.GetNumberOfMemories();
                if (memoryCount == 0) {
                    ImGui::TextDisabled("No memories stored.");
                    selectedMemoryIndex = -1;
                } else {
                    ImGui::BeginChild("MemoriesSelectableList", ImVec2(0, 0), true);
                    for (unsigned int i = 0; i < memoryCount; i++) {
                        std::string name = actorTarget->memories.GetMemoryNameByIndex(i);
                        std::string val  = actorTarget->memories.GetMemoryValueByIndex(i);

                        char itemLabel[256];
                        snprintf(itemLabel, sizeof(itemLabel), "%s: %s", name.c_str(), val.c_str());

                        bool isSelected = (selectedMemoryIndex == (int)i);
                        if (ImGui::Selectable(itemLabel, isSelected)) {
                            selectedMemoryIndex = (int)i;
                            strncpy(editVal, val.c_str(), sizeof(editVal));
                            editVal[sizeof(editVal) - 1] = '\0';
                        }
                    }
                    ImGui::EndChild();
                }

                ImGui::EndTabItem();
            }
            
            // ==========================================
            // Inventory
            // ==========================================
            if (ImGui::BeginTabItem("Inventory")) {
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::TextColored(labelColor, "Equipped:");
                ImGui::SameLine();
                if (!actorTarget->inventory.inHandItemClass.empty()) {
                    ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "%s", actorTarget->inventory.inHandItemClass.c_str());
                } else {
                    ImGui::TextDisabled("None");
                }
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                const std::vector<std::string>& inventoryList = actorTarget->inventory.itemClassList;
                if (inventoryList.empty()) {
                    ImGui::TextDisabled("Inventory is empty.");
                } else {
                    ImGui::BeginChild("InventoryListScroll", ImVec2(0, 0), true);
                    for (size_t i = 0; i < inventoryList.size(); ++i) {
                        bool isEquipped = (inventoryList[i] == actorTarget->inventory.inHandItemClass);
                        if (isEquipped) {
                            ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "%s (Held)", inventoryList[i].c_str());
                        } else {
                            ImGui::Text("%s", inventoryList[i].c_str());
                        }
                    }
                    ImGui::EndChild();
                }
                
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
        
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(5);
    }
    ImGui::End();
    
    if (!isInspectorOpen) {
        CloseActorInspector();
        return false;
    }
    
    return true;
}
