#pragma once
#define __find_end find_end

#include <array>
#include <string>

constexpr int MaxIterations = 11;
using PiArray = std::array<double, MaxIterations>;
using PiMatrix = std::array<PiArray, MaxIterations>;

PiMatrix CalculateMonteCarloPi(int n);
PiArray CalculateAverages(const PiMatrix& pis, int n);
PiArray CalculateStdDevs(const PiMatrix& pis, const PiArray& avg, int n);
void PrintPiMatrix(const PiMatrix& pis, int size);
int UiPromptInteger(const std::string& prompt, int min, int max);
char UiPromptChar(const std::string& prompt);
void ConsolePrintResults(const PiMatrix& pis, const PiArray& avg, const PiArray& stDev, int n);
