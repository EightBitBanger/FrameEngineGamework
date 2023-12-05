#include <GameEngineFramework/Resources/assets/meshTag.h>
#include <GameEngineFramework/Logging/Logging.h>

#include "../../../vendor/Bly7/OBJ_Loader.h"

extern Logger Log;

MeshTag::MeshTag() : 
    
    path(""),
    name(""),
    
    isLoaded(false)
    
{
}

bool MeshTag::Load(void) {
    
    objl::Loader loader;
    if (!loader.LoadFile(path)) 
        return false;
    
    unsigned int numberOfMeshes = loader.LoadedMeshes.size();
    if (numberOfMeshes == 0) 
        return false;
    
    Unload();
    
    mesh.vertexCount = loader.LoadedMeshes[0].Vertices.size();
    mesh.indexCount = loader.LoadedMeshes[0].Indices.size();
    
    // Format the vertex layout
    for (unsigned int i=0; i < loader.LoadedMeshes[0].Vertices.size(); i++) {
        
        objl::Vertex objlVertex = loader.LoadedMeshes[0].Vertices[i];
        
        Vertex vertex;
        vertex.x = objlVertex.Position.X;
        vertex.y = objlVertex.Position.Y;
        vertex.z = objlVertex.Position.Z;
        vertex.r = 1;
        vertex.g = 1;
        vertex.b = 1;
        vertex.nx = objlVertex.Normal.X;
        vertex.ny = objlVertex.Normal.Y;
        vertex.nz = objlVertex.Normal.Z;
        vertex.u = objlVertex.TextureCoordinate.X;
        vertex.v = objlVertex.TextureCoordinate.Y;
        
        mesh.vertexBuffer.push_back(vertex);
    }
    
    for (unsigned int i=0; i < loader.LoadedMeshes[0].Indices.size(); i++) {
        mesh.indexBuffer.push_back(loader.LoadedMeshes[0].Indices[i]);
    }
    
#ifdef EVENT_LOG_DETAILED
    std::string logstr = "  + " + name + " " + IntToString( mesh.vertexCount ) + " vertices";
    Log.Write(logstr);
#endif
    
    isLoaded = true;
    return true;
}

bool MeshTag::Unload(void) {
    mesh.vertexCount = 0;
    mesh.indexCount = 0;
    
    mesh.vertexBuffer.clear();
    mesh.indexBuffer.clear();
    isLoaded = false;
    return true;
}
