#include "MultiSet.h"

#include <sstream>
#include <fstream>


//-------------------------------------------------------------------------------------------------
// MultiSet
//-------------------------------------------------------------------------------------------------

MultiSet::MultiSet()
{
}

MultiSet::~MultiSet()
{
}

const MultiSet::Variate& MultiSet::variate(const int i) const
{
	return m_Variates[i];
}

const MultiSet::Variate* MultiSet::variates() const
{
	return &(m_Variates.front());
}

const int MultiSet::dimensions() const
{
	return m_Dimensions;
}

const MultiSet::DataElement& MultiSet::element(const int i) const
{
	return m_DataElements[i];
}

const MultiSet::DataElement* MultiSet::elements() const
{
	return &(m_DataElements.front());
}

const float MultiSet::value(const int i, const int v) const
{
	return m_DataElements[i].values[v];
}

const int MultiSet::size() const
{
	return m_Size;
}


//-------------------------------------------------------------------------------------------------
// MultiSet File Loader
//-------------------------------------------------------------------------------------------------

bool MultiSet::loadFromFile(QString filename, QProgressBar* progressBar)
{
	std::string filenameStr = filename.toStdString();
	std::ifstream csvFile(filenameStr);
	if (!csvFile.is_open())
	{
		std::cerr << "+ Error loading file: " << filenameStr << std::endl;
		return false;
	}

	std::ifstream cntLines(filenameStr);
	int numLines = std::count(std::istreambuf_iterator<char>(cntLines), std::istreambuf_iterator<char>(), '\n') + 1;
	cntLines.close();
	progressBar->setRange(0, numLines);


	// read file line by line

	std::string line;
	int l = 1;

	while (std::getline(csvFile, line))
	{
		progressBar->setValue(l);

		std::stringstream lineStream(line);
		std::string cell;

		if (l == 1)   // read header
		{
			bool first = true;

			while (std::getline(lineStream, cell, '\t'))
			{
				if (first)
				{
					// skip first column because it only says "ID"
					first = false;
				}
				else
				{
					Variate variate;
					variate.name = cell;
					variate.min = std::numeric_limits<float>::max();
					variate.max = std::numeric_limits<float>::min();
					m_Variates.push_back(variate);
				}
			}
		}
		else   // read data
		{
			DataElement element;
			bool first = true;

			while (std::getline(lineStream, cell, '\t'))
			{
				if (first)
				{
					// first column depicts data element name
					element.name = cell;
					first = false;
				}
				else
				{
					float value = std::atof(cell.c_str());
					element.values.push_back(value);
				}
			}

			m_DataElements.push_back(element);
		}

		l++;
	}

	csvFile.close();

	m_Dimensions = m_Variates.size();
	m_Variates.resize(m_Dimensions);
	m_Size = m_DataElements.size();
	m_DataElements.resize(m_Size);

	progressBar->setValue(0);

	std::cout << "Loaded MULTIVARIATE with " << m_Dimensions << " dimensions and " << m_Size << " elements " << std::endl;


	// set min/max values
	
	for (int v = 0; v < m_Dimensions; v++)
	{
		Variate curVar = m_Variates[v];

		for (int e = 0; e < m_Size; e++)
		{
			float value = m_DataElements[e].values[v];
			if (value < curVar.min) curVar.min = value;
			if (value > curVar.max) curVar.max = value;
		}

		m_Variates[v] = curVar;
	}

	return true;
}
