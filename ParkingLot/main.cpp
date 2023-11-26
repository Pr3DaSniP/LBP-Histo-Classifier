#include <iostream>

#include "Timer.h"
#include "function.h"
#include "config.h"
#include "ProgressBar.h"
#include <functional>

void train() 
{
	Timer t; CConfig c;

	vector<std::string> images;
	for (auto& file : TRAIN_FILES) {
		auto tmp = loadImage(file);
		images.insert(images.end(), tmp.begin(), tmp.end());
	}
	ProgressBar bar(images.size());

	std::ofstream fOUT(MODEL_FILE, std::ios::out);

	int number = 0;
	for(auto& image : images) {
		bar.update(++number);
		
		std::vector<bool> classes(TRAIN_FILES.size(), false);
		if(image.find("cardboard") != std::string::npos)
			classes[0] = true;
		else if (image.find("glass") != std::string::npos)
			classes[1] = true;
		else if (image.find("metal") != std::string::npos)
			classes[2] = true;
		else if (image.find("paper") != std::string::npos)
			classes[3] = true;
		else if (image.find("plastic") != std::string::npos)
			classes[4] = true;
		else if (image.find("trash") != std::string::npos)
			classes[5] = true;

		Mat img; 
		if (USE_RGB_TO_GREY) {
			img = cv::imread(image);
			img = splitInto1Channel(img);
		}
		else
			img = cv::imread(image, cv::IMREAD_GRAYSCALE);

		if(USE_DETECT_CONTOURS)
			img = detectContours(img);
		
		vector<double> vec_desc;
		if (USE_PYRAMIDAL) {
			vector<Mat> splitIn3x3 = splitIn(img, 3);
			vector<Mat> splitIn2x2 = splitIn(img, 2);

			vector<Mat> lbps;
			for (auto& img : splitIn3x3)
				lbps.push_back(DESCRIPTOR_FUNC(img));

			for (auto& img : splitIn2x2)
				lbps.push_back(DESCRIPTOR_FUNC(img));

			lbps.push_back(DESCRIPTOR_FUNC(img));

			vector<vector<double>> histos_lbp;
			for (auto& lbp : lbps)
				histos_lbp.push_back(calculHisto(lbp));

			vec_desc = concat(histos_lbp);
		}
		else {
			img = DESCRIPTOR_FUNC(img);
			vec_desc = calculHisto(img);
		}

		if(NORMALISATION)
			vec_desc = normalisation(vec_desc, img.cols * img.rows);

		fOUT << getIndexClasse(classes) << ": ";
		for (auto& value : vec_desc) {
			fOUT << value << " ";
		}
		fOUT << "\n";
	}

	std::cout << "\n\n";

	fOUT.close();
}

void test()
{
	Timer t; CConfig c;

	int number = 0;

	vector<int> total_number(TRAIN_FILES.size(), 0);
	vector<int> total_predict_number(TRAIN_FILES.size(), 0);

	vector<std::string> images;
	for (auto& file : TEST_FILES) {
		auto tmp = loadImage(file);
		images.insert(images.end(), tmp.begin(), tmp.end());
	}
	ProgressBar bar(images.size());

	std::ofstream fOUT(PREDICT_FILE, std::ios::out);
	std::ifstream model(MODEL_FILE, std::ios::in);
	
	std::ofstream csv(PREDICT_DIR + "predict_" + DESCRIPTOR_FUNC_NAME + "_" + DIST_FUNC_NAME + ".csv", std::ios::out);
	csv << "Filename,Labels\n";

	std::vector<Sample> model_vec_desc;
	std::string line;
	while (std::getline(model, line)) {
		model_vec_desc.push_back(getSample(line));
	}

	for (auto& image : images) {
		bar.update(++number);

		Mat img;
		if (USE_RGB_TO_GREY) {
			img = cv::imread(image);
			img = splitInto1Channel(img);
		}
		else
			img = cv::imread(image, cv::IMREAD_GRAYSCALE);

		if (USE_DETECT_CONTOURS)
			img = detectContours(img);

		vector<double> vec_desc;
		if (USE_PYRAMIDAL) {
			vector<Mat> splitIn3x3 = splitIn(img, 3);
			vector<Mat> splitIn2x2 = splitIn(img, 2);
			
			vector<Mat> lbps;
			for (auto& img : splitIn3x3)
				lbps.push_back(DESCRIPTOR_FUNC(img));
				
			for (auto& img : splitIn2x2)
				lbps.push_back(DESCRIPTOR_FUNC(img));
			
			lbps.push_back(DESCRIPTOR_FUNC(img));
			
			vector<vector<double>> histos_lbp;
			for (auto& lbp : lbps)
				histos_lbp.push_back(calculHisto(lbp));
			
			vec_desc = concat(histos_lbp);
		}
		else {
			img = DESCRIPTOR_FUNC(img);
			vec_desc = calculHisto(img);
		}

		if(NORMALISATION)
			vec_desc = normalisation(vec_desc, img.cols * img.rows);

		vector<double> dist;
		for (auto& sample : model_vec_desc) {
			dist.push_back(DIST_FUNC(vec_desc, sample.desc));
		}

		vector<double>::iterator res;
		if(SEARCH_MAX)
			res = std::max_element(dist.begin(), dist.end());
		else 
			res = std::min_element(dist.begin(), dist.end());

		int index = std::distance(dist.begin(), res);

		int predict = model_vec_desc[index].classe;

		fOUT << predict << " ------- " << image << "\n";
		
		std::string filename = image.substr(image.find_last_of("/\\") + 1);
		csv << filename << "," << getClasse(predict) << "\n";
	}

	/*
	int number_full = number - number_free;

	double pourcentage_free = (double)predict_number_free / number_free * 100;
	double pourcentage_full = (double)predict_number_full / number_full * 100;
	double pourcentage_total = (double)(predict_number_free + predict_number_full) / number * 100;

	std::string pourcents = "Pourcentage: \n";
	pourcents += "   --> FREE:\t" + std::to_string(pourcentage_free) + "%\n";
	pourcents += "   --> FULL:\t" + std::to_string(pourcentage_full) + "%\n";
	pourcents += "   --> TOTAL:\t" + std::to_string(pourcentage_total) + "%\n";
	std::cout << "\n\n" << pourcents << "\n\n";

	fOUT << "\n\n";
	fOUT << pourcents;
	fOUT << "\n\n";
	*/
	fOUT << t.elapsed();

	fOUT.close();
	csv.close();
	model.close();
}

int main() {

	if(DO_TRAIN)
		train();

	if(DO_TEST)
		test();

	return 0;
}