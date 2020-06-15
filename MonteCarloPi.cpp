﻿#include <assert.h>
#include <iostream>
#include <random>
#include <math.h>
#include <iomanip>

#include "MonteCarloPi.h"

using namespace std;


// Glavna petlja programa cita korisnicki unos i pokrece racunanje.
void MainLoop(void* arg)
{
	auto* app = (MonteCarloApp*)arg;

	Stopwatch sw;
	char repeat = 'y';
	while (repeat == 'y')
	{
		cout << endl;
		auto n = UiPromptInteger("Unesi potenciju: ", 1, MaxIterations);

		sw.Start();
		app->PiCalc.Calculate(n);
		cout << "Vrijeme: " << sw.ElapsedMilliseconds() << " ms" << endl;

		ConsolePrintResults(app->PiCalc.BrPi, app->PiCalc.srVrij, app->PiCalc.stDev, app->PiCalc.n, app->PiCalc.relDiff, app->PiCalc.m);
		app->Invalidate();

		cout << endl;
		repeat = UiPromptChar("Zelite li ponoviti sve eksperimente s drugom potencijom (y/n)?");
	};
	
}


/*
	Treba napraviti MCSim za generiranje broja \pi.
	U kvadratu površine 1 (1x1) generirati će se parovi brojeva (x,y)&[0-1].
	Upisati kružnicu radiusa 1, površine 1 * 1 * \pi= \pi. Prebrojati koliko se točaka nalazi u kružnici.
	BrTuKrug/BrTuKvad=\pi/4
*/
int main(int argc, char* argv[], char* /*envp[]*/)
{
	
	// Glavni ROOT prozor.
	MonteCarloApp app{argc, argv};

	// Deklariraj i pokreni 'background' thread.
	// Background thread cita komande s konzole i izvrsava racunanje.
	TThread thread("mainLoop", MainLoop);
	thread.Run(&app);

	// Pokreni ROOT 
	app.Run(true);

	// Zustavi 'backgrond' thread ako je jos uvijek aktivan.
	thread.Kill();

	return 0;
}

void MonteCarloApp::UpdateCanvas(const MonteCarloPiCalculator& calc)
{
	c48->cd(1);
	auto* gr = new TGraphErrors(calc.n, calc.pot.data(), calc.srVrij.data(), nullptr, calc.stDev.data());//Moraju biti pointeri
	gr->SetTitle("Graficki prikaz srednje vrijednosti po potenciji");
	gr->GetXaxis()->SetTitle("Potencija");
	gr->GetYaxis()->SetTitle("Srednja vrijednost");
	gr->SetLineColor(2);
	gr->SetLineWidth(4);
	gr->SetMarkerColor(4);
	gr->SetMarkerSize(1.5);
	gr->SetMarkerStyle(21);
	

	gPad->DrawFrame(0, 0, 4, 8);
	gr->Draw("ALP");
	c48->Modified();
	c48->Update();

	// Drugi grafikon, samo za probu
	c48->cd(2);
	auto* gr2 = new TGraphErrors(calc.n, calc.pot.data(), calc.srVrij.data(), nullptr, calc.stDev.data());
	gr2->SetTitle("Isto kao i prvi grafikon");
	gr2->GetXaxis()->SetTitle("Potencija");
	gr2->GetYaxis()->SetTitle("Srednja vrijednost");
	gPad->DrawFrame(0, 0, 4, 8);
	gr2->Draw("AP");
	c48->Modified();
	c48->Update();
	
	//treći graf
	/*
	c48->cd(3);
	auto* gr3 = new TGraphErrors(calc.m, calc.pot.data(), calc.relDiff.data(), nullptr, nullptr);
	gr3->SetTitle("Relativna razlika");
	gr3->GetXaxis()->SetTitle("Potencija");
	gr3->GetYaxis()->SetTitle("Relativna Razlika");
	gr3->SetLineColor(2);
	gr3->SetLineWidth(4);
	gr3->SetMarkerColor(4);
	gr3->SetMarkerSize(1.5);
	gr3->SetMarkerStyle(21);
	gPad->DrawFrame(0, 0, 4, 8);
	gr3->Draw("ALP");
	c48->Modified();
	c48->Update();

	//četvrti graf
	c48->cd(4);
	auto* gr4 = new TGraphErrors(calc.m, calc.pot.data(), calc.relDiff.data(), nullptr, nullptr);
	gr4->SetTitle("Relativna razlika");
	gr4->GetXaxis()->SetTitle("Potencija");
	gr4->GetYaxis()->SetTitle("Relativna Razlika");
	gPad->DrawFrame(0, 0, 4, 8);
	gr4->Draw("AP");
	c48->Modified();
	c48->Update();
	*/
	

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

void ConsolePrintResults(const PiMatrix& pis, const PiArray& avg, const PiArray& stDev, int n, const PiMatrix& relDiff, int m)
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
	cout << endl;
	PrintRelDiffMatrix(relDiff,n,m);
	

}

void PrintRelDiffMatrix(const PiMatrix& relDiff, int n, int m)
{
	cout << "Relaativna razlika od aproksimacije broja pi: \n";
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cout << "(" << i + 1 << "," << j + 1 << ")" << "-tog eksperimenta: " << relDiff[i][j];
		}
		cout <<endl <<endl;
	}
}

void PrintPiMatrix(const PiMatrix& pis, int n)
{
	for (int i = 0; i < 100; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cout << "(" << i + 1 << ", " << j + 1 << ")-ti pi je: " << pis[i][j] << " ";
		}
		cout << endl;
	}
}
