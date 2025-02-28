//
// Render system enumerators

// Render queues

#define RENDER_QUEUE_OVERLAY      8
#define RENDER_QUEUE_FOREGROUND   7
#define RENDER_QUEUE_POSTGEOMETRY 6
#define RENDER_QUEUE_GEOMETRY     5
#define RENDER_QUEUE_PREGEOMETRY  4
#define RENDER_QUEUE_BACKGROUND   3
#define RENDER_QUEUE_SKY          2

// Fog layers

#define RENDER_FOG_LAYER_0        0
#define RENDER_FOG_LAYER_1        1
#define RENDER_FOG_LAYER_2        2
#define RENDER_FOG_LAYER_3        3

// Mesh primitive drawing types

#define  MESH_POINTS          GL_POINTS
#define  MESH_LINES           GL_LINES
#define  MESH_LINE_STRIP      GL_LINE_STRIP
#define  MESH_LINE_LOOP       GL_LINE_LOOP

#define  MESH_TRIANGLES       GL_TRIANGLES
#define  MESH_TRIANGLE_STRIP  GL_TRIANGLE_STRIP
#define  MESH_TRIANGLE_FAN    GL_TRIANGLE_FAN

#define  MESH_QUADS           GL_QUADS
#define  MESH_QUAD_STRIP      GL_QUAD_STRIP
#define  MESH_POLYGON         GL_POLYGON


// ============================================
// Material filtering

#define  MATERIAL_FILTER_NONE              GL_NEAREST
#define  MATERIAL_FILTER_LINEAR            GL_LINEAR
#define  MATERIAL_FILTER_TRILINEAR         GL_LINEAR_MIPMAP_NEAREST
#define  MATERIAL_FILTER_ANISOTROPIC       GL_LINEAR_MIPMAP_LINEAR

// Depth filter
#define  MATERIAL_DEPTH_NEVER              GL_NEVER
#define  MATERIAL_DEPTH_LESS               GL_LESS
#define  MATERIAL_DEPTH_LEQUAL             GL_LEQUAL
#define  MATERIAL_DEPTH_EQUAL              GL_EQUAL
#define  MATERIAL_DEPTH_NOTEQUAL           GL_NOTEQUAL
#define  MATERIAL_DEPTH_GEQUAL             GL_GEQUAL
#define  MATERIAL_DEPTH_GREATER            GL_GREATER
#define  MATERIAL_DEPTH_ALWAYS             GL_ALWAYS

// Edge wrapping
#define  MATERIAL_WRAP_REPEAT              GL_REPEAT
#define  MATERIAL_WRAP_MIRRORED_REPEAT     GL_MIRRORED_REPEAT
#define  MATERIAL_WRAP_CLAMP               GL_CLAMP_TO_EDGE
#define  MATERIAL_WRAP_CLAMP_BORDER        GL_CLAMP_TO_BORDER


// ============================================
// Blend functions

#define  BLEND_ZERO                      GL_ZERO
#define  BLEND_ONE                       GL_ONE
#define  BLEND_SRC_COLOR                 GL_SRC_COLOR
#define  BLEND_DST_COLOR                 GL_DST_COLOR
#define  BLEND_SRC_ALPHA                 GL_SRC_ALPHA
#define  BLEND_DST_ALPHA                 GL_DST_ALPHA
#define  BLEND_CONSTANT_COLOR            GL_CONSTANT_COLOR
#define  BLEND_CONSTANT_ALPHA            GL_CONSTANT_ALPHA
#define  BLEND_ONE_MINUS_SRC_COLOR       GL_ONE_MINUS_SRC_COLOR
#define  BLEND_ONE_MINUS_DST_COLOR       GL_ONE_MINUS_DST_COLOR
#define  BLEND_ONE_MINUS_SRC_ALPHA       GL_ONE_MINUS_SRC_ALPHA
#define  BLEND_ONE_MINUS_DST_ALPHA       GL_ONE_MINUS_DST_ALPHA
#define  BLEND_ONE_MINUS_CONSTANT_COLOR  GL_ONE_MINUS_CONSTANT_COLOR
#define  BLEND_ONE_MINUS_CONSTANT_ALPHA  GL_ONE_MINUS_CONSTANT_ALPHA

// Blend equations
#define  BLEND_EQUATION_MIN              GL_MIN
#define  BLEND_EQUATION_MAX              GL_MAX
#define  BLEND_EQUATION_ADD              GL_FUNC_ADD
#define  BLEND_EQUATION_SUB              GL_FUNC_SUBTRACT
#define  BLEND_EQUATION_REV_SUB          GL_FUNC_REVERSE_SUBTRACT


// ============================================
// Shaders

#define  SHADER_VERTEX                  GL_VERTEX_SHADER
#define  SHADER_FRAGMENT                GL_FRAGMENT_SHADER
//#define  SHADER_GEOMETRY                GL_GEOMETRY_SHADER
//#define  SHADER_COMPUTE                 GL_COMPUTE_SHADER
//#define  SHADER_TESS_CONTROL            GL_TESS_CONTROL_SHADER
//#define  SHADER_TESS_EVALUATION         GL_TESS_EVALUATION_SHADER

// Culling
#define  RENDER_CULL_FRONT               GL_FRONT
#define  RENDER_CULL_BACK                GL_BACK

// Winding
#define  RENDER_WINDING_CW               GL_CW
#define  RENDER_WINDING_CCW              GL_CCW


// ============================================
// Light types

#define  LIGHT_TYPE_POINT                0
#define  LIGHT_TYPE_DIRECTIONAL          1
#define  LIGHT_TYPE_SPOT                 2


