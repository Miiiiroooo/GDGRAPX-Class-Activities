#pragma once

struct Vec3f
{
	float x;
	float y;
	float z;

	Vec3f()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	Vec3f(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

class Model3D
{
public:
	Model3D();
	void Draw();

private:
	Vec3f position;
	Vec3f rotation;
	Vec3f scale;
};

