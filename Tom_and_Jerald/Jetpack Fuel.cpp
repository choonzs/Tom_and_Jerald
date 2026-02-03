#include "JetpackFuel.hpp"
#include "pch.hpp"
JetpackFuel::JetpackFuel(float maxFuel, float burnRate, float passiveDrain)
    : mMaxFuel(maxFuel), mCurrentFuel(maxFuel),
    mBurnRate(burnRate), mPassiveDrain(passiveDrain), mpMesh(nullptr)
{
    AEGfxMeshStart();
    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
        0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
    AEGfxTriAdd(0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
        0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
    mpMesh = AEGfxMeshEnd();
}

JetpackFuel::~JetpackFuel() {
    if (mpMesh) AEGfxMeshFree(mpMesh);
}

void JetpackFuel::Update(float dt, bool isThrusting) {
    mCurrentFuel -= mPassiveDrain * dt;

    if (isThrusting) {
        mCurrentFuel -= mBurnRate * dt;
    }

    if (mCurrentFuel < 0.0f) mCurrentFuel = 0.0f;
    if (mCurrentFuel > mMaxFuel) mCurrentFuel = mMaxFuel;
}

void JetpackFuel::Draw(float screenX, float screenY, float barWidth, float barHeight) {
    if (!mpMesh) return;

    float ratio = mCurrentFuel / mMaxFuel;
    std::cout << ratio << "\n";
    AEMtx33 transform, mTrans, mScale;

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxTextureSet(NULL, 0.0f, 0.0f);

    AEMtx33Trans(&mTrans, screenX, screenY);
    AEMtx33Scale(&mScale, barWidth + 2.0f, barHeight + 2.0f);
    AEMtx33Concat(&transform, &mTrans, &mScale);
    AEGfxSetTransform(transform.m);

	AEGfxSetColorToMultiply(0, 0, 0, 1);
    AEGfxSetColorToAdd(0, 0, 1, 1);
    AEGfxMeshDraw(mpMesh, AE_GFX_MDM_TRIANGLES);

    float currentWidth = barWidth * ratio;
    std::cout << currentWidth << "\n";
    float xOffset = (barWidth - currentWidth) / 2.0f;
    AEMtx33Trans(&mTrans, screenX - xOffset, screenY);
    AEMtx33Scale(&mScale, currentWidth , barHeight);
    AEMtx33Concat(&transform, &mTrans, &mScale);
    AEGfxSetTransform(transform.m); 

    AEGfxSetColorToAdd(0, 1, 0, 1);
    AEGfxMeshDraw(mpMesh, AE_GFX_MDM_TRIANGLES);
}