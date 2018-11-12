#pragma once
#include <array>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#ifndef GRID_FLOAT_DATA_H
#define GRID_FLOAT_DATA_H

//	Defines the byte order of the gridFloat file
enum byteOrder {
	MSBFIRST = 0,
	LSBFIRST = 1,
	VMS_FFLOAT = 2
};

class GridFloatData
{
private:
	std::string fileName;

public:
	int ncols;
	int nrows;
	float xllcorner;
	float yllcorner;
	float cellsize;
	int NODATA = -9999;
	byteOrder order;
	int highestValue;
	int lowestValue;
	float range;
	std::array<float, 2> posHighestPoint;

	std::vector<std::vector<float>> data;

	bool static fileExists(const std::string& filename);
	float static ReverseFloat(const float);
	int GetGridFloatFromFile();
	int SelectNewFile(std::string);

	GridFloatData(std::string);
	~GridFloatData();
};

#endif

