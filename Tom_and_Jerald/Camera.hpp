#include "pch.hpp"
#ifndef CAMERA_HPP
#define CAMERA_HPP

/*
TODO CHANGE TO CLASS
*/
//struct Camera {
//	f32 x, y,
//		set_x, set_y,
//		speed,
//		magnitude;
//};
//
//namespace CAMERA {
//	Camera* Initialize_Camera();
//	void Set_Camera_Shake();
//	void Update_Camera(Camera& camera);
//	void Shake_Camera(Camera& camera);
//	void Free_Camera(Camera* camera);
//}
	class Camera {
	public:	
		Camera() : position{ 0, 0 },
			speed{ 0 },
			magnitude{ 0.3f },
			timer{ 0 },
			camera_shaking{ false }
		{};
		void Set_Shaking();
		void Update();
		void Shake();
		~Camera() = default;

		//Accessors & Mutator
		AEVec2 Position() const;
		AEVec2& Position();

		f32 Magnitude() const;
		f32& Magnitude();

	private:
		AEVec2 position;
		f32	speed,
			magnitude,
			timer;

		bool camera_shaking = false;
	};
#endif // !CAMERA_HPP
