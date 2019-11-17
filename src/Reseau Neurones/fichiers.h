#pragma once

/*
 *		Pour enregistrer / lire les fichiers de données.
 */

#include <string>
#include <vector>

#include "Matrice.h"


namespace io
{
	//	Charge le dataset des labels
	// - retour : Tableau dynamique de vecteur colone (sous forme de matrice, one hot encoded vector)
	std::vector<Matrice> chargerLabels(const std::string &EMPLACEMENT);

	//	Charge le dataset des images
	// - UNI_DIMENSIONELLES : Si true les images sont chargées dans des vecteurs ligne
	// - retour : Tableau dynamique de matrice normalisées (0.F = pixel noir, 1.F = pixel blanc)
	std::vector<Matrice> chargerImages(const std::string &EMPLACEMENT, const bool UNI_DIMENSIONELLES = false);

	// Enregistre la matrice dans un fichier
	bool enregistrerMatrice(const Matrice &MAT, const std::string &EMPLACEMENT);

	// Charge la matrice
	Matrice chargerMatrice(const std::string &EMPLACEMENT);

	//	Lit l'image dans une matrice
	// Renvoie seulement la canal rouge normalisé (le premier canal).
	// Ne supporte pas l'alpha.
	Matrice chargerImage(const std::string &EMPLACEMENT);
};

