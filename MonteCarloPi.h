#pragma once

#include <array>
#include <string>
#include <chrono>

// Deinicije i pragme koje ispravljaju errore i warninge kod kompajliranja ROOT-a 
#define __find_end find_end
#pragma warning (push)
#pragma warning (disable: 4996)
#pragma warning (disable: 4068)
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

// Vrati warning nivo nakon kompajliranja ROOT-a
#pragma warning (pop)



// Klasa MonteCarloApp sluzi za grupiranje UI elementa i omogucava multi-thread rad.
class MonteCarloApp : public TApplication
{
public:

	MonteCarloApp(int argc, char* argv[]) : TApplication{ "ROOT Application", &argc, argv }
	{
	}

	// Ovdje treba dodati sve UI elemente vidljive na TCanvas-u.
	TCanvas canvas{ "c1" };
	TH1F th1f{ "th1f", "", 11, 2.0, 4.0 };

	virtual void StartIdleing()
	{
		TApplication::StartIdleing();

		// Re-draw when data changes.
		if (invalidated)
		{
			invalidated = false;

			th1f.Draw();
			canvas.Update();
		}
	}

	void Invalidate()
	{
		invalidated = true;
	}

private:
	bool invalidated = false;
};


constexpr int MaxIterations = 11;
using PiArray = std::array<double, MaxIterations>;
using PiMatrix = std::array<PiArray, MaxIterations>;

PiMatrix CalculateMonteCarloPi(int n);
PiArray CalculateAverages(const PiMatrix& pis, int n);
PiArray CalculateStdDevs(const PiMatrix& pis, const PiArray& avg, int n);
double GetRandomNumber();
void PrintPiMatrix(const PiMatrix& pis, int size);
int UiPromptInteger(const std::string& prompt, int min, int max);
char UiPromptChar(const std::string& prompt);
void ConsolePrintResults(const PiMatrix& pis, const PiArray& avg, const PiArray& stDev, int n);
void GuiPrintResults(MonteCarloApp* app, const PiMatrix& pis, const PiArray& avg, const PiArray& stDev, int n);
void Potencija(int n);


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