#include <assert.h>
#include <iostream>
#include <random>
#include <math.h>
#include <iomanip>
#include <fstream>

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

		ConsolePrintResults(app->PiCalc.BrPi, app->PiCalc.srVrij, app->PiCalc.stDev, app->PiCalc.n, app->PiCalc.relDiff, app->PiCalc.m, app->PiCalc.relDiffsrVrij, app->PiCalc.relDiffstDev);
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
int main(int argc, char* argv[], char* /*envp[]*/) //tu se javlja neki "zeleni" error u vezi veličine stacka,
												   //ne znam što to znači ni kako popraviti
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

	//prvi graf (1,1)
	c48->cd(1); 
	auto* gr = new TGraphErrors(calc.n, calc.pot.data(), calc.srVrij.data(), nullptr, calc.stDev.data());//Moraju biti pointeri
	gr->SetTitle("Graficki prikaz srednje vrijednosti po potenciji");
	gr->GetXaxis()->SetTitle("Potencija");
	gr->GetYaxis()->SetTitle("Srednja vrijednost");
	gr->GetXaxis()->SetRangeUser(-0.5, 7.);
	gr->SetLineColor(1);
	gr->SetLineWidth(1);
	gr->SetMarkerColor(4);
	gr->SetMarkerSize(1.5);
	gr->SetMarkerStyle(5);
	gPad->DrawFrame(0, 0, 4, 8);
	gr->Draw("ALP");
	c48->Modified();
	c48->Update();

	// Drugi grafikon (1,2)
	c48->cd(2);
	auto* gr2 = new TGraphErrors(calc.n, calc.pot.data(), calc.srVrij.data(), nullptr, calc.stDev.data());
	gr2->SetTitle("Isto kao i prvi grafikon");
	gr2->GetXaxis()->SetTitle("Potencija");
	gr2->GetYaxis()->SetTitle("Srednja vrijednost");
	gr2->GetXaxis()->SetRangeUser(-0.5, 7.);
	gr2->SetMarkerStyle(5);
	gPad->DrawFrame(0, 0, 4, 8);
	gr2->Draw("AP");
	c48->Modified();
	c48->Update();
	
	
	////treći graf (2,1) Možda je bolje koristiti TGraph(const TVectorD &vx, const TVectorD &vy); za vektore
	c48->cd(3);
	auto* gr3 = new TGraphErrors(calc.n, calc.pot.data(), calc.relDiffsrVrij.data(), nullptr, calc.relDiffstDev.data());
	gr3->SetTitle("Srednja vrijednost apsolutnih razlika broja Pi");
	gr3->GetXaxis()->SetTitle("Potencija");
	gr3->GetYaxis()->SetTitle("Absolutna razlika");
	gr3->GetXaxis()->SetRangeUser(-0.5, 7.);
	gr3->SetLineColor(1);
	gr3->SetLineWidth(1);
	gr3->SetMarkerColor(4);
	gr3->SetMarkerSize(1.5);
	gr3->SetMarkerStyle(5);
	gPad->DrawFrame(0, 0, 4, 8);
	gr3->Draw("ALP");
	c48->Modified();
	c48->Update();


	////četvrti graf (2,2) 
	c48->cd(4);
	auto* gr4 = new TGraphErrors(calc.n, calc.pot.data(), calc.relDiffsrVrij.data(), nullptr, calc.relDiffstDev.data());
	gr4->SetTitle("Isto kao i prijasnji graf");
	gr4->GetXaxis()->SetTitle("Potencija");
	gr4->GetYaxis()->SetTitle("Absolutna razlika");
	gr4->GetXaxis()->SetRangeUser(-0.5, 7.);
	gr4->SetMarkerStyle(5);
	gr4->SetMarkerSize(1.5);
	gPad->DrawFrame(0, 0, 4, 8);
	gr4->Draw("AP");
	c48->Modified();
	c48->Update();
	
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

void ConsolePrintResults(const PiMatrix& pis, const PiArray& avg, const PiArray& stDev, int n, const PiMatrix& relDiff, int m, const PiArray& relDiffsrVrij, const PiArray& relDiffstDev)
{
	cout << std::fixed;
	cout << std::setprecision(9);

	cout << endl;

	auto odg = UiPromptChar("Zelite li ispisati dobivene pi-jeve i relativne razlike? (y/n)");
	if (odg == 'y')
	{
		cout << endl << "Skup pi-jeva dobive pomocu Monte Carlo metode." << endl;
		PrintPiMatrix(pis, n, m);
		cout << endl << "Skup relativnih razlika broja pi dobiveni pomocu Monte Carlo metode." << endl;
		PrintPiMatrix(relDiff, n, m);
	}
	cout << endl;
	
	cout << endl << "Ovdje su srednje vrijednosti i standardne devijacije po identicnom eksperimentu." << endl;
	PrintAllResults(avg, stDev, n);

	cout << endl << "Ovdje su absolutne srednje vrijednosti i standardne devijacije po identicnom eksperimentu." << endl;
	PrintAllResults(relDiffsrVrij, relDiffstDev, n);

	//spremanje rezultata
	SaveResults(avg, stDev, n, relDiffsrVrij, relDiffstDev); //radim ovo tu jer mi je najjednostavnije ovako
	
}
void SaveResults(const PiArray& avg, const PiArray& stDev, int n , const PiArray& relavg, const PiArray& relstDev)
{
	ofstream outdata;
	outdata.open("data.txt");
	outdata << "Potencija \t Srednja vrijednost pi-jeva \t Standardna devijacija srednjih vrijednosti pi-jeva \t Absolutno odstupanje \t Standardna devijacija absolutnog odstupanja \n";
	for (int i = 0; i < n; ++i)
	{
		outdata <<i <<",	"<< avg[i]<<",		" << stDev[i] << ",		" << relavg[i] << ",		" << relstDev[i] << endl;
	}
	outdata.close();
}



void PrintAllResults(const PiArray& avg, const PiArray& stDev, int n)
{

	cout << endl;
	for (int i = 0; i < n; i++)
	{
		cout << "Srednja vrijednost i standardna devijacija " << i + 1 << "-tog eksperimenta: " << avg[i] << " +- " << stDev[i] << endl;
	}
	
}

void PrintPiMatrix(const PiMatrix& pis, int n, int m)
{
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cout << "(" << i + 1 << ", " << j + 1 << ")-ti pi je: " << pis[i][j] << " ";
		}
		cout << endl << endl;
	}
}