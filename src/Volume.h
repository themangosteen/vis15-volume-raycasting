#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cstdio>

#include <QProgressBar>


//-------------------------------------------------------------------------------------------------
// Voxel
//-------------------------------------------------------------------------------------------------

class Voxel
{
public:

	Voxel();
	Voxel(const Voxel &other);
	Voxel(const float value);

	~Voxel();


	// VOXEL VALUE

	void					setValue(const float value);
	const float				getValue() const;


	// OPERATORS

	const bool				operator==(const Voxel &other) const;
	const bool				operator!=(const Voxel &other) const;
	const bool				operator>(const Voxel &other) const;
	const bool				operator>=(const Voxel &other) const;
	const bool				operator<(const Voxel &other) const;
	const bool				operator<=(const Voxel &other) const;

	const Voxel				operator+(const Voxel &other) const;
	const Voxel				operator-(const Voxel &other) const;
	const Voxel				operator*(const float &value) const;
	const Voxel				operator/(const float &value) const;

	const Voxel&			operator+=(const Voxel &other);
	const Voxel&			operator-=(const Voxel &other);
	const Voxel&			operator*=(const float &value);
	const Voxel&			operator/=(const float &value);


private:

	float					m_Value;

};


//-------------------------------------------------------------------------------------------------
// Volume
//-------------------------------------------------------------------------------------------------

class Volume
{

public:

	Volume();
	~Volume();


	// VOLUME DATA

	const Voxel&			voxel(const int i) const;
	const Voxel&			voxel(const int x, const int y, const int z) const;
	const Voxel*			voxels() const;

	const int				width() const;
	const int				height() const;
	const int				depth() const;

	const int				size() const;

	bool					loadFromFile(QString filename, QProgressBar* progressBar);


private:

	std::vector<Voxel>		m_Voxels;

	int						m_Width;
	int						m_Height;
	int						m_Depth;

	int						m_Size;

};
