#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>

using std::vector;
using std::pair;

// Opencv stuff
#include <opencv2/opencv.hpp>

using cv::Mat;
using cv::Scalar;
using cv::Point;

struct Sample {
	int classe;
	vector<double> desc;

	friend std::ostream& operator<<(std::ostream& os, const Sample& s) {
		os << s.classe << ": ";
		for (auto& d : s.desc) {
			os << d << " ";
		}
		return os;
	}
};

const vector<pair<int, int>> Directions = {
	{-1 ,-1},	// Haut-gauche
	{ 0 ,-1},	// Haut
	{ 1 ,-1},	// Haut-droite
	{ 1 , 0},	// Droite
	{ 1 , 1},	// Bas-droite
	{ 0 , 1},	// Bas
	{-1 , 1},	// Bas-gauche
	{-1 , 0}	// Gauche
};

int s(int value) {
	return value <= 0 ? 1 : 0;
}

Mat LBP(const Mat& image) {
	Mat lbp = image.clone();
	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			int centre = image.at<uchar>(y, x);

			// Si le centre est sur le bord de l'image, on skip
			if (x == 0 || x == image.cols - 1 || y == 0 || y == image.rows - 1) {
				lbp.at<uchar>(y, x) = 0;
				continue;
			}

			int value = 0;
			for (int i = 0; i < 8; i++) {
				int x2 = x + Directions[i].first;
				int y2 = y + Directions[i].second;
				if (x2 >= 0 && x2 < image.cols && y2 >= 0 && y2 < image.rows) {
					int niveauGrisVoisin = image.at<uchar>(y2, x2);
					value += s(niveauGrisVoisin - centre) * pow(2, 7 - i);
				}
			}
			lbp.at<uchar>(y, x) = value;
		}
	}
	return lbp;
}

Mat MeanLBP(const Mat& image) {
	Mat mlbp = image.clone();
	for (int y = 1; y < image.rows; y++) {
		for (int x = 1; x < image.cols; x++) {
			double m = (
				image.at<uchar>(y - 1, x - 1) +
				image.at<uchar>(y - 1, x) +
				image.at<uchar>(y, x) +
				image.at<uchar>(y, x - 1)
				) / 4;

			mlbp.at<uchar>(y, x) =
				s(image.at<uchar>(y - 1, x - 1) - m) * pow(2, 3) +
				s(image.at<uchar>(y - 1, x) - m) * pow(2, 2) +
				s(image.at<uchar>(y, x) - m) * pow(2, 1) +
				s(image.at<uchar>(y, x - 1) - m);
		}
	}
	return mlbp;
}

double moyenne(const vector<double>& vec) {
	double acc = 0;
	for (int i = 0; i < vec.size(); i++)
		acc += vec[i];

	return acc / vec.size();
}

int getIndex(const vector<double>& vec, double value) {
	for (int i = 0; i < vec.size(); i++) {
		if (vec[i] == value) {
			return i;
		}
	}
	return -1;
}

vector<std::string> loadImage(const std::string& path) {
	vector<std::string> images;
	cv::glob(path + "*.jpg", images);
	return images;
}

Sample getSample(const std::string& line) {
	Sample vec;
	std::stringstream ss(line);
	// Tant que le token recu n'est pas un ":" on continue
	while (ss.peek() != ':') {
		double value;
		ss >> value;
		vec.classe = value;
	}
	// On skip le ":"	
	ss.ignore();
	// On ajoute les doubles dans le vecteur
	while (ss.peek() != EOF) {
		double value;
		ss >> value;
		vec.desc.push_back(value);
	}
	return vec;
}

int getNbLigne(std::ifstream& file) {
	int nbLigne = 0;
	std::string line;
	while (std::getline(file, line)) {
		nbLigne++;
	}
	return nbLigne;
}

vector<double> calculHisto(const Mat& image) {
	vector<double> histo(256, 0);

	for (int y = 0; y < image.rows; y++)
		for (int x = 0; x < image.cols; x++)
			histo[(int)image.at<uchar>(y, x)]++;

	return histo;
}

void afficherHistog(const vector<double>& hist, std::string nom) {
	vector<double> histogram = hist;

	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / 256);
	Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(255, 255, 255));

	// find the maximum intensity element from histogram
	auto max = *std::max_element(histogram.begin(), histogram.end());

	// normalize the histogram between 0 and histImage.rows
	for (int i = 0; i < histogram.size(); i++)
		histogram[i] = (int)(((1.0 * histogram[i]) / max) * histImage.rows + 0.5);

	// draw the intensity line for histogram
	for (int i = 0; i < histogram.size(); i++)
		line(histImage, Point(bin_w * (i), hist_h), Point(bin_w * (i), hist_h - histogram[i]), Scalar(0, 0, 0), 1, 8, 0);

	imshow("Histogramme " + nom, histImage);
}

vector<double> normalisation(const vector<double>& hist, int numberofpixel) {
	vector<double> normalise;
	for (int i = 0; i < hist.size(); i++)
		normalise.push_back(hist[i]);

	for (int i = 0; i < normalise.size(); i++)
		normalise[i] = normalise[i] / numberofpixel;

	return normalise;
}

Mat splitInto1Channel(const Mat& img) {
	Mat R, G, B;
	vector<Mat> channels;
	split(img, channels);
	R = channels[0];
	G = channels[1];
	B = channels[2];
	
	Mat result(img.rows, img.cols * 3, CV_8UC1);
	
	cv::hconcat(R, G, result);
	cv::hconcat(result, B, result);
	
	return result;
}

// Plus la valeur est proche de 0, plus les histogrammes sont similaires
double distanceManhattan(const vector<double>& hist1, const vector<double>& hist2) {
	double dist = 0;
	for (int i = 0; i < hist1.size(); i++) {
		dist += abs(hist1[i] - hist2[i]);
	}
	return dist;
}

// Plus la valeur est proche de 0, plus les histogrammes sont similaires
double distanceEuclidienne(const vector<double>& hist1, const vector<double>& hist2) {
	double dist = 0;
	for (int i = 0; i < hist1.size(); i++) {
		dist += pow(hist1[i] - hist2[i], 2);
	}
	return sqrt(dist);
}

// Plus la valeur est proche de 0, plus les histogrammes sont similaires
double distanceChi2(const vector<double>& hist1, const vector<double>& hist2) {
	double dist = 0;
	for (int i = 0; i < hist1.size(); i++) {
		if (hist1[i] + hist2[i] != 0)
			dist += pow(hist1[i] - hist2[i], 2) / (hist1[i] + hist2[i]);
		else
			dist += 0;
	}
	return dist;
}

// Plus la valeur est proche de 1, plus les histogrammes sont similaires
double distanceIntersection(const vector<double>& hist1, const vector<double>& hist2) {
	// Copie des vecteurs pour ne pas les modifier
	vector<double> vecA = hist1;
	vector<double> vecB = hist2;

	vector<double> intersection;
	vector<double> unionSet;

	// Tri des vecteurs pour l'intersection
	sort(vecA.begin(), vecA.end());
	sort(vecB.begin(), vecB.end());

	// Calcul de l'intersection des vecteurs
	set_intersection(vecA.begin(), vecA.end(), vecB.begin(), vecB.end(),
		back_inserter(intersection));

	// Calcul de l'union des vecteurs
	set_union(vecA.begin(), vecA.end(), vecB.begin(), vecB.end(),
		back_inserter(unionSet));

	// Calcul de la distance d'intersection
	double intersection_distance = static_cast<double>(intersection.size()) / unionSet.size();

	return intersection_distance;
}

// Plus la valeur est proche de 0, plus les histogrammes sont similaires
double distanceBhattacharyya(const vector<double>& hist1, const vector<double>& hist2) {
	double hist1_moyenne = moyenne(hist1);
	double hist2_moyenne = moyenne(hist2);
	double score = 0;

	for (int i = 0; i < hist1.size(); i++)
		if (std::find(hist2.begin(), hist2.end(), hist1[i]) != hist2.end())
			score += sqrt(hist1[i] * hist2[i]);

	score = sqrt(1 - (1 / sqrt(hist1_moyenne * hist2_moyenne * hist1.size() * hist2.size())) * score);
	return score;
}

// Plus la valeur est proche de 0, plus les histogrammes sont differents
double distanceCorrelation(const vector<double>& hist1, const vector<double>& hist2) {
	double hist1_moyenne = moyenne(hist1);
	double hist2_moyenne = moyenne(hist2);
	double score = 0;
	for (int i = 0; i < hist1.size(); i++) {
		score += (hist1[i] - hist1_moyenne) * (hist2[i] - hist2_moyenne);
	}

	double tmp = 0;
	for (int i = 0; i < hist1.size(); i++) {
		tmp += pow(hist1[i] - hist1_moyenne, 2) * pow(hist2[i] - hist2_moyenne, 2);
	}

	double correlationLineaire = score / sqrt(tmp);
	return correlationLineaire < 0 ? 0 : correlationLineaire;
}

vector<double> concat(const vector<vector<double>>& hists) {
	vector<double> concatened;
	for (auto& hist : hists)
		concatened.insert(concatened.end(), hist.begin(), hist.end());
	return concatened;
}

vector<Mat> splitIn(const Mat& image, int n) {
	vector<Mat> subImages;
	int width = image.cols / n;
	int height = image.rows / n;
	for (int y = 0; y < image.rows - (height / 2); y += height)
		for (int x = 0; x < image.cols - (width / 2); x += width)
			subImages.push_back(image(cv::Rect(x, y, width, height)));

	return subImages;
}

Mat createImage(const vector<Mat>& imageArray, int gap) {
	int imagesPerRow = sqrt(imageArray.size());
	int imageWidth = imageArray[0].cols;

	// Calculer la largeur et la hauteur de chaque sous-image avec l'espace ajouté
	int subImageWidth = imageWidth + gap;
	int subImageHeight = imageArray[0].rows + gap;

	// Calculer le nombre de lignes nécessaires pour le montage final
	int numRows = (imageArray.size() + imagesPerRow - 1) / imagesPerRow;

	// Créer une image vide pour le montage final
	Mat finalImage(numRows * subImageHeight - gap, imagesPerRow * subImageWidth - gap, CV_8UC3, cv::Scalar(0, 0, 0));

	// Boucle pour fusionner les images
	int imageIndex = 0;
	for (int row = 0; row < numRows; ++row) {
		for (int col = 0; col < imagesPerRow; ++col) {
			// Calculer les coordonnées de la sous-image dans le montage final
			int x = col * subImageWidth;
			int y = row * subImageHeight;

			if (imageIndex < imageArray.size()) {
				// Copier l'image actuelle dans le montage final avec l'espace ajouté
				Mat roi = finalImage(cv::Rect(x, y, imageWidth, imageArray[0].rows));
				imageArray[imageIndex].copyTo(roi);
				imageIndex++;
			}

			// Dessiner l'espace horizontal
			if (col < imagesPerRow - 1) {
				cv::Rect horizontalGapRect(x + imageWidth, y, gap, imageArray[0].rows);
				cv::rectangle(finalImage, horizontalGapRect, cv::Scalar(0, 127, 0), -1);
			}

			// Dessiner l'espace vertical
			if (row < numRows - 1) {
				cv::Rect verticalGapRect(x, y + imageArray[0].rows, subImageWidth, gap);
				cv::rectangle(finalImage, verticalGapRect, cv::Scalar(0, 127, 0), -1);
			}
		}
	}
	return finalImage;
}

int getIndexClasse(const std::vector<bool>& classes) {
	for (int i = 0; i < classes.size(); i++)
		if (classes[i])
			return i;
	return -1;
}

std::string getClasse(int index) {
	vector<std::string> classes =
	{
		"cardboard",								
		"glass",									
		"metal",									
		"paper",									
		"plastic",									
		"trash"										
	};
	return classes[index];
}

Mat detectContours(const cv::Mat& inputImage) {
	cv::Mat edges;
	cv::Canny(inputImage, edges, 60, 80);
	return edges;
}