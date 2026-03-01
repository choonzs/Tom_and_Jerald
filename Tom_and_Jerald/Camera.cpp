#include "pch.hpp"
#include "Camera.hpp"
#include "Utils.hpp"
// TODO Shake Camera
	void Camera::Set_Shaking() {
		camera_shaking = true;
	}

	void Camera::Update() {
		f32 delta_time = (f32)AEFrameRateControllerGetFrameTime();
		if (camera_shaking) {
			timer += delta_time;
			if (timer >= 0.4f) {
				camera_shaking = false;
				timer = 0.0f;
			}
			else {
				Camera::Shake();
			}
		}
	}

	void Camera::Shake() {
		position.x += randomRange(-magnitude, magnitude);
		position.y += randomRange(-magnitude, magnitude);
	}

	void Camera::Follow(const AEVec2& target) {
		position.x = target.x;
		position.y = 0.0f;
	}

	AEVec2 Camera::Position() const { return position; }
	AEVec2& Camera::Position() { return position; }

	f32 Camera::Magnitude() const { return magnitude; }
	f32& Camera::Magnitude() { return magnitude; }
