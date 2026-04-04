/*************************************************************************
@file Camera.cpp
@Author Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@Co-authors NIL
@brief
     Implements Camera follow behaviour and the timed screen-shake effect.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/

#include "pch.hpp"
#include "Camera.hpp"
#include "Utils.hpp"

// ---------------------------------------------------------------------------
// Camera::Set_Shaking
// ---------------------------------------------------------------------------
// Arms the shake effect. Update() will apply per-frame displacement until
// the timer reaches `duration`, then disarm automatically.
// ---------------------------------------------------------------------------
void Camera::Set_Shaking()
{
    camera_shaking = true;
    timer = 0.0f; // Reset so a fresh trigger always plays the full duration.
}

// ---------------------------------------------------------------------------
// Camera::Update
// ---------------------------------------------------------------------------
// Advances the shake timer each frame. While shaking, delegates to Shake()
// for per-frame displacement. Disarms the effect once duration is reached.
// ---------------------------------------------------------------------------
void Camera::Update()
{
    if (!camera_shaking)
        return;

    const f32 delta_time = static_cast<f32>(AEFrameRateControllerGetFrameTime());
    timer += delta_time;

    if (timer >= duration)
    {
        camera_shaking = false;
        timer = 0.0f;
    }
    else
    {
        Shake();
    }
}

// ---------------------------------------------------------------------------
// Camera::Shake
// ---------------------------------------------------------------------------
// Displaces the camera by a random offset in [-magnitude, +magnitude] on
// both axes, producing a screen-shake effect for one frame.
// ---------------------------------------------------------------------------
void Camera::Shake()
{
    position.x += randomRange(-magnitude, magnitude);
    position.y += randomRange(-magnitude, magnitude);
}

// ---------------------------------------------------------------------------
// Camera::Follow
// ---------------------------------------------------------------------------
// Locks the camera X to the target position. Y stays fixed at 0 because
// this is a side-scroller with no vertical scrolling.
// ---------------------------------------------------------------------------
void Camera::Follow(const AEVec2& target)
{
    position.x = target.x;
    position.y = 0.0f;
}

// ---------------------------------------------------------------------------
// Accessors
// ---------------------------------------------------------------------------

AEVec2  Camera::Position()  const { return position; }
AEVec2& Camera::Position() { return position; }

f32     Camera::Magnitude() const { return magnitude; }
f32& Camera::Magnitude() { return magnitude; }
