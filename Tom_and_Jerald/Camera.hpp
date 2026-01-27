#include "pch.h"
#ifndef CAMERA_HPP
#define CAMERA_HPP
struct Camera {
	f32 x, y,
		set_x, set_y,
		speed,
		magnitude;
};

namespace CAMERA {
	Camera* Initialize_Camera();
	void Set_Camera_Shake();
	void Update_Camera(Camera& camera);
	void Shake_Camera(Camera& camera);
	void Free_Camera(Camera* camera);
}
#endif // !CAMERA_HPP
