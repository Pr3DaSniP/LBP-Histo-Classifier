#pragma once

#include "function.h"

#define DESCRIPTOR_FUNC LBP
constexpr auto DESCRIPTOR_FUNC_NAME = "LBP";

/*
*	distanceManhattan : MIN
*	distanceEuclidienne : MIN
*   distanceChi2 : MIN
*   distanceIntersection : MAX
*	distanceBhattacharyya : MIN
*   distanceCorrelation : MAX
*/
#define DIST_FUNC distanceChi2
constexpr auto DIST_FUNC_NAME = "distanceChi2";

// 100 | 2k | 20k | 42k | Garbage
const std::string NUMBER_OF_IMAGES = "Garbage";

// Use if u want to get max instead of min
const bool SEARCH_MAX = false;

// Use if u want normalization
const bool NORMALISATION = true;

// Use if u want to use color
const bool USE_RGB_TO_GREY = true;

// Use if u want to compute pyramidal
const bool USE_PYRAMIDAL = true;

const bool USE_DETECT_CONTOURS = true;


const bool DO_TRAIN = true;
const bool DO_TEST = true;

const std::string DATASET_PATH = "Dataset" + NUMBER_OF_IMAGES + "/";

const std::string IS_NORMALISED = NORMALISATION ? "N_" : "";
const std::string IS_RGB_TO_GREY = USE_RGB_TO_GREY ? "RGB_" : "";
const std::string IS_PYRAMIDAL = USE_PYRAMIDAL ? "P_" : "";
const std::string IS_DETECT_CONTOURS = USE_DETECT_CONTOURS ? "DC_" : "";

const std::string MODEL_DIR = "Models/Garbage/";
const std::string MODEL_FILE = MODEL_DIR + IS_DETECT_CONTOURS + IS_PYRAMIDAL + IS_RGB_TO_GREY + IS_NORMALISED + NUMBER_OF_IMAGES + "_"+ DESCRIPTOR_FUNC_NAME + "_" + "model.txt";

const std::string PREDICT_DIR = "Predictions/Garbage/";
const std::string PREDICT_FILE = PREDICT_DIR + IS_DETECT_CONTOURS + IS_PYRAMIDAL + IS_RGB_TO_GREY + IS_NORMALISED + NUMBER_OF_IMAGES + "_" + DESCRIPTOR_FUNC_NAME + "_" + DIST_FUNC_NAME + "_predictions.txt";

const vector<std::string> TRAIN_FILES = {
	DATASET_PATH + "Train/cardboard/",
	DATASET_PATH + "Train/glass/",
	DATASET_PATH + "Train/metal/",
	DATASET_PATH + "Train/paper/",
	DATASET_PATH + "Train/plastic/",
	DATASET_PATH + "Train/trash/",
};

const vector<std::string> TEST_FILES = {
	DATASET_PATH + "Test/"
};


class CConfig
{
public:
	CConfig() {
        // Affiche la configuration actuelle
        std::cout << "Configuration actuelle : \n";
        std::cout << "   --> DIST_FUNC : " << DIST_FUNC_NAME << "\n";
		std::cout << "   --> DESCRIPTOR_FUNC : " << DESCRIPTOR_FUNC_NAME << "\n";
        std::cout << "   --> SEARCH_MAX : " << (SEARCH_MAX ? "Maximum" : "Minimum") << "\n";
        std::cout << "   --> NORMALISATION : " << (NORMALISATION ? "Oui" : "Non") << "\n";
        std::cout << "   --> USE_RGB_TO_GREY : " << (USE_RGB_TO_GREY ? "Oui" : "Non") << "\n";
        std::cout << "   --> USE_PYRAMIDAL : " << (USE_PYRAMIDAL ? "Oui" : "Non") << "\n";
        std::cout << "\n";
		std::cout << "   --> MODELE : " << MODEL_FILE << "\n";
		if (DO_TEST)
		    std::cout << "   --> PREDICTION : " << PREDICT_FILE << "\n";
		std::cout << "\n\n";
	}
};