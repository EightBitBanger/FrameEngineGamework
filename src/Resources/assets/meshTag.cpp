#include <GameEngineFramework/Resources/assets/meshTag.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Types/Types.h>

#include "../../../vendor/Bly7/OBJ_Loader.h"

extern Logger Log;
extern IntType Int;

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
    
    // Clear the sub mesh list
    Unload();
    
    // Load sub meshes
    for (unsigned int a=0; a < numberOfMeshes; a++) {
        
        SubMesh subMesh;
        
        subMesh.name        = loader.LoadedMeshes[a].MeshName;
        subMesh.vertexCount = loader.LoadedMeshes[a].Vertices.size();
        subMesh.indexCount  = loader.LoadedMeshes[a].Indices.size();
        
        // Format the vertex layout
        for (unsigned int i=0; i < loader.LoadedMeshes[a].Vertices.size(); i++) {
            
            objl::Vertex objlVertex = loader.LoadedMeshes[a].Vertices[i];
            
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
            
            subMesh.vertexBuffer.push_back(vertex);
        }
        
        for (unsigned int i=0; i < loader.LoadedMeshes[a].Indices.size(); i++) {
            
            subMesh.indexBuffer.push_back(loader.LoadedMeshes[a].Indices[i]);
            
        }
        
#ifdef EVENT_LOG_DETAILED
        std::string logstr = "  + " + subMesh.name + " " + Int.ToString( subMesh.vertexCount ) + " vertices";
        Log.Write(logstr);
#endif
        
        subMeshes.push_back( subMesh );
        
        continue;
    }
    
    isLoaded = true;
    return true;
}

bool MeshTag::Unload(void) {
    subMeshes.clear();
    isLoaded = false;
    return true;
}
