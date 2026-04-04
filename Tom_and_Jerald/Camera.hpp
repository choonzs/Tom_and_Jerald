/*************************************************************************
@file Camera.hpp
@Author Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@Co-authors NIL
@brief
     Declares the Camera class, which tracks a target in the side-scrolling
     world and supports a timed screen-shake effect.

     Follow behaviour:
       Camera X tracks the target position exactly. Camera Y is locked at 0
       because the game is a side-scroller with no vertical scrolling.

     Screen shake:
       Calling Set_Shaking() arms the shake. Each subsequent Update() call
       displaces the camera position by a small random offset for `duration`
       seconds, then resets automatically.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "pch.hpp"

class Camera
{
public:
    Camera()
        : position{ 0, 0 },
        speed{ 0.0f },
        magnitude{ 0.3f },
        timer{ 0.0f },
        duration{ 0.4f },
        camera_shaking{ false } {
    }

    ~Camera() = default;

    // Arms the screen-shake effect. The shake runs for `duration` seconds
    // on the next Update() calls then stops automatically.
    void Set_Shaking();

    // Called every frame. Advances the shake timer and disables the effect
    // once the duration has elapsed.
    void Update();

    // Locks the camera X position to the target, keeping Y fixed at 0.
    void Follow(const AEVec2& target);

    // Accessors
    AEVec2  Position()  const;
    AEVec2& Position();

    f32     Magnitude() const;
    f32& Magnitude();

private:
    AEVec2 position;       // Current camera world-space position.
    f32    speed;          // Reserved for future smooth-follow interpolation.
    f32    magnitude;      // Maximum per-frame shake displacement (pixels).
    f32    timer;          // Elapsed shake time (seconds).
    f32    duration;       // Total shake duration (seconds).
    bool   camera_shaking; // True while a shake effect is in progress.

    // Applies a single frame of random displacement to the camera position.
    // Called internally by Update() while camera_shaking is true.
    void Shake();
};

#endif // CAMERA_HPP