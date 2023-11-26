#include "Timer.h"

Timer::Timer()
{
	start = std::chrono::high_resolution_clock::now();
}

std::string Timer::elapsed() {
	end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration = end - start;
	// Millisecondes
	std::string elapsed = "Temps d'execution : \n   -->\t" + std::to_string(duration.count() * 1000.0f) + " millisecondes";
	// Secondes
	elapsed += "\n   -->\t" + std::to_string(duration.count()) + " secondes";
	// Minutes
	elapsed += "\n   -->\t" + std::to_string(duration.count() / 60) + " minutes";
	return elapsed;
}

Timer::~Timer()
{
	end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration = end - start;
	// Millisecondes
	std::cout << "Temps d'execution : \n   -->\t" << duration.count() * 1000.0f << " millisecondes" << std::endl;
	// Secondes
	std::cout << "   -->\t" << duration.count() << " secondes" << std::endl;
	// Minutes
	std::cout << "   -->\t" << duration.count() / 60 << " minutes" << std::endl;
}