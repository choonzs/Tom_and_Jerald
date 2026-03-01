#include "pch.hpp"
#ifndef CAMERA_HPP
#define CAMERA_HPP

class Camera {
public:	
	Camera() : position{ 0, 0 },
		speed{ 0 },
		magnitude{ 0.3f },
		timer{ 0 },
		duration {0.4f},
		camera_shaking{ false }
	{};
	void Set_Shaking();
	void Update();
	void Shake();
	void Follow(const AEVec2& target);
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
		timer,
		duration;

	bool camera_shaking = false;
};
#endif // !CAMERA_HPP
