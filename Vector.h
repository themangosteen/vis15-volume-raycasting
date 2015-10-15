#pragma once


// 2D-VECTOR
class Vector2
{

	public:

		Vector2();
		Vector2(const float x, const float y);
		Vector2(const float values[]);

		~Vector2();


		// VECTOR VALUES

		void values(const float x, const float y);
		const float* values() const;
		const float value(const unsigned int i) const;

		void x(const float x);
		void y(const float y);

		const float x() const;
		const float y() const;


		// VECTOR MATH

		const float magnitude() const;

		void normalise();
		void invert();

		const float dot(const Vector2 &other) const;


		// OPERATORS

		const bool operator==(const Vector2 &other) const;
		const bool operator!=(const Vector2 &other) const;

		const Vector2 operator-() const;

		const Vector2 operator+(const Vector2 &other) const;
		const Vector2 operator-(const Vector2 &other) const;
		const Vector2 operator*(const Vector2 &other) const;
		const Vector2 operator/(const Vector2 &other) const;

		const Vector2& operator+=(const Vector2 &other);
		const Vector2& operator-=(const Vector2 &other);
		const Vector2& operator*=(const Vector2 &other);
		const Vector2& operator/=(const Vector2 &other);

		const Vector2 operator*(const float &other) const;
		const Vector2 operator/(const float &other) const;

		const Vector2& operator*=(const float &other);
		const Vector2& operator/=(const float &other);

		float& operator[](const unsigned int i);
		const float operator[](const unsigned int i) const;


	private:

		float m_Elements[2];

};


// 3D-VECTOR
class Vector3
{
	public:

		Vector3();
		Vector3(const float x, const float y, const float z);
		Vector3(const float values[]);

		~Vector3();


		// VECTOR VALUES

		void values(const float x, const float y, const float z);
		const float* values() const;
		const float value(const unsigned int i) const;

		void x(const float x);
		void y(const float y);
		void z(const float z);

		const float x() const;
		const float y() const;
		const float z() const;


		// VECTOR MATH

		const float magnitude() const;

		void normalise();
		void invert();

		const float dot(const Vector3 &other) const;
		const Vector3 cross(const Vector3 &other) const;


		// OPERATORS
		
		const bool operator==(const Vector3 &other) const;
		const bool operator!=(const Vector3 &other) const;

		const Vector3 operator-() const;

		const Vector3 operator+(const Vector3 &other) const;
		const Vector3 operator-(const Vector3 &other) const;
		const Vector3 operator*(const Vector3 &other) const;
		const Vector3 operator/(const Vector3 &other) const;
		
		const Vector3& operator+=(const Vector3 &other);
		const Vector3& operator-=(const Vector3 &other);
		const Vector3& operator*=(const Vector3 &other);
		const Vector3& operator/=(const Vector3 &other);
		
		const Vector3 operator*(const float &other) const;
		const Vector3 operator/(const float &other) const;

		const Vector3& operator*=(const float &other);
		const Vector3& operator/=(const float &other);
		
		float& operator[](const unsigned int i);
		const float operator[](const unsigned int i) const;
		

	private:

		float m_Elements[3];
};
