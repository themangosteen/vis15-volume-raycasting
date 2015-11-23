#pragma once

#include "Vector.h"

#include <vector>
#include <string>
#include <iostream>

#include <QProgressBar>


class VectorField
{

public:

	VectorField();
	~VectorField();


	// VECTOR-FIELD DATA

	const Vector2&					vector(const int i) const;
	const Vector2&					vector(const int x, const int y) const;
	const Vector2*					vectors() const;

	typedef std::vector<float> Parameter;

	const Parameter&				parameter(const int i) const;
	const Parameter&				parameter(const int x, const int y) const;
	const Parameter*				parameters() const;

	const int						width() const;
	const int						height() const;
	const int						size() const;


	// LOAD FROM FILE

	bool							loadFromFile(QString filename, QProgressBar* progressBar);


private:

	int								m_Width;
	int								m_Height;
	int								m_Size;

	int								m_NumParameters;

	Vector2*						m_Vectors;
	Parameter*						m_Parameters;

};
