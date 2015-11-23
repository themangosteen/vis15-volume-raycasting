#pragma once

#include <vector>
#include <string>
#include <iostream>

#include <QProgressBar>


//-------------------------------------------------------------------------------------------------
// Multivariate Dataset
//-------------------------------------------------------------------------------------------------

class MultiSet
{

public:

	MultiSet();
	~MultiSet();


	// DATA DIMENSIONS

	struct Variate
	{
		std::string		name;
		float			min;
		float			max;
	};

	const Variate&						variate(const int i) const;
	const Variate*						variates() const;

	const int							dimensions() const;


	// DATA VALUES

	struct DataElement
	{
		std::string						name;
		std::vector<float>				values;
	};

	const DataElement&					element(const int i) const;
	const DataElement*					elements() const;

	const float							value(const int i, const int v) const;

	const int							size() const;


	// FILE LOADER

	bool								loadFromFile(QString filename, QProgressBar* progressBar);


private:

	std::vector<DataElement>			m_DataElements;
	std::vector<Variate>				m_Variates;

	int									m_Dimensions;
	int									m_Size;

};
