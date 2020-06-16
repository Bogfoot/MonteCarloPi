#pragma once

#include <array>
#include <vector>


constexpr int MaxIterations = 100;
constexpr double aprox_PI = 3.141592653589793238463;

using PiArray = std::array<double, MaxIterations>;
using PiMatrix = std::array<PiArray, MaxIterations>;
using PiVector = std::vector<std::vector<double>> ;

class MonteCarloPiCalculator
{
public:

	int n = 0;
	int m = 100;

	PiMatrix BrPi;
	PiVector relDiff;
	PiVector VectPot;

	PiArray srVrij;
	PiArray stDev;
	PiArray pot;
	

	void Calculate(int count);

private:
	void CalculateMonteCarloPi();
	void CalculateRelativeDiff();
	void CalculateAverages();
	void CalculateStdDevs();
	double GetRandomNumber();
	void Potencija();
	void VectPotencija();
};
