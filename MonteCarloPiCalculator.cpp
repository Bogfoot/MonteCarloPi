﻿#include "MonteCarloPiCalculator.h"
#include <assert.h>
#include <numeric>
#include <random>
#include <chrono>

using namespace std;


void MonteCarloPiCalculator::Calculate(int count)
{
	n = count;
	int m = 100;
	CalculateMonteCarloPi();
	CalculateAverages();
	CalculateStdDevs();
	CalculateRelativeDiff();
	Potencija();
}

void MonteCarloPiCalculator::CalculateMonteCarloPi()
{
	assert((n >= 1) && (n <= MaxIterations));

	for (int k = 0; k < m; k++) {
		for (int j = 0; j < n; j++) {
			int BrTuKrug = 0;
			for (int i = 0; i < pow(10, j); i++)
			{
				double rand_x = GetRandomNumber();
				double rand_y = GetRandomNumber();
				double polozaj = pow(rand_x, 2) + pow(rand_y, 2);
				if (sqrt(polozaj) <= 1)
					BrTuKrug++;
			}
			BrPi[k][j] = ((double)BrTuKrug / pow(10, j) * 4);
		}
	}
}
void MonteCarloPiCalculator::CalculateRelativeDiff()
{
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
			{
				relDiff[i][j] = (BrPi[i][j] - aprox_PI) / (double)aprox_PI;
			}
	}
}


void MonteCarloPiCalculator::CalculateAverages()
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			srVrij[i] += BrPi[j][i];
		}
		srVrij[i] = srVrij[i] / ((double)m );
	}
}

void MonteCarloPiCalculator::CalculateStdDevs()
{
	/*
		sqrt(suma(x_i - x_sr)^2/n)
	*/

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			stDev[i] += pow(BrPi[j][i] - srVrij[i], 2);
		}
		stDev[i] = sqrt(stDev[i] / ((double)m+1));
	}
}

void MonteCarloPiCalculator::Potencija()
{
	for (int i = 0; i < n; i++)
		pot[i] = i;
}

double MonteCarloPiCalculator::GetRandomNumber()
{
	constexpr double maxRandInt = numeric_limits<unsigned int>::max();

	// 'static' ovdje znaci da se ova inicijalizacija izvrsava samo kod prvog poziva funkcije GetRandomNumber();
	static std::random_device rd;
	//static std::mt19937 rng{ rd() };
	//static std::ranlux24 rng{ rd() };
	//static std::default_random_engine rng{ rd() };
	static std::knuth_b rng{ rd() };
	//static std::ranlux48 rng{ rng() };
	
	static uniform_int_distribution<unsigned int> distribution(0, (unsigned int)maxRandInt - 1);

	//static normal_distribution<double> distribution(0.5, 0.25); //ide bez dijeljenja jer je otprilikei između 0 i 1
	//static chi_squared_distribution<double> distribution(1);
	//static student_t_distribution<double> distribution(1.0);
	// Racunanje slucajnog broja kod svakog poziva funkcije GetRandomNumber();
	return distribution(rng) / ((double)maxRandInt-1);
}
