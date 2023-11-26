#include "ProgressBar.h"

ProgressBar::ProgressBar(int totalSteps) : m_totalSteps(totalSteps), m_currentStep(0) {
    startTime = std::chrono::high_resolution_clock::now();
}

void ProgressBar::update(int step) {
    m_currentStep = step;
    draw();
}

void ProgressBar::draw() {
    float progress = static_cast<float>(m_currentStep) / m_totalSteps;
    int barWidth = 50;
    int numChars = static_cast<int>(barWidth * progress);

    std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedTime = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime);
    double etaSeconds = (elapsedTime.count() / progress) - elapsedTime.count();

    int etaMinutes = static_cast<int>(etaSeconds) / 60;
    int etaSecondsRemaining = static_cast<int>(etaSeconds) % 60;

    std::cout << "[";

    for (int i = 0; i < barWidth; ++i) {
        if (i == numChars) {
            std::cout << ">";
        }
        else if (i < numChars) {
            std::cout << "=";
        }
        else {
            std::cout << ".";
        }
    }

    std::cout << "] " << std::fixed << std::setprecision(1) << (progress * 100.0) << "% ETA: " << etaMinutes << "m " << etaSecondsRemaining << "s\r" << m_currentStep << " / " << m_totalSteps << " ";
    std::cout.flush();
}