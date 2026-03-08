#include "pch.hpp"
#include "Camera.hpp"
#include "Utils.hpp"

// Set Shaking Mode On
void Camera::Set_Shaking() {
	camera_shaking = true;
}

// Updating of Camera
void Camera::Update() {
	f32 delta_time = (f32)AEFrameRateControllerGetFrameTime();
	// Checks if camera is in shaking mode, increment timer until it reaches the set amt of time to shake
	if (camera_shaking) {
		timer += delta_time;
		if (timer >= duration) {
			camera_shaking = false;
			timer = 0.0f;
		}
		else {
			Camera::Shake();
		}
	}
}

// Use randomRange to shake camera randomly
void Camera::Shake() {
	position.x += randomRange(-magnitude, magnitude);
	position.y += randomRange(-magnitude, magnitude);
}

// Set camera to follow a specific target
void Camera::Follow(const AEVec2& target) {
	position.x = target.x;
	position.y = 0.0f; // Since its side scroller, we dont want the height to change
}

// Accessor & Mutators
AEVec2 Camera::Position() const { return position; }
AEVec2& Camera::Position() { return position; }

f32 Camera::Magnitude() const { return magnitude; }
f32& Camera::Magnitude() { return magnitude; }
