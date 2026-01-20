#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/UI/UserInterfaceSystem.h>
#include <GameEngineFramework/Application/Platform.h>

#include <GameEngineFramework/configuration.h>

#ifdef PLATFORM_WINDOWS
  #ifndef _WIN32_WINNT
  #define _WIN32_WINNT 0x500
  #endif
  
  #define WIN32_LEAN_AND_MEAN
  
  #include <sdkddkver.h>
  #include <windows.h>
  
  #include "ImGUI/imgui.h"
  #include "ImGUI/backends/imgui_impl_win32.h"
  #include "ImGUI/backends/imgui_impl_opengl3.h"
  
  extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
  
#endif

#ifdef PLATFORM_LINUX
  #include "imgui.h"
  #include "backends/imgui_impl_sdl3.h"
  #include "backends/imgui_impl_opengl3.h"
#endif

extern PlatformLayer Platform;


void UserInterfaceSystem::Initiate(void* windowHandle, void* deviceCntext) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    
    // Custom style
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 1.0f;
    style.FrameBorderSize = 1.0f;
    
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_FrameBg]        = ImVec4(0.15f, 0.15f, 0.15f, 0.85f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.2f, 0.95f);
    colors[ImGuiCol_FrameBgActive]  = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_Text]           = ImVec4(0.87f, 0.87f, 0.87f, 0.9f);
    colors[ImGuiCol_Border]         = ImVec4(0.3f, 0.8f, 0.3f, 0.5f);
    
#ifdef PLATFORM_WINDOWS
    ImGui_ImplWin32_Init( (HWND)windowHandle );
#endif
    
#ifdef PLATFORM_LINUX
    ImGui_ImplSDL3_InitForOpenGL( (SDL_Window*)windowHandle, (SDL_GLContext*)deviceCntext );
#endif
    
    ImGui_ImplOpenGL3_Init("#version 330 core");
}


uint8_t initiate_counter=0;

void UserInterfaceSystem::Render(void) {
    // Initiation trip
    if (initiate_counter < 10) {
        initiate_counter++;
        return;
    }
    
    glDisable(GL_DEPTH_TEST);
    
#ifdef PLATFORM_WINDOWS
    ImGui_ImplWin32_NewFrame();
#endif
    
#ifdef PLATFORM_LINUX
    ImGui_ImplSDL3_NewFrame();
#endif
    
    ImGui_ImplOpenGL3_NewFrame();
    
    ImGui::NewFrame();
    
    UpdatePanelElements();
    UpdateSpriteElements();
    UpdateTextFieldElements();
    UpdateTextElements();
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    glEnable(GL_DEPTH_TEST);
}


bool UserInterfaceSystem::ProcedureHandler(uintptr_t wHandle, uint32_t message, uintptr_t wParameter, uintptr_t lParameter) {
    
#ifdef PLATFORM_WINDOWS
    HWND hWnd   = reinterpret_cast<HWND>(wHandle);
    UINT msg    = static_cast<UINT>(message);
    WPARAM wParam = static_cast<WPARAM>(wParameter);
    LPARAM lParam = static_cast<LPARAM>(lParameter);
    
    ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
#endif
    
    return false;
}

bool UserInterfaceSystem::ProcedureHandlerSDL(SDL_Event event) {
#ifdef PLATFORM_LINUX
    return ImGui_ImplSDL3_ProcessEvent(&event);
#endif
    return false;
}

static char console_string[128] = "";

void UserInterfaceSystem::UpdateTextElements(void) {
    
    for (unsigned int i=0; i < mTextElements.Size(); i++) {
        Text* text = mTextElements[i];
        
        // Fade out
        if (text->doFadeout) {
            text->color.a -= 0.004f;
            
            if (text->color.a < 0.0f) {
                text->color.a = 0.0f;
                text->isActive = false;
            }
        }
        
        if (!text->isActive)
            continue;
        
        float alpha = text->color.a;
        if (alpha > 1.0f) 
            alpha = 1.0f;
        
        ImVec2 pos(text->x, text->y);
        
        ImGui::GetBackgroundDrawList()->AddText(pos, IM_COL32(text->color.r * 255, 
                                                              text->color.g * 255, 
                                                              text->color.b * 255, 
                                                              alpha * 255), text->text.c_str());
        
    }
}

void UserInterfaceSystem::UpdateTextFieldElements(void) {
    
    for (unsigned int i=0; i < mTextFieldElements.Size(); i++) {
        TextField* textField = mTextFieldElements[i];
        if (!textField->isActive)
            continue;
        
        // Remove padding and borders
        
        ImGui::SetNextWindowPos( ImVec2(textField->x, textField->y) );
        ImGui::SetNextWindowSize( ImVec2(textField->w, textField->h) );
        
        ImGui::Begin("##hiddenlabel", nullptr, 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoScrollbar | 
        ImGuiWindowFlags_NoSavedSettings | 
        ImGuiWindowFlags_NoFocusOnAppearing | 
        ImGuiWindowFlags_NoNav);
        
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        
        // Kill background and border colors
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_BorderShadow, IM_COL32(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(textField->color.r * 255, 
                                                      textField->color.g * 255, 
                                                      textField->color.b * 255, 
                                                      textField->color.a * 255));
        
        if (textField->text.size() < 1024) 
            textField->text.resize(1024, '\0');
        
        if (textField->doStayInFocus) 
            ImGui::SetKeyboardFocusHere();
        
        if (ImGui::InputText("##hiddenlabel", (char*)textField->text.data(), textField->text.size(), 
                             ImGuiInputTextFlags_EnterReturnsTrue)) {
            
            std::size_t pos = textField->text.find('\0');
            std::string subString;
            
            if (pos != std::string::npos) 
                subString = textField->text.substr(0, pos);
            
            textField->callback(subString);
        }
        
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(4);
        
        ImGui::End();
    }
}

void UserInterfaceSystem::UpdatePanelElements(void) {
    for (unsigned int i=0; i < mPanelElements.Size(); i++) {
        Panel* panel = mPanelElements[i];
        if (!panel->isActive) 
            continue;
        
        ImVec2 pos(panel->x, panel->y);
        ImVec2 sz(panel->w, panel->h);
        ImGui::SetNextWindowBgAlpha(panel->alpha);
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(sz, ImGuiCond_Always);
        
        ImGui::Begin(panel->name.c_str(), nullptr, 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoScrollbar | 
        ImGuiWindowFlags_NoSavedSettings | 
        ImGuiWindowFlags_NoFocusOnAppearing | 
        ImGuiWindowFlags_NoNav);
        
        ImGui::End();
    }
}

void UserInterfaceSystem::UpdateSpriteElements() {
    for (unsigned int i = 0; i < mSpriteElements.Size(); i++) {
        Sprite* sprite = mSpriteElements[i];
        if (!sprite->isActive || sprite->texture == nullptr) 
            continue;
        
        ImVec2 p0((float)sprite->x, (float)sprite->y);
        ImVec2 p1((float)(sprite->x + sprite->w),(float)(sprite->y + sprite->h));
        ImVec2 uv0(sprite->u0, sprite->v0);
        ImVec2 uv1(sprite->u1, sprite->v1);
        
        ImU32 tintCol = IM_COL32(
            (int)(sprite->tint.r * 255),
            (int)(sprite->tint.g * 255),
            (int)(sprite->tint.b * 255),
            (int)(sprite->tint.a * 255)
        );
        
        ImTextureID handle = (ImTextureID)sprite->texture->GetHandle();
        ImGui::GetBackgroundDrawList()->AddImage(handle, p0, p1, uv0, uv1, tintCol);
    }
}

Text* UserInterfaceSystem::CreateText(void) {
    return mTextElements.Create();
}

bool UserInterfaceSystem::DestroyText(Text* textPtr) {
    return mTextElements.Destroy(textPtr);
}

TextField* UserInterfaceSystem::CreateTextField(void) {
    return mTextFieldElements.Create();
}

bool UserInterfaceSystem::DestroyTextField(TextField* textFieldPtr) {
    return mTextFieldElements.Destroy(textFieldPtr);
}

Panel* UserInterfaceSystem::CreatePanel(void) {
    return mPanelElements.Create();
}

bool UserInterfaceSystem::DestroyPanel(Panel* panelPtr) {
    return mPanelElements.Destroy(panelPtr);
}

Sprite* UserInterfaceSystem::CreateSprite(void) {
    return mSpriteElements.Create();
}

bool UserInterfaceSystem::DestroySprite(Sprite* spritePtr) {
    return mSpriteElements.Destroy(spritePtr);
}
