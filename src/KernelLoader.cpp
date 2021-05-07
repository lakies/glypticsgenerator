#include "KernelLoader.h"
#include <fstream>
#include <iostream>

void KernelLoader::load(int nSamples) {
	std::ifstream infile("res/marble_kernel.txt");

	std::vector<std::vector<float>> kernel;

	float r, g, b, p;

	while (infile >> r >> g >> b >> p) {
		std::vector<float> v = { r, g, b, p };
		kernel.push_back(v);
	}

	kernel = downsample(kernel, nSamples);

	std::cout << "static const float4 kernel[] = {" << std::endl;
	for (auto sample : kernel) {
		std::cout << "	float4(";
		for (int i = 0; i < 4; i++) {
			std::cout << sample[i];
			if (i < 3)
			{
				std::cout << ',';
			}
		}
		std::cout << ")," << std::endl; 
	}

	std::cout << "};" << std::endl;
}

std::vector<std::vector<float>> KernelLoader::downsample(std::vector<std::vector<float>> kernel, int nSamples) {
	const float EXPONENT = 2.0f; // used for impartance sampling

	float RANGE = kernel[kernel.size() - 1][3]; // get max. sample location

	// calculate offsets
	std::vector<float> offsets;
	calculateOffsets(RANGE, EXPONENT, nSamples, offsets);

	// calculate areas (using importance-sampling) 
	std::vector<float> areas;
	calculateAreas(offsets, areas);

	std::vector<std::vector<float>> resultKernel(nSamples);
	for (int i = 0; i < nSamples; i++)
	{
		resultKernel[i] = std::vector<float>(4);
	}

	std::vector<float> sum = { 0, 0, 0 }; // weights sum for normalization

	// compute interpolated weights
	for (int i = 0; i < nSamples; i++)
	{
		float sx = offsets[i];

		std::vector<float> v = linInterpol1D(kernel, sx);
		resultKernel[i][0] = v[0] * areas[i];
		resultKernel[i][1] = v[1] * areas[i];
		resultKernel[i][2] = v[2] * areas[i];
		resultKernel[i][3] = sx;

		sum[0] += resultKernel[i][0];
		sum[1] += resultKernel[i][1];
		sum[2] += resultKernel[i][2];
	}

	// Normalize
	for (int i = 0; i < nSamples; i++) {
		resultKernel[i][0] /= sum[0];
		resultKernel[i][1] /= sum[1];
		resultKernel[i][2] /= sum[2];
	}

	// TEMP put center at first
	std::vector<float> t = resultKernel[nSamples / 2];
	for (int i = nSamples / 2; i > 0; i--)
		resultKernel[i] = resultKernel[i - 1];
	resultKernel[0] = t;

	return resultKernel;
}

void KernelLoader::calculateOffsets(float _range, float _exponent, int _offsetCount, std::vector<float>& _offsets)
{
	// Calculate the offsets:
	float step = 2.0f * _range / (_offsetCount - 1);
	for (int i = 0; i < _offsetCount; i++) {
		float o = -_range + float(i) * step;
		float sign = o < 0.0f ? -1.0f : 1.0f;
		float ofs = _range * sign * abs(pow(o, _exponent)) / pow(_range, _exponent);
		_offsets.push_back(ofs);
	}
}

void KernelLoader::calculateAreas(std::vector<float>& _offsets, std::vector<float>& _areas)
{
	int size = _offsets.size();

	for (int i = 0; i < size; i++) {
		float w0 = i > 0 ? abs(_offsets[i] - _offsets[i - 1]) : 0.0f;
		float w1 = i < size - 1 ? abs(_offsets[i] - _offsets[i + 1]) : 0.0f;
		float area = (w0 + w1) / 2.0f;
		_areas.push_back(area);
	}
}

std::vector<float> KernelLoader::linInterpol1D(std::vector<std::vector<float>> _kernelData, float _x)
{
	// naive, a lot to improve here

	if (_kernelData.size() < 1) throw "_kernelData empty";

	unsigned int i = 0;
	while (i < _kernelData.size())
	{
		if (_x > _kernelData[i][3]) i++;
		else break;
	}

	std::vector<float> v(3);

	if (i < 1)
	{
		v[0] = _kernelData[0][0];
		v[1] = _kernelData[0][1];
		v[2] = _kernelData[0][2];
	}
	else if (i > _kernelData.size() - 1)
	{
		v[0] = _kernelData[_kernelData.size() - 1][0];
		v[1] = _kernelData[_kernelData.size() - 1][1];
		v[2] = _kernelData[_kernelData.size() - 1][2];
	}
	else
	{
		std::vector<float> b = _kernelData[i];
		std::vector<float> a = _kernelData[i - 1];

		float d = b[3] - a[3];
		float dx = _x - a[3];

		float t = dx / d;

		v[0] = a[0] * (1 - t) + b[0] * t;
		v[1] = a[1] * (1 - t) + b[1] * t;
		v[2] = a[2] * (1 - t) + b[2] * t;
	}

	return v;
}