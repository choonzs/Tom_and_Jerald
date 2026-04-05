/*************************************************************************
@file    JetpackFuel.hpp
@Author  Peng Jun Rong Jerald p.rongjunjerald@digipen.edu
@Co-authors  Tan Choon Ming choonming.tan@digipen.edu
@brief
     Declares the JetpackFuel class: fuel capacity, burn rate, passive
     drain, refuelling, and HUD percentage accessor.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#ifndef JETPACK_FUEL_HPP
#define JETPACK_FUEL_HPP

#include "AEEngine.h"

class JetpackFuel {
public:
    JetpackFuel(float maxFuel = 100.0f, float burnRate = 25.0f, float passiveDrain = 2.0f);
    ~JetpackFuel();

    void Update(float dt, bool isThrusting);
    void Draw(float screenX, float screenY, float barWidth, float barHeight);

    void RestoreFuel(float amount);

    bool HasFuel() const { return mCurrentFuel > 0.0f; }
    float GetCurrentFuel() const { return mCurrentFuel; }
    float GetMaxFuel() const { return mMaxFuel; }

private:
    float mMaxFuel;
    float mCurrentFuel;
    float mBurnRate;
    float mPassiveDrain;

    AEGfxVertexList* mpMesh;
};

#endif