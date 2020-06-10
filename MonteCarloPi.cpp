#include <assert.h>
#include <iostream>
#include <random>
#include <chrono>
#include <math.h>
#include <time.h>
#include <iomanip>

#include "MonteCarloPi.h"
#include <TApplication.h>
#include <TF1.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TPaveLabel.h>
#include <TPaveText.h>
#include <TArrow.h>
#include <TSystem.h>

using namespace std;


/*
	Treba napraviti MCSim za generiranje broja \pi.
	U kvadratu površine 1 (1x1) generirati će se parovi brojeva (x,y)&[0-1].
	Upisati kružnicu radiusa 1, površine 1 * 1 * \pi= \pi. Prebrojati koliko se točaka nalazi u kružnici.
	BrTuKrug/BrTuKvad=\pi/4
*/
int main(int argc, char* argv[], char* /*envp[]*/)
{
	TApplication app("ROOT Application", &argc, argv);
	TCanvas canvas("c1");

	char repeat = 'y';
	while (repeat == 'y')
	{
		cout << endl;
		auto n = UiPromptInteger("Unesi potenciju: ", 1, MaxIterations);

		// Mjeri vrijeme racunanja.
		auto start = chrono::high_resolution_clock::now();

		auto BrPi = CalculateMonteCarloPi(n);
		auto srVrij = CalculateAverages(BrPi, n);
		auto stDev = CalculateStdDevs(BrPi, srVrij, n);

		auto elapsed = chrono::high_resolution_clock::now() - start;
		auto us = chrono::duration_cast<chrono::microseconds>(elapsed).count();
		cout << "Vrijeme: " << us / 1000.0 << " ms" << endl;

		ConsolePrintResults(BrPi, srVrij, stDev, n);

		GuiPrintResults(BrPi, srVrij, stDev, n);
		canvas.Update();

		cout << endl;
		repeat = UiPromptChar("Zelite li ponoviti sve eksperimente s drugom potencijom (y/n)?");
	};

	system("PAUSE");
	return 0;
}

void GuiPrintResults(const PiMatrix& pis, const PiArray& avg, const PiArray& stDev, int n)
{
	TH1F* h = new TH1F("h", "example histogram", n, 2.0, 4.0);
	for (int i = 0; i < n; i++)
	{

		h->Fill(avg[i]);
	}
	h->Draw();
}


PiMatrix CalculateMonteCarloPi(int n)
{
	assert((n >= 1) && (n <= MaxIterations));

	PiMatrix BrPi{};

	for (int k = 0; k < n; k++) {
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
	return BrPi;
}

PiArray CalculateAverages(const PiMatrix& pis, int n)
{
	PiArray srVrij{};

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			srVrij[i] += pis[j][i];
		}
		srVrij[i] = srVrij[i] / ((double)n);
	}
	return srVrij;
}

PiArray CalculateStdDevs(const PiMatrix& pis, const PiArray& avg, int n)
{
	/*
		sqrt(suma(x_i - x_sr)^2/n)
	*/

	PiArray stDev{};
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			stDev[i] += pow(pis[j][i] - avg[i], 2);
		}
		stDev[i] = sqrt(stDev[i] / ((double)n));
	}
	return stDev;
}

double GetRandomNumber()
{
	constexpr double maxRandInt = numeric_limits<unsigned int>::max();

	// 'static' ovdje znaci da se ova inicijalizacija izvrsava samo kod prvog poziva funkcije GetRandomNumber();
	static std::random_device rd;
	static std::mt19937 rng{ rd() };
	static uniform_int_distribution<unsigned int> distribution(0, (unsigned int)maxRandInt - 1);
	
	// Racunanje slucajnog broja kod svakog poziva funkcije GetRandomNumber();
	return distribution(rng) / maxRandInt;
}

int UiPromptInteger(const string& prompt, int min, int max)
{
	int n = 0;
	do
	{
		cout << prompt << endl;
		cin >> n;
		if (cin.fail()) 
		{
			// Not an int.
			cin.clear();
			cin.ignore(256, '\n');
		}
		if ((n < min) || (n > max))
		{
			cout << "------- Neispravan unos -------" << endl;
		}
	} while ((n < min) || (n > max));
	return n;
}

char UiPromptChar(const string& prompt)
{
	char n = 0;
	cout << prompt << endl;
	cin >> n;
	return n;
}

void ConsolePrintResults(const PiMatrix& pis, const PiArray& avg, const PiArray& stDev, int n)
{
	cout << std::fixed;
	cout << std::setprecision(5);

	cout << endl;

	auto odg = UiPromptChar("Zelite li ispisati dobivene pi-jeve (y/n)?");
	if (odg == 'y')
	{
		cout << endl << "Skup pi-jeva dobijen pomocu Monte Carlo metode." << endl;
		PrintPiMatrix(pis, n);
	}

	cout << endl << "Ovdje su srednje vrijednosti po identicnom eksperimentu." << endl;
	for (int i = 0; i < n; i++)
	{
		cout << "Srednja vrijednost za " << i + 1 << "-ti eksperiment je: " << avg[i] << endl;
	}

	cout << endl << "Ovdje su standardne devijacije po identicnom eksperimentu." << endl;
	for (int i = 0; i < n; i++)
	{
		cout << "Standardne devijacije " << i + 1 << "-tog eksperimenta: " << stDev[i] << endl;
	}

	cout << endl;
	for (int i = 0; i < n; i++)
	{
		cout << "Srednja vrijednost i standardna devijacija " << i + 1 << "-tog eksperimenta: " << avg[i] << " +- " << stDev[i] << endl;
	}
}

void PrintPiMatrix(const PiMatrix& pis, int n)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cout << "(" << i + 1 << ", " << j + 1 << ")-ti pi je: " << pis[i][j] << " ";
		}
		cout << endl;
	}
}

//int main()
//{
//	cout << std::fixed;
//	cout << std::setprecision(5);
//	srand(time(NULL));
//	double rand_x, rand_y, BrPi[11][11];
//	double polozaj;
//	int n;
//	char odg, odg1;
//	double srVrij[11] = { 0.0 }, stDev[11] = { 0.0 };
//
//skok:
//	cout << "Unesi potenciju: " << endl;
//	cin >> n;
//
//	for (int k = 0; k < n; k++) {
//		for (int j = 0; j < n; j++) {
//			int BrTuKrug = 0;
//			for (int i = 0; i < pow(10, j); i++)
//			{
//				rand_x = ((double)rand() / (RAND_MAX));
//				rand_y = ((double)rand() / (RAND_MAX));
//				polozaj = pow(rand_x, 2) + pow(rand_y, 2);
//				if (sqrt(polozaj) <= 1)
//					BrTuKrug++;
//
//			}
//			double pi = ((double)BrTuKrug / pow(10, j) * 4);
//			BrPi[k][j] = pi;
//		}
//	}
//	cout << "Zelite li ispisati dobivene pi-jeve?(y/n)" << endl;
//	cin >> odg;
//	if (odg == 'y')
//	{
//		cout << "Skup \pi-jeva dobijen pomocu Monte Carlo metode." << endl;
//		for (int i = 0; i < n; i++)
//		{
//			for (int j = 0; j < n; j++)
//			{
//				cout << "(" << i + 1 << ", " << j + 1 << ")-ti \pi je: " << BrPi[i][j] << " ";
//
//			}
//			cout << endl;
//		}
//	}
//	else
//	{
//
//	}
//	/*Treba skužit kako uključiti grafove */
//
///*Srednja vrijednost i standardna devijacija*/
//
//
//	cout << "Ovdje su srednje vrijednosti po identicnom eksperimentu." << endl;
//
//	for (int i = 0; i < n; i++)
//	{
//
//		for (int j = 0; j < n; j++)
//		{
//			srVrij[i] += BrPi[j][i];
//		}
//		srVrij[i] = srVrij[i] / ((double)n);
//		cout << "Srednja vrijednost za " << i + 1 << "-ti eksperiment je: " << srVrij[i] << endl;
//	}
//
//	cout << "Ovdje su standardne devijacije po identicnom eksperimentu." << endl;
//	/*
//		sqrt(suma(x_i - x_sr)^2/n)
//	*/
//	for (int i = 0; i < n; i++)
//	{
//		for (int j = 0; j < n; j++)
//		{
//			stDev[i] += pow(BrPi[j][i] - srVrij[i], 2);
//		}
//		stDev[i] = sqrt(stDev[i] / ((double)n));
//	}
//	for (int i = 0; i < n; i++)
//	{
//		cout << "Standardne devijacije " << i + 1 << "-tog eksperimenta: " << stDev[i] << endl;
//	}
//
//	for (int i = 0; i < n; i++)
//	{
//		cout << "Srednja vrijednost i standardna devijacija" << i + 1 << "-tog eksperimenta: " << srVrij[i] << " +- " << stDev[i] << endl;
//	}
//
//	cout << "Zelite li ponoviti sve eksperimente s drugom potencijom?(y/n)" << endl;
//	cin >> odg1;
//	if (odg1 == 'y')
//		goto skok;
//	else
//	{
//		goto stop;
//	}
//stop:
//	system("PAUSE");
//	return 0;
//}
