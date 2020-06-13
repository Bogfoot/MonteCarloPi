#pragma once

#include <array>


constexpr int MaxIterations = 101;

using PiArray = std::array<double, MaxIterations>;
using PiMatrix = std::array<PiArray, MaxIterations>;


class MonteCarloPiCalculator
{
public:

	int n = 0;
	PiMatrix BrPi;
	PiArray srVrij;
	PiArray stDev;
	PiArray pot;

	void Calculate(int count);

private:
	void CalculateMonteCarloPi();
	void CalculateAverages();
	void CalculateStdDevs();
	double GetRandomNumber();
	void Potencija();
};
