#pragma once

#include <iostream>
#include <iomanip>
#include <chrono>

class ProgressBar
{
public:
    ProgressBar(int totalSteps);
    void update(int step);

private:
    int m_totalSteps;
    int m_currentStep;
    std::chrono::high_resolution_clock::time_point startTime;

    void draw();
};