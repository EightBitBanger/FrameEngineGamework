#ifndef USER_INTERFACE_SYSTEM
#define USER_INTERFACE_SYSTEM

#include <GameEngineFramework/MemoryAllocation/poolallocator.h>

#include <GameEngineFramework/UI/components/text.h>
#include <GameEngineFramework/UI/components/textField.h>
#include <GameEngineFramework/UI/components/panel.h>
#include <GameEngineFramework/UI/components/sprite.h>

#ifndef SDL_Event
 #define SDL_Event  int
#endif


class ENGINE_API UserInterfaceSystem {
    
public:
    
    UserInterfaceSystem();
    
    void Initiate(void* windowHandle, void* deviceCntext);
    
    void Render(void);
    
    /// Event handlers
    bool ProcedureHandler(uintptr_t wHandle, uint32_t message, uintptr_t wParameter, uintptr_t lParameter);
    bool ProcedureHandlerSDL(SDL_Event event);
    
    /// Create a new text element and return its pointer.
    Text* CreateText(void);
    /// Destroy a text element and free its memory.
    bool DestroyText(Text* textPtr);
    
    /// Create a new text field element and return its pointer.
    TextField* CreateTextField(void);
    /// Destroy a text field element and free its memory.
    bool DestroyTextField(TextField* textFieldPtr);
    
    /// Create a new panel element and return its pointer.
    Panel* CreatePanel(void);
    /// Destroy a panel element and free its memory.
    bool DestroyPanel(Panel* panelPtr);
    
    /// Create a new sprite element and return its pointer.
    Sprite* CreateSprite(void);
    /// Destroy a sprite element and free its memory.
    bool DestroySprite(Sprite* spritePtr);
    
private:
    
    // Update systems
    void UpdateTextElements(void);
    void UpdateTextFieldElements(void);
    void UpdatePanelElements(void);
    void UpdateSpriteElements(void);
    
    // Element allocators
    PoolAllocator<Text> mTextElements;
    PoolAllocator<TextField> mTextFieldElements;
    PoolAllocator<Panel> mPanelElements;
    PoolAllocator<Sprite> mSpriteElements;
    
};


#endif
