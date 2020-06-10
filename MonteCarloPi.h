#pragma once
#define __find_end find_end

#include <array>
#include <string>
#include <chrono>


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
void GuiPrintResults(const PiMatrix& pis, const PiArray& avg, const PiArray& stDev, int n);
void Potencija(int n);


struct Stopwatch
{
   void Start()
   {
      start = std::chrono::high_resolution_clock::now();
   }

   double StopAndElapsedMilliseconds()
   {
      using namespace std::chrono;

      auto elapsed = high_resolution_clock::now() - start;
      auto us = duration_cast<microseconds>(elapsed).count();
      return us / 1000.0;
   }

private:
   std::chrono::steady_clock::time_point start;
};