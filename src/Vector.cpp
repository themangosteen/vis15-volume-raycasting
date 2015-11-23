#include "Vector.h"

#include <math.h>


//-------------------------------------------------------------------------------------------------
// Vector2
//-------------------------------------------------------------------------------------------------

Vector2::Vector2()
{
	m_Elements[0] = 0.0f;
	m_Elements[1] = 0.0f;
}

Vector2::Vector2(const float x, const float y)
{
	m_Elements[0] = x;
	m_Elements[1] = y;
}

Vector2::Vector2(const float vfValues[])
{
	m_Elements[0] = vfValues[0];
	m_Elements[1] = vfValues[1];
}

Vector2::~Vector2()
{
}

void Vector2::values(const float x, const float y)
{
	m_Elements[0] = x;
	m_Elements[1] = y;
}

const float* Vector2::values() const
{
	return m_Elements;
}

const float Vector2::value(const unsigned int uIndex) const
{
	return m_Elements[uIndex];
}

void Vector2::x(const float fX)
{
	m_Elements[0] = fX;
}

void Vector2::y(const float fY)
{
	m_Elements[1] = fY;
}

const float Vector2::x() const
{
	return m_Elements[0];
}

const float Vector2::y() const
{
	return m_Elements[1];
}

const float Vector2::magnitude() const
{
	return sqrtf(x()*x() + y()*y());
}

void Vector2::normalise()
{
	float magnitude = (*this).magnitude();
	if (magnitude > 0.0f)
		(*this) /= magnitude;
}

void Vector2::invert()
{
	x(1.0f / x());
	y(1.0f / y());
}

const float Vector2::dot(const Vector2 &other) const
{
	return x() * other.x() + y()*other.y();
}

const bool Vector2::operator==(const Vector2 &other) const
{
	return x() == other.x() && y() == other.y();
}

const bool Vector2::operator!=(const Vector2 &other) const
{
	return !(*this == other);
}

const Vector2 Vector2::operator-() const
{
	return Vector2(-x(), -y());
}

const Vector2 Vector2::operator+(const Vector2 &other) const
{
	Vector2 vecNew = *this;
	vecNew += other;
	return vecNew;
}

const Vector2 Vector2::operator-(const Vector2 &other) const
{
	Vector2 vecNew = *this;
	vecNew -= other;
	return vecNew;
}

const Vector2 Vector2::operator*(const Vector2 &other) const
{
	Vector2 vecNew = *this;
	vecNew *= other;
	return vecNew;
}

const Vector2 Vector2::operator/(const Vector2 &other) const
{
	Vector2 vecNew = *this;
	vecNew /= other;
	return vecNew;
}

const Vector2& Vector2::operator+=(const Vector2 &other)
{
	x(x() + other.x());
	y(y() + other.y());
	return *this;
};

const Vector2& Vector2::operator-=(const Vector2 &other)
{
	x(x() - other.x());
	y(y() - other.y());
	return *this;
};

const Vector2& Vector2::operator*=(const Vector2 &other)
{
	x(x() * other.x());
	y(y() * other.y());
	return *this;
};

const Vector2& Vector2::operator/=(const Vector2 &other)
{
	x(x() / other.x());
	y(y() / other.y());
	return *this;
};

const Vector2 Vector2::operator*(const float &other) const
{
	Vector2 vecNew = *this;
	vecNew *= other;
	return vecNew;
};

const Vector2 Vector2::operator/(const float &other) const
{
	Vector2 vecNew = *this;
	vecNew /= other;
	return vecNew;
};

const Vector2& Vector2::operator*=(const float &other)
{
	x(x() * other);
	y(y() * other);
	return *this;
};

const Vector2& Vector2::operator/=(const float &other)
{
	x(x() / other);
	y(y() / other);
	return *this;
};

float& Vector2::operator[](const unsigned int i)
{
	return m_Elements[i];
}

const float Vector2::operator[](const unsigned int i) const
{
	return m_Elements[i];
}


//-------------------------------------------------------------------------------------------------
// Vector3
//-------------------------------------------------------------------------------------------------

Vector3::Vector3()
{
	m_Elements[0] = 0.0f;
	m_Elements[1] = 0.0f;
	m_Elements[2] = 0.0f;
}

Vector3::Vector3(const float x, const float y, const float z)
{
	m_Elements[0] = x;
	m_Elements[1] = y;
	m_Elements[2] = z;
}

Vector3::Vector3(const float vfValues[])
{
	m_Elements[0] = vfValues[0];
	m_Elements[1] = vfValues[1];
	m_Elements[2] = vfValues[2];
}

Vector3::~Vector3()
{
}

void Vector3::values(const float x, const float y, const float z)
{
	m_Elements[0] = x;
	m_Elements[1] = y;
	m_Elements[2] = z;
}

const float* Vector3::values() const
{
	return m_Elements;
}

const float Vector3::value(const unsigned int uIndex) const
{
	return m_Elements[uIndex];
}

void Vector3::x(const float x)
{
	m_Elements[0] = x;
}

void Vector3::y(const float y)
{
	m_Elements[1] = y;
}

void Vector3::z(const float z)
{
	m_Elements[2] = z;
}

const float Vector3::x() const
{
	return m_Elements[0];
}

const float Vector3::y() const
{
	return m_Elements[1];
}

const float Vector3::z() const
{
	return m_Elements[2];
}

const float Vector3::magnitude() const
{
	return sqrtf(x()*x() + y()*y() + z()*z());
}

void Vector3::normalise()
{
	float magnitude = (*this).magnitude();
	if (magnitude > 0.0f)
		(*this) /= magnitude;
}

void Vector3::invert()
{
	x(1.0f / x());
	y(1.0f / y());
	z(1.0f / z());
}

const float Vector3::dot(const Vector3 &other) const
{
	return x() * other.x() + y()*other.y() + z()*other.z();
}

const Vector3 Vector3::cross(const Vector3 &other) const
{
	return Vector3(y() * other.z() - z() * other.y(), z() * other.x() - x() * other.z(), x() * other.y() - y() * other.x());
}


//-------------------------------------------------------------------------------------------------
// Operators
//-------------------------------------------------------------------------------------------------

const bool Vector3::operator==(const Vector3 &other) const
{
	return x() == other.x() && y() == other.y() && z() == other.z();
}

const bool Vector3::operator!=(const Vector3 &other) const
{
	return !(*this == other);
}

const Vector3 Vector3::operator-() const
{
	return Vector3(-x(), -y(), -z());
}

const Vector3 Vector3::operator+(const Vector3 &other) const
{
	Vector3 vecNew = *this;
	vecNew += other;
	return vecNew;
}

const Vector3 Vector3::operator-(const Vector3 &other) const
{
	Vector3 vecNew = *this;
	vecNew -= other;
	return vecNew;
}

const Vector3 Vector3::operator*(const Vector3 &other) const
{
	Vector3 vecNew = *this;
	vecNew *= other;
	return vecNew;
}

const Vector3 Vector3::operator/(const Vector3 &other) const
{
	Vector3 vecNew = *this;
	vecNew /= other;
	return vecNew;
}

const Vector3& Vector3::operator+=(const Vector3 &other)
{
	x(x() + other.x());
	y(y() + other.y());
	z(z() + other.z());
	return *this;
};

const Vector3& Vector3::operator-=(const Vector3 &other)
{
	x(x() - other.x());
	y(y() - other.y());
	z(z() - other.z());
	return *this;
};

const Vector3& Vector3::operator*=(const Vector3 &other)
{
	x(x() * other.x());
	y(y() * other.y());
	z(z() * other.z());
	return *this;
};

const Vector3& Vector3::operator/=(const Vector3 &other)
{
	x(x() / other.x());
	y(y() / other.y());
	z(z() / other.z());
	return *this;
};

const Vector3 Vector3::operator*(const float &other) const
{
	Vector3 vecNew = *this;
	vecNew *= other;
	return vecNew;
};

const Vector3 Vector3::operator/(const float &other) const
{
	Vector3 vecNew = *this;
	vecNew /= other;
	return vecNew;
};

const Vector3& Vector3::operator*=(const float &other)
{
	x(x() * other);
	y(y() * other);
	z(z() * other);
	return *this;
};

const Vector3& Vector3::operator/=(const float &other)
{
	x(x() / other);
	y(y() / other);
	z(z() / other);
	return *this;
};

float& Vector3::operator[](const unsigned int i)
{
	return m_Elements[i];
}

const float Vector3::operator[](const unsigned int i) const
{
	return m_Elements[i];
}

