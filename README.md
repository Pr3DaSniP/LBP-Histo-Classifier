# LBP-Histo-Classifier

![Language](https://img.shields.io/badge/Language-C++-blue.svg)
![Librairies](https://img.shields.io/badge/Librairies-OpenCV-green.svg)
![IDE](https://img.shields.io/badge/IDE-Visual%20Studio%202022-red.svg)

## Description

Ce projet est un programme de classification d'images basé sur l'histogramme de LBP (Local Binary Pattern). Il permet de classer des images en fonction de leur histogramme de LBP.

Vous trouverez dans ce dépôt deux datasets d'images différents. Le premier est un dataset de 200 images de 2 classes différentes (100 images par classe). Le second est un dataset de 2000 images de 6 classes différentes.

## Comment ça marche ?

Le programme va tout d'abord split les channels de l'image en 3 images différentes (une pour chaque channel) et les concaténer en une seule image. Ensuite, il va utiliser le descripteur LBP pour calculer sa texture. Enfin, il va calculer l'histogramme de cette texture et le normaliser.

Le programme va ensuite comparer l'histogramme de l'image à classifier avec les histogrammes de toutes les images du dataset en calculant la distance (euclidienne, chi2, intersection, correlation, bhattacharyya) entre les deux histogrammes. Il va ensuite classer l'image en fonction de la distance la plus faible.