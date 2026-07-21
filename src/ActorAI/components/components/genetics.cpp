#include <GameEngineFramework/ActorAI/ActorSystem.h>

GeneticsSystem::GeneticsSystem() : 
    mDoUpdateGenetics(false),
    mDoReexpressGenetics(false),
    mGeneration(0)
{}

unsigned int GeneticsSystem::AddGene(Gene& newGene) {
    mGenes.push_back( newGene );
    return mGenes.size();
}

void GeneticsSystem::RemoveGene(unsigned int index) {
    mGenes.erase( mGenes.begin() + index );
}

void GeneticsSystem::ClearGenome(void) {
    mGenes.clear();
}

void GeneticsSystem::ClearPhenome(void) {
    mPhen.clear();
}

unsigned int GeneticsSystem::GetNumberOfGenes(void) {
    return mGenes.size();
}

Gene GeneticsSystem::GetGeneFromGenome(unsigned int index) {
    if (index < mGenes.size()) 
        return mGenes[index];
    Gene gene;
    gene.color.x = -1.0f;
    return gene;
    
}

Phen GeneticsSystem::GetPhenFromPhenotype(unsigned int index) {
    if (index < mPhen.size()) 
        return mPhen[index];
    Phen phen;
    phen.color.x = -1.0f;
    return phen;
}

void GeneticsSystem::SetGeneration(unsigned int newGeneration) {
    mGeneration = newGeneration;
}

unsigned int GeneticsSystem::GetGeneration(void) {
    return mGeneration;
}

unsigned int GeneticsSystem::GetNumberOfMeshRenderers(void) {
    return mGeneticRenderers.size();
}

MeshRenderer* GeneticsSystem::GetMeshRendererAtIndex(unsigned int index) {
    if (index < mGeneticRenderers.size()) 
        return mGeneticRenderers[index];
    return nullptr;
}
