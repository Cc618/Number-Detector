#pragma once

#include "Matrice.h"


// Ici on défini un seuil car il y a de la compression
// sur certaines images
// Le seuil est la valeur a dépasser pour être pris en tant
// que chiffre
#define SEUIL_SEPARATION_FOND .1F


namespace img
{
	// Sépare les nombre d'une image
	Dataset couperImage(const Matrice &IMAGE);

	/*	Réduit la taille de l'image via un max pooling
	 * La taille fait alors 28x28 px
	 * Peut renvoyer nul si la taille fait moins de 28 pixels de côté
	 * - Renvoie un vecteur ligne de 28 * 28 = 784 pixels
	 */
	Matrice maxPooling(Matrice IMAGE);
};
