#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>
#include <cstdlib>
#include <cstring>
#include <cctype>

static inline void SkipWhitespace(const char*& ptr, const char* end) {
    while (ptr < end && (*ptr == ' ' || *ptr == '\t' || *ptr == '\r' || *ptr == '\n')) {
        ++ptr;
    }
}

static inline bool ParseNextFloat(const char*& ptr, const char* end, float& outVal) {
    SkipWhitespace(ptr, end);
    if (ptr >= end) return false;

    char* next = nullptr;
    outVal = std::strtof(ptr, &next);
    if (next == ptr) return false;

    ptr = next;
    SkipWhitespace(ptr, end);
    if (ptr < end && *ptr == ',') {
        ++ptr;
    }
    return true;
}

static inline bool ParseVec3(const char* start, const char* end, glm::vec3& outVec) {
    const char* ptr = start;
    if (!ParseNextFloat(ptr, end, outVec.x)) return false;
    if (!ParseNextFloat(ptr, end, outVec.y)) return false;
    if (!ParseNextFloat(ptr, end, outVec.z)) return false;
    return true;
}

unsigned int ChunkManager::AddPickupToMesh(Mesh* targetMesh, const std::string& itemClassification, const glm::vec3& position, const glm::vec3& rotation) {
    if (targetMesh == nullptr || itemClassification.empty()) 
        return 0;

    unsigned int initialSubMeshCount = targetMesh->GetSubMeshCount();

    glm::mat4 rotMat(1.0f);
    bool hasRotation = glm::length(rotation) > 0.0001f;
    if (hasRotation) {
        rotMat = glm::rotate(rotMat, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotMat = glm::rotate(rotMat, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        rotMat = glm::rotate(rotMat, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    const char* strPtr = itemClassification.c_str();
    const char* strEnd = strPtr + itemClassification.size();

    while (strPtr < strEnd) {
        SkipWhitespace(strPtr, strEnd);
        if (strPtr >= strEnd) break;

        const char* semiPos = std::strchr(strPtr, ';');
        const char* entryEnd = (semiPos != nullptr && semiPos < strEnd) ? semiPos : strEnd;

        if ((entryEnd - strPtr >= 5) && std::strncmp(strPtr, "build", 5) == 0) {
            const char* tokenStarts[5];
            const char* tokenEnds[5];
            size_t tokenCount = 0;

            const char* cur = strPtr;
            while (cur < entryEnd && tokenCount < 5) {
                const char* colon = (const char*)std::memchr(cur, ':', entryEnd - cur);
                tokenStarts[tokenCount] = cur;
                tokenEnds[tokenCount] = (colon != nullptr) ? colon : entryEnd;
                tokenCount++;

                if (colon == nullptr) break;
                cur = colon + 1;
            }

            if (tokenCount >= 5) {
                for (size_t t = 1; t < 5; ++t) {
                    SkipWhitespace(tokenStarts[t], tokenEnds[t]);
                    while (tokenEnds[t] > tokenStarts[t] && std::isspace((unsigned char)*(tokenEnds[t] - 1))) {
                        tokenEnds[t]--;
                    }
                }

                glm::vec3 subLocalPos(0.0f), subScale(1.0f), subColor(1.0f);
                if (ParseVec3(tokenStarts[2], tokenEnds[2], subLocalPos) &&
                    ParseVec3(tokenStarts[3], tokenEnds[3], subScale) &&
                    ParseVec3(tokenStarts[4], tokenEnds[4], subColor)) {

                    glm::vec3 rotatedOffset = hasRotation ? glm::vec3(rotMat * glm::vec4(subLocalPos, 1.0f)) : subLocalPos;
                    glm::vec3 finalPos = position + rotatedOffset;
                    Color colorVec(subColor.x, subColor.y, subColor.z);

                    size_t meshLen = tokenEnds[1] - tokenStarts[1];
                    if (meshLen == 4 && std::strncmp(tokenStarts[1], "cube", 4) == 0) {
                        targetMesh->AddCube(finalPos.x, finalPos.y, finalPos.z, subScale.x, subScale.y, subScale.z, colorVec);
                    } else {
                        std::string meshName(tokenStarts[1], meshLen);
                        auto meshIt = mStaticMeshes.find(meshName);
                        if (meshIt != mStaticMeshes.end()) {
                            int index = targetMesh->AddSubMesh(finalPos.x, finalPos.y, finalPos.z, meshIt->second, false);
                            targetMesh->ChangeSubMeshColor(index, colorVec);
                            targetMesh->ChangeSubMeshScale(index, subScale.x, subScale.y, subScale.z);
                            if (hasRotation) {
                                targetMesh->ChangeSubMeshRotation(index, rotation.x, glm::vec3(1, 0, 0));
                                targetMesh->ChangeSubMeshRotation(index, rotation.y, glm::vec3(0, 1, 0));
                                targetMesh->ChangeSubMeshRotation(index, rotation.z, glm::vec3(0, 0, 1));
                            }
                        }
                    }
                }
            }
        }

        strPtr = (semiPos != nullptr) ? semiPos + 1 : strEnd;
    }

    return targetMesh->GetSubMeshCount() - initialSubMeshCount;
}
