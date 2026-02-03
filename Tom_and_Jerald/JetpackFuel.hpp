#ifndef JETPACK_FUEL_HPP
#define JETPACK_FUEL_HPP

#include "AEEngine.h"

class JetpackFuel {
public:
    JetpackFuel(float maxFuel = 100.0f, float burnRate = 25.0f, float passiveDrain = 2.0f);
    ~JetpackFuel();

    void Update(float dt, bool isThrusting);
    void Draw(float screenX, float screenY, float barWidth, float barHeight);

    bool HasFuel() const { return mCurrentFuel > 0.0f; }

private:
    float mMaxFuel;
    float mCurrentFuel;
    float mBurnRate;
    float mPassiveDrain; 

    AEGfxVertexList* mpMesh;
};

#endif