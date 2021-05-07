#pragma once

#include <vector>;

class KernelLoader {
public:
	void load(int nSamples);

private:
	std::vector<std::vector<float>> downsample(std::vector<std::vector<float>> kernel, int nSamples);
	void KernelLoader::calculateOffsets(float _range, float _exponent, int _offsetCount, std::vector<float>& _offsets);
	void KernelLoader::calculateAreas(std::vector<float>& _offsets, std::vector<float>& _areas);
	std::vector<float> KernelLoader::linInterpol1D(std::vector<std::vector<float>> _kernelData, float _x);
};