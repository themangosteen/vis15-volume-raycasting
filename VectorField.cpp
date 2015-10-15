#include "VectorField.h"

#include <sstream>
#include <fstream>


//-------------------------------------------------------------------------------------------------
// VectorField
//-------------------------------------------------------------------------------------------------

VectorField::VectorField()
{
}

VectorField::~VectorField()
{
}

const Vector2& VectorField::vector(const int i) const
{
	return m_Vectors[i];
}

const Vector2& VectorField::vector(const int x, const int y) const
{
	return m_Vectors[x + y*m_Width];
}

const VectorField::Parameter& VectorField::parameter(const int i) const
{
	return m_Parameters[i];
}

const VectorField::Parameter& VectorField::parameter(const int x, const int y) const
{
	return m_Parameters[x + y*m_Width];
}

const VectorField::Parameter* VectorField::parameters() const
{
	return m_Parameters;
}

const int VectorField::width() const
{
	return m_Width;
}

const int VectorField::height() const
{
	return m_Height;
}

const int VectorField::size() const
{
	return m_Size;
}


//-------------------------------------------------------------------------------------------------
// VectorField File Loader
//-------------------------------------------------------------------------------------------------

bool VectorField::loadFromFile(QString filename, QProgressBar* progressBar)
{
	std::string filenameStr = filename.toStdString();
	std::ifstream griFile(filenameStr);
	if (!griFile.is_open())
	{
		std::cerr << "+ Error loading file: " << filenameStr << std::endl;
		return false;
	}

	// progress bar
	progressBar->setRange(0, 100);
	progressBar->setValue(0);


	// --- READ GEOMETRY --- //

	unsigned short uWidth, uHeight, uNum;
	std::string datFile;

	std::string line;
	int index = 1;
	while (std::getline(griFile, line))
	{
		std::istringstream iss(line);
		switch (index)
		{
		case 1:
			iss >> uWidth;
			break;
		case 2:
			iss >> uHeight;
			break;
		case 3:
			iss >> uNum;
			break;
		case 4:
			std::string path = filenameStr.substr(0, filenameStr.find_last_of("/") + 1);
			datFile = path + line;
			break;
		}
		index++;
	}
	griFile.close();

	// set dataset dimensions
	m_Width = int(uWidth);
	m_Height = int(uHeight);
	m_NumParameters = int(uNum);
	m_Size = m_Width * m_Height;

	// initialise arrays
	m_Vectors = new Vector2[m_Size];
	m_Parameters = new Parameter[m_Size];

	// progress bar
	progressBar->setValue(20);


	// --- READ DATA --- //

	FILE *fp = NULL;
	fopen_s(&fp, datFile.c_str(), "rb");
	if (!fp)
	{
		std::cerr << "+ Error loading file: " << datFile << std::endl;
		return false;
	}

	// read into vector before writing data into volume to speed up process
	int dataSize = (m_Size * 3) + (m_Size * m_NumParameters);   // at every grid position, a 3D-vector and m_NumParameters parameters are stored
	float* tmpArray = new float[dataSize];
	fread(tmpArray, sizeof(float), dataSize, fp);
	fclose(fp);

	// progress bar
	progressBar->setValue(40);


	// store vector data

	int vecIdx = 0;
	for (int i = 0; i < m_Size; i++)
	{
		// vecIdx							->  x
		// vecIdx+1							->  y
		// vecIdx+2							->  z (ignored here, because only 2D data)
		// vecIdx+3 - vecIdx+3+(uNum-1)		->  additional parameter

		float vecX = tmpArray[vecIdx];
		vecIdx += 1;
		float vecY = tmpArray[vecIdx];
		vecIdx += 2;
		
		Parameter parameter;
		parameter.resize(m_NumParameters);
		for (int p = 0; p < m_NumParameters; p++)
		{
			parameter[p] = tmpArray[vecIdx];
			vecIdx++;
		}

		// store data in vector field
		m_Vectors[i] = Vector2(vecX, vecY);
		m_Parameters[i] = parameter;

		// progress bar
		float progress = ((float)i / float(m_Size)) * 60.0f;
		progressBar->setValue(40 + progress);
	}

	// progress bar
	progressBar->setValue(0);

	// delete temporary array
	delete tmpArray;

	std::cout << "Loaded VECTORFIELD with dimensions " << m_Width << " x " << m_Height << std::endl;

	return true;
}
