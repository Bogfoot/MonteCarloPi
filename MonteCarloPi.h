#pragma once

#include <string>
#include <chrono>

// Definicije i pragme koje ignoriraju errore i warninge kod kompajliranja ROOT-a 
#define __find_end find_end //stari compiler javlja grešku ; samo komentirati 
#pragma warning (push)
#pragma warning (disable: 4996)
#pragma warning (disable: 4068)
#pragma warning (disable: 4706)
#pragma warning (disable: 4127)
#pragma warning (disable: 4100)
#pragma warning (disable: 4245)
#pragma warning (disable: 4244)

#include <TThread.h>
#include <TApplication.h>
#include <TF1.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TFrame.h>
#include <TPaveLabel.h>
#include <TPaveText.h>
#include <TArrow.h>
#include <TGraphErrors.h>
#include <TGraph.h>


// Vrati warning nivo nakon kompajliranja ROOT-a
#pragma warning (pop)

#include "MonteCarloPiCalculator.h"


// Klasa MonteCarloApp sluzi za grupiranje UI elementa i omogucava multi-thread rad.
class MonteCarloApp : public TApplication
{
public:
	MonteCarloPiCalculator PiCalc;

	MonteCarloApp(int argc, char* argv[]) : PiCalc{}, TApplication { "ROOT Application", & argc, argv }
	{
		// Inicijalizacija top-level kanvasa
		c48 = new TCanvas("c48", "c48", 200, 10, 900, 600); //   TCanvas(const char *name, const char *title, Int_t wtopx, Int_t wtopy,	Int_t ww, Int_t wh);
		c48->GetFrame()->SetFillColor(21);
		c48->GetFrame()->SetBorderSize(12);
		c48->Divide(2, 2);
	}

	virtual void StartIdleing()
	{
		// Ova se funkcija poziva iz ROOT UI threada.

		TApplication::StartIdleing();

		// Re-draw when data changes.
		if (invalidated)
		{
			invalidated = false;
			UpdateCanvas(PiCalc);
		}
	}

	void Invalidate()
	{
		invalidated = true;
	}

private:
	TCanvas* c48 = nullptr;
	bool invalidated = false;

	void UpdateCanvas(const MonteCarloPiCalculator& calc);
};


struct Stopwatch
{
   void Start()
   {
      start = std::chrono::high_resolution_clock::now();
   }

   double ElapsedMilliseconds()
   {
      using namespace std::chrono;

      auto elapsed = high_resolution_clock::now() - start;
      auto us = duration_cast<microseconds>(elapsed).count();
      return us / 1000.0;
   }

private:
   std::chrono::steady_clock::time_point start;
};


void PrintPiMatrix(const PiMatrix& pis, int size);
int UiPromptInteger(const std::string& prompt, int min, int max);
char UiPromptChar(const std::string& prompt);
void ConsolePrintResults(const PiMatrix& pis, const PiArray& avg, const PiArray& stDev, int n, const PiMatrix& relDiff, int m, const PiArray& relDiffsrVrij, const PiArray& relDiffstDev);
void PrintRelDiffMatrix(const PiMatrix& relDiff, int n, int m);