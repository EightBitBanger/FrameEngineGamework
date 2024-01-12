#include <GameEngineFramework/Renderer/components/framebuffer.h>

#define GLEW_STATIC
#include <gl/glew.h>


FrameBuffer::FrameBuffer() {
    glGenFramebuffers(1, &mFrameBuffer);
    return;
}

FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &mFrameBuffer);
    return;
}

void FrameBuffer::Bind(void) {
    glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
    return;
}
