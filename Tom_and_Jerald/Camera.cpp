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
		position.x = randomRange(-magnitude, magnitude);
		position.y = randomRange(-magnitude, magnitude);
	}

	AEVec2 Camera::Position() const { return position; }
	AEVec2& Camera::Position() { return position; }

	f32 Camera::Magnitude() const { return magnitude; }
	f32& Camera::Magnitude() { return magnitude; }




/*
namespace CAMERA {
	bool camera_shaking = false;
	f32 timer = 0.0f;
	
	Camera* Initialize_Camera() {
		Camera* camera = new Camera;

		camera->x = 0.0f;
		camera->y = 0.0f;
		camera->set_x = camera->x;
		camera->set_y = camera->y;

		camera->magnitude = 0.3f;
		return camera;
	}
	void Set_Camera_Shake() {
		camera_shaking = true;
	}

	void Update_Camera(Camera& camera) {
		f32 delta_time = (f32)AEFrameRateControllerGetFrameTime();
		if (camera_shaking) {
			timer += delta_time;
			if (timer >= 0.4f) {
				camera_shaking = false;
				timer = 0.0f;
			}
			else {
				Shake_Camera(camera);
			}
		}
	}

	void Shake_Camera(Camera& camera) {
		camera.x = randomRange(-camera.magnitude, camera.magnitude);
		camera.y = randomRange(-camera.magnitude, camera.magnitude);
		}

	void Free_Camera(Camera* camera) {
		camera_shaking = false;
		delete[] camera;
	}
}
*/