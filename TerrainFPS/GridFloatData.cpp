#include "GridFloatData.h"

//-------------------------------------------------------------------------
//  Checks if the file exists.
//-------------------------------------------------------------------------
bool GridFloatData::fileExists(const std::string& filename) {
	std::ifstream ifile(filename.c_str());
	return (bool)ifile;
}

//	Reverses the bytes of a float.
float GridFloatData::ReverseFloat(const float inFloat)
{
	float retVal;
	char *floatToConvert = (char*)& inFloat;
	char *returnFloat = (char*)& retVal;

	// swap the bytes into a temporary buffer
	returnFloat[0] = floatToConvert[3];
	returnFloat[1] = floatToConvert[2];
	returnFloat[2] = floatToConvert[1];
	returnFloat[3] = floatToConvert[0];

	return retVal;
}

//	Reads in the file responsable for determining how the gridFloat flt file is formatted.
int GridFloatData::GetGridFloatFromFile() {
	std::cout << "Begin reading in files\n";
	if (fileExists(fileName + ".hdr")) {
		std::ifstream file;
		std::string line;

		file.open(fileName + ".hdr");

		//	Parses through the hdr file and gets the relevant data out of it.
		while (std::getline(file, line)) {
			std::stringstream stream(line);
			std::string lineDefinition;
			stream >> lineDefinition;

			if (lineDefinition == "ncols") {
				std::cout << "ncols = ";
				stream >> ncols;
				std::cout << ncols << "\n";
			}
			else if (lineDefinition == "nrows") {
				std::cout << "nrows = ";
				stream >> nrows;
				std::cout << nrows << "\n";
			}
			else if (lineDefinition == "xllcorner") {
				std::cout << "xllcorner = ";
				stream >> xllcorner;
				std::cout << xllcorner << "\n";
			}
			else if (lineDefinition == "yllcorner") {
				std::cout << "yllcorner = ";
				stream >> yllcorner;
				std::cout << yllcorner << "\n";
			}
			else if (lineDefinition == "cellsize") {
				std::cout << "cellsize = ";
				stream >> cellsize;
				std::cout << cellsize << "\n";
			}
			else if (lineDefinition == "NODATA_value") {
				std::cout << "nodataValue =";
				stream >> NODATA;
				std::cout << NODATA << "\n";
			}
			else if (lineDefinition == "byteorder") {
				std::cout << "byteorder =";
				std::string order;
				stream >> order;
				std::cout << order << '\n';

				if (order == "LSBFIRST")
					order = LSBFIRST;
				else if (order == "MSBFIRST")
					order = MSBFIRST;
				else if (order == "VMS_FFLOAT")
					order = VMS_FFLOAT;
				else {
					std::cout << "error reading byteorder\n";
					return 1;
				}
			}
		}

	}
	else {
		std::cerr << "Unable to read .hdr file\n";
	}
	/*
	//	Reads in the prj file, which has info I don't care about right now. So it awaits implimentation
	std::cout << "\n";
	if (fileExists(fileName + ".prj")) {
	std::ifstream file;
	std::string line;

	file.open(fileName + ".prj");

	while (std::getline(file, line))
	{
	//cout << line << '\n';
	}

	}
	else {
	std::cerr << "Unable to read .prj file\n";
	}
	*/

	//	Reads the actual gridFloat data in, as determined by the gridFLoatData struct.
	std::cout << "\n";
	if (fileExists(fileName + ".flt")) {
		std::ifstream file;

		file.open(fileName + ".flt", std::ios::binary | std::ios::in);

		data.resize(nrows, std::vector<float>(ncols, (float)NODATA));

		highestValue = (float)NODATA;
		lowestValue = 99999999.0;
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < ncols; j++) {
				float numReadIn;

				//if (order == LSBFIRST) 
				file.read((char *)&numReadIn, sizeof(numReadIn));

				data[i][j] = numReadIn;
				if (numReadIn > highestValue) {
					highestValue = numReadIn;
					posHighestPoint[0] = i;
					posHighestPoint[1] = j;
				}
				if (numReadIn < lowestValue)
					lowestValue = numReadIn;
			}
			//std::cout << data[i][0] << " i :" << i << "   j :" << 0 << " \n";
		}

	}
	else {
		std::cerr << "Unable to read .flt file" << std::endl;
	}
	range = highestValue - lowestValue;
	std::cout << "Finished reading in files" << std::endl;
	return 0;
}

int GridFloatData::SelectNewFile(std::string name) {
	fileName = name;
	GetGridFloatFromFile();

	return 0;
}

GridFloatData::GridFloatData(std::string name)
{
	fileName = name;
	GetGridFloatFromFile();
}


GridFloatData::~GridFloatData()
{
}
