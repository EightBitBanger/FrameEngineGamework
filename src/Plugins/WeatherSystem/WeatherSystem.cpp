#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/Plugins/plugins.h>

#include <GameEngineFramework/Plugins/WeatherSystem/WeatherSystem.h>

WeatherSystem::WeatherSystem() :
    weatherStateCounter(320),
    
    mWorldTime(0.0f),
    mTimeScale(0.25f),
    
    mWorldLightLow(0.0087f),
    mWorldLightHigh(0.87f),
    
    mSkyLightLow(0.0087f),
    mSkyLightHigh(0.87f),
    
    mLightIntensity(1.0f),
    mLightAngle(glm::vec3(0)),
    
    mAmbientColor(Colors.MakeGrayScale(0.1f)),
    
    mPlayerTransform(nullptr),
    
    mSunObject(nullptr),
    mSunLight(nullptr),
    mLightTransform(nullptr),
    mSunRenderer(nullptr),
    
    mSkyObject(nullptr),
    mSkyMaterial(nullptr),
    
    mWorldMaterial(nullptr),
    mStaticMaterial(nullptr),
    mWaterMaterial(nullptr),
    
    mWorldAmbientBase(Colors.ltgray),
    mStaticAmbientBase(Colors.ltgray),
    mCapturedAmbientBase(false),
    
    mNextWeather(WeatherType::Clear),
    mCurrentWeather(WeatherType::Clear),
    
    mWeatherMasterCounter(0.0f),
    mWeatherShiftCounter(0.0f),
    mWeatherFogCounter(1.0f),
    
    mFogWorld(nullptr),
    
    mWorldFogDensity(0.0f),
    mWorldFogNear(0.0f),
    mWorldFogFar(0.0f),
    mWorldFogColorNear(Colors.ltgray),
    mWorldFogColorFar(Colors.gray),
    
    mFogLightBias(0.0f),
    
    mWorldWaterLevel(-10.0f),
    mFogWater(nullptr),
    
    mRainEmitter(nullptr),
    mSnowEmitter(nullptr)
{
}

void WeatherSystem::Initiate(void) {
    // Sun tilt angle (yaw tilt on horizon)
    mLightAngle = glm::vec3(0.0f, -15.0f, 0.0f);
    
    mSunObject = Engine.Create<GameObject>();
    mSunObject->AddComponent(Engine.CreateComponent<Light>());
    mLightTransform = mSunObject->GetComponent<Transform>();
    
    // Set directional light parameters
    mSunLight = mSunObject->GetComponent<Light>();
    mSunLight->type      = LIGHT_TYPE_DIRECTIONAL;
    mSunLight->intensity = 0.0f;
    mSunLight->color     = Colors.white;
    Engine.sceneMain->AddLightToSceneRoot(mSunLight);
    
    // Visual Sun Renderer
    mSunRenderer = Engine.Create<MeshRenderer>();
    mSunRenderer->mesh = Engine.Create<Mesh>();
    mSunRenderer->mesh->AddSphere(0.0f, 0.0f, 0.0f, 45.0f, 16, 16, Colors.white, false);
    mSunRenderer->mesh->Load();
    
    mSunRenderer->material = Engine.Create<Material>();
    mSunRenderer->material->shader = Resources.shaders.colorUnlit;
    mSunRenderer->material->ambient = Colors.white;
    mSunRenderer->material->diffuse = Colors.white;
    
    mSunRenderer->material->DisableDepthTest();
    mSunRenderer->material->DisableShadowVolumePass();
    mSunRenderer->material->DisableCulling();
    mSunRenderer->DisableFrustumCulling();
    
    Engine.sceneMain->AddMeshRendererToSceneRoot(mSunRenderer, RENDER_QUEUE_BACKGROUND);
    mSunRenderer->isActive = true;
    
    // Sky
    float colorBias = 1.0f;
    mSkyObject = Weather.CreateSky("sky", Colors.blue, Colors.blue, colorBias);
    Engine.sceneMain->AddMeshRendererToSceneRoot(mSkyObject->GetComponent<MeshRenderer>(), RENDER_QUEUE_SKY);
    
    MeshRenderer* skyRenderer = mSkyObject->GetComponent<MeshRenderer>();
    mSkyMaterial = skyRenderer->material;
    mSkyMaterial->diffuse = Colors.dkgray;
    mSkyMaterial->ambient = Colors.black;
    
    // World fog
    mFogWorld = Renderer.CreateFog();
    Engine.sceneMain->AddFogLayerToScene(mFogWorld);
    mFogWorld->fogActive = true;
    mFogWorld->fogDensity = mWorldFogDensity;
    mFogWorld->fogBegin = mWorldFogNear;
    mFogWorld->fogEnd = mWorldFogFar;
    mFogWorld->fogColorBegin = mWorldFogColorNear;
    mFogWorld->fogColorEnd = mWorldFogColorFar;
    
    // Rain emitter
    mRainEmitter = Particle.CreateEmitter();
    mRainEmitter->type = EmitterType::AreaEffector;
    mRainEmitter->position = glm::vec3(0, 0, 0);
    mRainEmitter->direction = glm::vec3(0, -0.9f, 0);
    mRainEmitter->scale = glm::vec3(0.012f, 0.4f, 0.012f);
    mRainEmitter->velocity = glm::vec3(0.0f, -0.8f, 0.0f);
    mRainEmitter->width = 40.0f;
    mRainEmitter->height = 70.0f;
    mRainEmitter->colorBegin = Colors.Lerp(Colors.blue, Colors.ltgray, 0.2f);
    mRainEmitter->colorEnd = Colors.Lerp(Colors.blue, Colors.ltgray, 0.7f);
    mRainEmitter->maxParticles = 2000;
    mRainEmitter->heightMinimum = 0.0f;
    
    // Snow emitter
    mSnowEmitter = Particle.CreateEmitter();
    mSnowEmitter->type = EmitterType::AreaEffector;
    mSnowEmitter->position = glm::vec3(0, 0, 0);
    mSnowEmitter->direction = glm::vec3(0, -0.3f, 0);
    mSnowEmitter->scale = glm::vec3(0.05f, 0.02f, 0.05f);
    mSnowEmitter->velocity = glm::vec3(0.0f, -0.03f, 0.0f);
    mSnowEmitter->width = 30.0f;
    mSnowEmitter->height = 70.0f;
    mSnowEmitter->colorBegin = Colors.white;
    mSnowEmitter->colorEnd = Colors.white;
    mSnowEmitter->maxParticles = 2000;
    mSnowEmitter->heightMinimum = 0.0f;
    
    mFogWater = Renderer.CreateFog();
    SetWeather(WeatherType::Clear);
    UpdateSunAndLighting();
}

void WeatherSystem::Update(void) {
    mWorldTime += mTimeScale * 0.25f;
    while (mWorldTime >= 24000.0f)
        mWorldTime -= 24000.0f;
    while (mWorldTime < 0.0f)
        mWorldTime += 24000.0f;
    
    UpdateSunAndLighting();
    
    // Smoothly update light bias and fog
    mFogLightBias = Float.Lerp(mFogLightBias, mLightIntensity, 0.18f);
    
    // Calculate target fog colors driven by world light bias without corrupting base targets
    Color targetColorNear = Colors.Lerp(Colors.black, mWorldFogColorNear, mFogLightBias);
    Color targetColorFar  = Colors.Lerp(Colors.black, mWorldFogColorFar,  mFogLightBias);
    
    // Smoothly transition fog properties frame-by-frame
    float fogBlendRate = 0.05f;
    mFogWorld->fogDensity    = Float.Lerp(mFogWorld->fogDensity,    mWorldFogDensity,   fogBlendRate);
    mFogWorld->fogBegin      = Float.Lerp(mFogWorld->fogBegin,      mWorldFogNear,      fogBlendRate);
    mFogWorld->fogEnd        = Float.Lerp(mFogWorld->fogEnd,        mWorldFogFar,       fogBlendRate);
    mFogWorld->fogColorBegin = Colors.Lerp(mFogWorld->fogColorBegin, targetColorNear,    fogBlendRate);
    mFogWorld->fogColorEnd   = Colors.Lerp(mFogWorld->fogColorEnd,   targetColorFar,     fogBlendRate);
    
    // Shift to the next weather cycle
    if (mNextWeather != mCurrentWeather) {
        // Begin shift
        if (mWeatherShiftCounter == 0.0f) {
            AddWeather(mNextWeather);
        }
        
        mWeatherShiftCounter++;
        if (mWeatherShiftCounter > weatherStateCounter) {
            mWeatherShiftCounter = 0.0f;
            // Finish shift and finalize emitters
            SetWeather(mNextWeather);
        }
    }
    
    // Water underwater effect
    if (mPlayerTransform != nullptr) {
        if (mPlayerTransform->position.y < mWorldWaterLevel) {
            mFogWater->fogHeightCutoff = 1000.0f;
            mFogWater->fogDensity = 0.8f;
            mFogWater->fogBegin = 0.0f;
            mFogWater->fogEnd = 24.0f;
            mFogWater->fogColorBegin = Colors.blue;
            mFogWater->fogColorEnd = Colors.blue;
        } else {
            mFogWater->fogHeightCutoff = mWorldWaterLevel;
            mFogWater->fogDensity = 0.8f;
            mFogWater->fogBegin = 0.0f;
            mFogWater->fogEnd = 1.0f;
            mFogWater->fogColorBegin = Colors.blue;
            mFogWater->fogColorEnd = Colors.blue;
        }
    }
}
void WeatherSystem::UpdateSunAndLighting(void) {
    float fullDayRange = mWorldTime / 24000.0f;
    
    // Calculate continuous solar orbit angle in radians:
    // Sunrise (tick 6000)  = -PI/2
    // Noon    (tick 12000) =  0
    // Sunset  (tick 18000) = +PI/2
    // Midnight(tick 0/24000)= ±PI
    float solarAngleRad = glm::radians((fullDayRange * 360.0f) - 180.0f);
    float tiltRad       = glm::radians(mLightAngle.y);
    
    // Compute directional vector pointing from scene center toward the sun
    glm::vec3 sunDirection = glm::normalize(glm::vec3(
        -sinf(solarAngleRad) * cosf(tiltRad), // East (+X) to West (-X)
         cosf(solarAngleRad),                  // Altitude (Zenith at noon, negative at night)
         sinf(tiltRad) * cosf(solarAngleRad)   // Azimuth slant
    ));
    
    // Align light transform pitch and yaw without roll flips
    mLightAngle.x = glm::degrees(solarAngleRad);
    mLightTransform->SetIdentity();
    mLightTransform->RotateEuler(mLightAngle.x, mLightAngle.y, 0.0f);
    
    // Compute daytime intensity based on elevation above horizon
    if (sunDirection.y > 0.0f) {
        mLightIntensity = glm::clamp(sunDirection.y * 4.0f, 0.0f, 1.0f);
    } else {
        mLightIntensity = 0.0f;
    }
    
    // Update directional light component
    if (mSunLight != nullptr) {
        mSunLight->intensity = Float.Lerp(mWorldLightLow, mWorldLightHigh, mLightIntensity);
        // Direction vector traveling from the sun into the scene
        mSunLight->direction = -sunDirection;
    }
    
    // Visual sun mesh positioning
    if (mSunRenderer != nullptr) {
        //mSunRenderer->isActive = true;
            
            glm::vec3 centerPos = (mPlayerTransform != nullptr) ? mPlayerTransform->position : glm::vec3(0.0f);
            const float sunDistance = 1500.0f;
            glm::vec3 sunPosition = centerPos + (sunDirection * sunDistance);
            
            mSunRenderer->transform.SetIdentity();
            mSunRenderer->transform.Translate(sunPosition);
            mSunRenderer->transform.position = sunPosition;
            
            Color sunsetColor     = Colors.red;
            Color noonColor       = Colors.Lerp(Colors.yellow, Colors.white, 0.2f);
            Color currentSunColor = Colors.Lerp(sunsetColor, noonColor, mLightIntensity);
            
            mSunRenderer->material->ambient = currentSunColor;
            mSunRenderer->material->diffuse = currentSunColor;
    }
    
    // World & static ambient lighting updates
    if (mWorldMaterial != nullptr && mStaticMaterial != nullptr) {
        if (!mCapturedAmbientBase) {
            mWorldAmbientBase    = mWorldMaterial->ambient;
            mStaticAmbientBase   = mStaticMaterial->ambient;
            mCapturedAmbientBase = true;
        }
        
        Color ambientWorld  = Colors.Lerp(mAmbientColor, mWorldAmbientBase,  mLightIntensity);
        Color ambientStatic = Colors.Lerp(mAmbientColor, mStaticAmbientBase, mLightIntensity);
        
        mWorldMaterial->ambient  = ambientWorld;
        mStaticMaterial->ambient = ambientStatic;
    }
    
    // Sky dome coloring
    if (mSkyMaterial != nullptr) {
        float skyColor = Math.Lerp(mSkyLightLow, mSkyLightHigh, mLightIntensity);
        SetSkyAmbientColor(Colors.MakeGrayScale(skyColor));
    }
}

void WeatherSystem::SetSkyAmbientColor(Color skyColor) {
    mSkyMaterial->ambient = skyColor;
}

void WeatherSystem::SetPlayerObject(GameObject* player) {
    if (mSkyObject == nullptr)
        return;
    mPlayerTransform = player->GetComponent<Transform>();
    mSkyObject->GetComponent<Transform>()->parent = mPlayerTransform;
}

void WeatherSystem::SetWorldMaterial(Material* materialPtr) {
    mWorldMaterial = materialPtr;
    if (mWorldMaterial != nullptr) {
        mWorldAmbientBase = mWorldMaterial->ambient;
    }
    mCapturedAmbientBase = false;
}

void WeatherSystem::SetStaticMaterial(Material* materialPtr) {
    mStaticMaterial = materialPtr;
    if (mStaticMaterial != nullptr) {
        mStaticAmbientBase = mStaticMaterial->ambient;
    }
    mCapturedAmbientBase = false;
}

void WeatherSystem::SetWaterMaterial(Material* materialPtr) {
    mWaterMaterial = materialPtr;
}

void WeatherSystem::SetTime(float newTime) {
    mWorldTime = newTime;
    UpdateSunAndLighting();
}

float WeatherSystem::GetTime(void) {
    return mWorldTime;
}

void WeatherSystem::SetWorldLightLevel(float low, float high) {
    mWorldLightLow  = low;
    mWorldLightHigh = high;
}

void WeatherSystem::SetWorldLightAmbience(Color color) {
    mAmbientColor = color;
}

void WeatherSystem::SetSkyLightLevel(float low, float high) {
    mSkyLightLow  = low;
    mSkyLightHigh = high;
}

void WeatherSystem::SetFogDensity(float density) {
    mWorldFogDensity = density;
}

void WeatherSystem::SetFogLightBias(float bias) {
    mFogLightBias = bias;
}

void WeatherSystem::SetFogRange(float nearRange, float farRange) {
    mWorldFogNear = nearRange;
    mWorldFogFar  = farRange;
}

void WeatherSystem::SetFogRangeColor(Color nearColor, Color farColor) {
    mWorldFogColorNear = nearColor;
    mWorldFogColorFar  = farColor;
}

void WeatherSystem::SetWeather(WeatherType type) {
    mCurrentWeather = type;
    mNextWeather = type;
    
    if (type == WeatherType::Clear) {
        mRainEmitter->Deactivate();
        mSnowEmitter->Deactivate();
        FogClear();
        return;
    }
    
    if (type == WeatherType::Rain) {
        mRainEmitter->Activate();
        mSnowEmitter->Deactivate();
        
        mWorldFogDensity   = 0.87f;
        mWorldFogNear      = 30.0f;
        mWorldFogFar       = 200.0f;
        mWorldFogColorNear = Colors.gray;
        mWorldFogColorFar  = Colors.gray;
        return;
    }
    
    if (type == WeatherType::Snow) {
        mRainEmitter->Deactivate();
        mSnowEmitter->Activate();
        
        mWorldFogDensity   = 1.1f;
        mWorldFogNear      = 20.0f;
        mWorldFogFar       = 400.0f;
        mWorldFogColorNear = Colors.ltgray;
        mWorldFogColorFar  = Colors.white;
        return;
    }
}

void WeatherSystem::SetWeatherNext(WeatherType type) {
    mNextWeather = type;
    
    if (type == WeatherType::Clear) {
        mWorldFogDensity   = 20.0f;
        mWorldFogNear      = 100.0f;
        mWorldFogFar       = 8000.0f;
        mWorldFogColorNear = Colors.ltgray * 0.7f;
        mWorldFogColorFar  = Colors.Lerp(Colors.blue, Colors.gray, 0.8f);
        return;
    }
    
    if (type == WeatherType::Rain) {
        mWorldFogDensity   = 0.87f;
        mWorldFogNear      = 30.0f;
        mWorldFogFar       = 200.0f;
        mWorldFogColorNear = Colors.gray;
        mWorldFogColorFar  = Colors.gray;
        return;
    }
    
    if (type == WeatherType::Snow) {
        mWorldFogDensity   = 1.1f;
        mWorldFogNear      = 20.0f;
        mWorldFogFar       = 400.0f;
        mWorldFogColorNear = Colors.ltgray;
        mWorldFogColorFar  = Colors.white;
        return;
    }
}

void WeatherSystem::AddWeather(WeatherType type) {

    // Only add in weather of the appropriate type

    if (type == WeatherType::Clear) {
        return;
    }

    if (type == WeatherType::Rain) {
        mRainEmitter->Activate();
        return;
    }

    if (type == WeatherType::Snow) {
        mSnowEmitter->Activate();
        return;
    }
}

WeatherType WeatherSystem::GetWeatherCurrent(void) {
    return mCurrentWeather;
}

WeatherType WeatherSystem::GetWeatherNext(void) {
    return mNextWeather;
}

float WeatherSystem::GetWeatherCycleCounter(void) {
    return mWeatherMasterCounter;
}

void WeatherSystem::SetWeatherCycleCounter(float counter) {
    mWeatherMasterCounter = counter;
}

void WeatherSystem::FogClear(void) {
    // Trigger the fog to shift
    mWeatherFogCounter = 1.0f;
    
    mWorldFogDensity = 20.0f;
    mWorldFogNear    = 100.0f;
    mWorldFogFar     = 8000.0f;
    
    mWorldFogColorNear = Colors.ltgray * 0.7f;
    mWorldFogColorFar  = Colors.Lerp(Colors.blue, Colors.gray, 0.8f);
}

void WeatherSystem::SetTimeScale(float scale) {
    mTimeScale = scale;
}

float WeatherSystem::GetTimeScale(void) {
    return mTimeScale;
}

GameObject* WeatherSystem::CreateSky(const std::string& meshTagName, Color low, Color high, float biasMul) {
    Mesh* skyMesh = Resources.CreateMeshFromTag(meshTagName);
    if (skyMesh == nullptr) return nullptr;
    
    Color skyHigh = low;
    Color skyLow  = low;
    
    // Set the base range
    skyHigh += Colors.MakeGrayScale(0.997f);
    skyLow  += Colors.MakeGrayScale(0.799f);
    
    // Tweak the values
    skyHigh *= Colors.MakeGrayScale(0.961f);
    skyLow  *= Colors.MakeGrayScale(1.01f);
    
    // Uniform scale down
    skyHigh *= Colors.MakeGrayScale(0.24f);
    skyLow  *= Colors.MakeGrayScale(0.24f);
    
    Material* skyMaterial = Renderer.CreateMaterial();
    
    skyMaterial->ambient = Colors.MakeGrayScale(0.0f);
    skyMaterial->diffuse = Colors.MakeGrayScale(0.0f);
    
    skyMaterial->shader  = Resources.shaders.sky;
    
    skyMaterial->DisableDepthTest();
    skyMaterial->DisableShadowVolumePass();
    
    for (unsigned int i=0; i < skyMesh->GetNumberOfVertices(); i++) {
        Vertex vertex = skyMesh->GetVertex(i);
        
        if (vertex.y > 0) {
            vertex.r = Math.Lerp(skyHigh.r, skyLow.r, vertex.y * biasMul);
            vertex.g = Math.Lerp(skyHigh.g, skyLow.g, vertex.y * biasMul);
            vertex.b = Math.Lerp(skyHigh.b, skyLow.b, vertex.y * biasMul);
        } else {
            vertex.r = skyLow.r;
            vertex.g = skyLow.g;
            vertex.b = skyLow.b;
        }
        
        skyMesh->SetVertex(i, vertex);
    }
    skyMesh->Load();
    
    GameObject* skyObject = Engine.Create<GameObject>();
    skyObject->name = "sky";
    
    Component* skyRendererComponent = Engine.CreateComponent<MeshRenderer>();
    skyObject->AddComponent( skyRendererComponent );
    MeshRenderer* skyRenderer = skyObject->GetComponent<MeshRenderer>();
    skyRenderer->mesh = skyMesh;
    skyRenderer->material = skyMaterial;
    
    skyObject->renderDistance = -1;
    
    Transform* transformCache = skyObject->GetComponent<Transform>();
    transformCache->SetScale(10000, 2000, 10000);
    
    return skyObject;
}
