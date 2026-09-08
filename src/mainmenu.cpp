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

bool MainMenu(void) {
    if (!Platform.isPaused || actorTarget != nullptr || isInspectorOpen) 
        return true;
    
    const unsigned int buttonHeight = 24;
    const float bottomBarHeight = 40.0f;
    
    ImVec2 centerPos(Platform.windowArea.w * 0.5f, Platform.windowArea.h * 0.5f);
    
    ImGui::SetNextWindowPos(centerPos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(480, 370), ImGuiCond_Always);
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 255, 128, 255));
    
    ImGui::PushStyleColor(ImGuiCol_Tab,                ImVec4(0.12f, 0.12f, 0.12f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TabHovered,         ImVec4(0.20f, 0.40f, 0.28f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TabActive,          ImVec4(0.15f, 0.30f, 0.20f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TabUnfocused,       ImVec4(0.10f, 0.10f, 0.10f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, ImVec4(0.15f, 0.25f, 0.18f, 1.00f));
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding,     0.0f);
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | 
                             ImGuiWindowFlags_NoResize | 
                             ImGuiWindowFlags_NoCollapse;
    
    if (ImGui::Begin("CenteredWindow##ID", nullptr, flags)) {
        
        bool isWorldActive = !GameWorld.world.name.empty();
        
        static std::string selectedWorld = "";
        static bool isConfirmingDelete = false;
        static bool isConfirmingLoad = false;
        
        if (ImGui::BeginTabBar("MainMenuTabBar", ImGuiTabBarFlags_None)) {
            
            ImGuiTabItemFlags currentFlags   = (requestedTab == TAB_CURRENT)   ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None;
            ImGuiTabItemFlags newWorldFlags  = (requestedTab == TAB_NEW_WORLD)  ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None;
            ImGuiTabItemFlags loadWorldFlags = (requestedTab == TAB_LOAD_WORLD) ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None;
            
            // ==========================================
            // TAB Current World
            // ==========================================
            if (ImGui::BeginTabItem("Current", nullptr, currentFlags)) {
                if (requestedTab == TAB_NONE) 
                    activeTab = TAB_CURRENT;
                
                if (activeTab == TAB_CURRENT) {
                    if (isWorldActive) {
                        ImGui::Spacing();
                        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Active World '%s'", GameWorld.world.name.c_str());
                        ImGui::Text("Seed %s", Int.ToString(GameWorld.worldSeed).c_str());
                        ImGui::Separator();
                    } else {
                        const char* emptyMsg = "No active world";
                        ImVec2 textSize = ImGui::CalcTextSize(emptyMsg);
                        ImVec2 windowSize = ImGui::GetWindowSize();
                        ImGui::SetCursorPos(ImVec2((windowSize.x - textSize.x) * 0.5f, (windowSize.y - textSize.y) * 0.45f));
                        ImGui::TextDisabled("%s", emptyMsg);
                    }
                    
                    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - bottomBarHeight);
                    ImGui::Separator();
                    ImGui::Spacing();
                    
                    ImGui::BeginDisabled(!isWorldActive);
                    if (ImGui::Button("Save World", ImVec2(105, buttonHeight))) {
                        FuncSave({ GameWorld.world.name });
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Clear World", ImVec2(105, buttonHeight))) {
                        FuncClear({});
                        GameWorld.world.name = "";
                        selectedWorld.clear();
                        isConfirmingDelete = false;
                        isConfirmingLoad = false;
                    }
                    ImGui::EndDisabled();
                    
                    if (isWorldActive) {
                        ImGui::SameLine();
                        if (ImGui::Button("Resume", ImVec2(105, buttonHeight))) {
                            Platform.isPaused = false;
                            MainMenuDisable();
                        }
                    }
                }
                
                ImGui::EndTabItem();
            }
            
            // ==========================================
            // TAB New World
            // ==========================================
            if (ImGui::BeginTabItem("New world", nullptr, newWorldFlags)) {
                if (requestedTab == TAB_NONE) 
                    activeTab = TAB_NEW_WORLD;
                
                if (activeTab == TAB_NEW_WORLD) {
                    static char newWorldBuffer[64] = "New world";
                    static char seedBuffer[64] = "100";
                    static int newWorldSeed = 100;
                    static bool useRandomSeed = true;
                    
                    std::string targetWorldName(newWorldBuffer);
                    std::vector<std::string> dirList = fs.DirectoryGetList("worlds");
                    
                    // Find next available name with incremental suffix if duplicate
                    std::string finalWorldName = targetWorldName;
                    bool worldExists = false;
                    
                    for (const std::string& existingWorld : dirList) {
                        if (existingWorld == targetWorldName) {
                            worldExists = true;
                            break;
                        }
                    }
                    
                    if (worldExists && !targetWorldName.empty()) {
                        int counter = 1;
                        bool matchFound = true;
                        while (matchFound) {
                            finalWorldName = targetWorldName + " (" + Int.ToString(counter) + ")";
                            matchFound = false;
                            for (const std::string& existingWorld : dirList) {
                                if (existingWorld == finalWorldName) {
                                    matchFound = true;
                                    counter++;
                                    break;
                                }
                            }
                        }
                    }
                    
                    bool isNameInvalid = targetWorldName.empty();
                    
                    ImGui::Spacing();
                    ImGui::Text("Name");
                    ImGui::InputText("##NewWorldName", newWorldBuffer, IM_ARRAYSIZE(newWorldBuffer));
                    
                    if (targetWorldName.empty()) {
                        ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "World name cannot be empty.");
                    } else if (worldExists) {
                        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f), "Already exists. Renaming to '%s'", finalWorldName.c_str());
                    }
                    
                    ImGui::Spacing();
                    ImGui::Text("Seed ");
                    
                    if (useRandomSeed) {
                        char randomSeedPreview[64];
                        snprintf(randomSeedPreview, sizeof(randomSeedPreview), "%d", Random.Range(100000, 1000000));
                        
                        ImGui::BeginDisabled(true);
                        ImGui::InputText("##NewWorldSeed", randomSeedPreview, sizeof(randomSeedPreview), ImGuiInputTextFlags_ReadOnly);
                        ImGui::EndDisabled();
                    } else {
                        if (ImGui::InputText("##NewWorldSeed", seedBuffer, IM_ARRAYSIZE(seedBuffer), ImGuiInputTextFlags_CharsDecimal)) {
                            newWorldSeed = String.ToInt(seedBuffer);
                        }
                    }
                    
                    ImGui::Checkbox("Generate random seed", &useRandomSeed);
                    
                    // Pin button bar to bottom
                    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - bottomBarHeight);
                    ImGui::Separator();
                    ImGui::Spacing();
                    
                    ImGui::BeginDisabled(isNameInvalid);
                    if (ImGui::Button("Create", ImVec2(130, buttonHeight))) {
                        if (!useRandomSeed) {
                            GameWorld.worldSeed = newWorldSeed;
                        }
                        FuncLoad({ finalWorldName });
                        if (!useRandomSeed) {
                            GameWorld.worldSeed = newWorldSeed;
                            GameWorld.SaveWorld();
                        }
                        Platform.isPaused = false;
                        MainMenuDisable();
                    }
                    ImGui::EndDisabled();
                }
                
                ImGui::EndTabItem();
            }
            
            // ==========================================
            // TAB Load World
            // ==========================================
            if (ImGui::BeginTabItem("Load world", nullptr, loadWorldFlags)) {
                if (requestedTab == TAB_NONE) 
                    activeTab = TAB_LOAD_WORLD;
                
                if (activeTab == TAB_LOAD_WORLD) {
                    std::vector<std::string> dirList = fs.DirectoryGetList("worlds");
                    
                    ImGui::Spacing();
                    
                    if (dirList.empty()) {
                        ImGui::TextDisabled("No saved worlds found in 'worlds/' directory.");
                        selectedWorld.clear();
                        isConfirmingDelete = false;
                        isConfirmingLoad = false;
                    } else {
                        bool selectionExists = false;
                        for (const std::string& worldName : dirList) {
                            if (worldName == selectedWorld) {
                                selectionExists = true;
                                break;
                            }
                        }
                        if (!selectionExists) {
                            selectedWorld.clear();
                            isConfirmingDelete = false;
                            isConfirmingLoad = false;
                        }
                        
                        ImGui::BeginChild("WorldListScroll", ImVec2(0, 185), true);
                        for (const std::string& worldName : dirList) {
                            bool isSelected = (selectedWorld == worldName);
                            if (ImGui::Selectable(worldName.c_str(), isSelected)) {
                                selectedWorld = worldName;
                                isConfirmingDelete = false;
                                isConfirmingLoad = false;
                            }
                        }
                        ImGui::EndChild();
                    }
                    
                    ImGui::Spacing();
                    if (isConfirmingDelete && !selectedWorld.empty()) {
                        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Are you sure you want to delete '%s'?", selectedWorld.c_str());
                        ImGui::TextDisabled("This action permanently removes the world file.");
                    } else if (isConfirmingLoad && !selectedWorld.empty()) {
                        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f), "Loading '%s' will replace the current active world!", selectedWorld.c_str());
                        ImGui::TextDisabled("Any unsaved progress in the active world will be cleared.");
                    } else if (!selectedWorld.empty()) {
                        ImGui::TextDisabled("Selected: %s", selectedWorld.c_str());
                    }
                    
                    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - bottomBarHeight);
                    ImGui::Separator();
                    ImGui::Spacing();
                    
                    if (isConfirmingDelete && !selectedWorld.empty()) {
                        if (ImGui::Button("Confirm Delete", ImVec2(120, buttonHeight))) {
                            FuncRemove({ selectedWorld });
                            selectedWorld.clear();
                            isConfirmingDelete = false;
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Cancel", ImVec2(70, buttonHeight))) {
                            isConfirmingDelete = false;
                        }
                    } else if (isConfirmingLoad && !selectedWorld.empty()) {
                        if (ImGui::Button("Confirm Load", ImVec2(110, buttonHeight))) {
                            FuncClear({});
                            FuncLoad({ selectedWorld });
                            Platform.isPaused = false;
                            MainMenuDisable();
                            isConfirmingLoad = false;
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Cancel", ImVec2(70, buttonHeight))) {
                            isConfirmingLoad = false;
                        }
                    } else {
                        bool hasSelection = !selectedWorld.empty();
                        bool isSelectedWorldActive = isWorldActive && (selectedWorld == GameWorld.world.name);
                        
                        ImGui::BeginDisabled(!hasSelection);
                        if (ImGui::Button("Load", ImVec2(90, buttonHeight))) {
                            if (isWorldActive) {
                                isConfirmingLoad = true;
                                isConfirmingDelete = false;
                            } else {
                                FuncLoad({ selectedWorld });
                                Platform.isPaused = false;
                                MainMenuDisable();
                            }
                        }
                        ImGui::EndDisabled();
                        
                        ImGui::SameLine();
                        
                        ImGui::BeginDisabled(!hasSelection || isSelectedWorldActive);
                        if (ImGui::Button("Delete", ImVec2(90, buttonHeight))) {
                            isConfirmingDelete = true;
                            isConfirmingLoad = false;
                        }
                        ImGui::EndDisabled();
                    }
                }
                
                ImGui::EndTabItem();
            }
            
            requestedTab = TAB_NONE;
            
            ImGui::EndTabBar();
        }
        
    }
    ImGui::End();
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(6);
    
    return true;
}
