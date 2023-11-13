#include "text.h"

Text::Text() : 
    
    text(""),
    
    spriteAtlasWidth(15),
    spriteAtlasHeight(15),
    
    spriteWidth(0.03127),
    spriteHeight(0.0274),
    spriteStartX(0),
    spriteStartY(-0.003),
    
    spacingWidth(0.6),
    spacingHeight(0.9)
{
    color = Color(0, 0, 0);
    return;
}

