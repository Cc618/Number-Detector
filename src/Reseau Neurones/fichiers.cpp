#include "fichiers.h"

#include <iostream>
#include <fstream>
#include <fstream>
#include <vector>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "traitementImage.h"


namespace io
{
	// Lecture d'un fichier binaire
	std::vector<unsigned char> lire(const std::string &EMPLACEMENT, const bool DEBUG = false)
	{
		std::ifstream fichier;
		
		fichier.open(EMPLACEMENT, std::ios::binary);

		if (!fichier.is_open())
		{
			if (DEBUG)
				std::cerr << "Erreur lors de l'ouverture du fichier " << EMPLACEMENT << '\n';

			return {};
		}

		std::vector<unsigned char> contenu(std::istreambuf_iterator<char>(fichier), {});
		
		fichier.close();

		return contenu;
	}

	// Ecriture d'un fichier binaire
	bool ecrire(const std::string &EMPLACEMENT, const std::vector<char> &DONNEES)
	{
		std::ofstream fichier;

		// Ouverture
		fichier.open(EMPLACEMENT, std::ios::binary);

		// Vérification
		if (!fichier.is_open())
			return false;

		// Ecriture
		fichier.write(&DONNEES[0U], DONNEES.size());

		// Fermeture
		fichier.close();

		return true;
	}

	// Lit le nombre (uint32_t) à l'indice i dans DONNEES en big endian
	inline unsigned int lireUIntBig(const std::vector<unsigned char> &DONNEES, const unsigned int i)
	{
		// Conversion little-endian - big-endian
		return _byteswap_ulong(
			*reinterpret_cast<unsigned int const*>(&DONNEES[i])
		);
	}

	// Lit le nombre (uint32_t) à l'indice i dans DONNEES en little endian
	inline unsigned int lireUInt(const std::vector<unsigned char> &DONNEES, const unsigned int i)
	{
		return *reinterpret_cast<unsigned int const*>(&DONNEES[i]);
	}

	// Ecrit une valeur uint32_t dans le vecteur
	inline void ecrireUInt(std::vector<char> &donnees, const unsigned int i, const unsigned int VALEUR)
	{
		*reinterpret_cast<unsigned int*>(&donnees[i]) = VALEUR;
	}

	// Ecrit une valeur float32_t dans le vecteur
	inline void ecrireFloat(std::vector<char> &donnees, const unsigned int i, const float VALEUR)
	{
		*reinterpret_cast<float*>(&donnees[i]) = VALEUR;
	}

	std::vector<Matrice> chargerLabels(const std::string &EMPLACEMENT)
	{
		// Données brutes du fichier
		const std::vector<unsigned char> DONNEES = lire(EMPLACEMENT);

		// Erreur lors de l'ouverture du fichier ou fichier vide
		if (DONNEES.size() < 8)
			return {};

		// Nombre d'éléments
		const unsigned int NOMBRE = lireUIntBig(DONNEES, 4U);

		// Création du tableau dynamique
		std::vector<Matrice> labels;
		labels.reserve(NOMBRE);

		for (size_t i = 0U; i < NOMBRE; i++)
			labels.emplace_back(Matrice::oneHot((unsigned int)DONNEES[8U + i], 10U));

		return labels;
	}

	std::vector<Matrice> chargerImages(const std::string & EMPLACEMENT, const bool UNI_DIMENSIONELLES)
	{
		// Données brutes du fichier
		const std::vector<unsigned char> DONNEES = lire(EMPLACEMENT);

		// Erreur lors de l'ouverture du fichier ou fichier vide
		if (DONNEES.size() < 8)
			return {};

		// Nombre d'éléments
		const unsigned int NOMBRE = lireUIntBig(DONNEES, 4U),
			// Hauteur d'une image
			LIGNES = lireUIntBig(DONNEES, 8U),
			// Largeur d'une image
			COLONES = lireUIntBig(DONNEES, 12U);

		// Création du tableau dynamique
		std::vector<Matrice> images;
		images.reserve(NOMBRE);

		// Pour chaque image
		for (size_t i = 0U; i < NOMBRE; i++)
		{
			// Allocation des données
			float *donnees = new float[LIGNES * COLONES];

			// Conversion de unsigned char en float
			// Pour chaque pixel
			for (unsigned int px = 0U; px < LIGNES * COLONES; px++)
				donnees[px] = DONNEES[16U + i * LIGNES * COLONES + px] >
					static_cast<unsigned char>(SEUIL_SEPARATION_FOND * 256.F) ? 1.F : 0.F;

			if (UNI_DIMENSIONELLES)
				images.emplace_back(Matrice(donnees, 1U, LIGNES * COLONES));
			else
				images.emplace_back(Matrice(donnees, LIGNES, COLONES));
		}

		return images;
	}

	bool enregistrerMatrice(const Matrice &MAT, const std::string & EMPLACEMENT)
	{
		// Nombre de composants
		const size_t TAILLE = MAT.getLignes() * MAT.getColones();

		std::vector<char> donnees(8U + TAILLE * sizeof(float));

		// Header (dimensions)
		ecrireUInt(donnees, 0U, MAT.getLignes());
		ecrireUInt(donnees, 4U, MAT.getColones());

		// Données des pixels
		for (size_t i = 0U; i < TAILLE; i++)
			ecrireFloat(donnees, 8U + i * sizeof(float), MAT.getDonnees()[i]);

		return ecrire(EMPLACEMENT, donnees);
	}

	Matrice chargerMatrice(const std::string & EMPLACEMENT)
	{
		//// Lecture ////
		const std::vector<unsigned char> DONNEES = lire(EMPLACEMENT);

		//// Vérification ////
		// Erreur lors de la lecture ou fichier vide
		if (DONNEES.size() < 8U)
			return Matrice();

		//// Header ////
		const unsigned int LIGNES = lireUInt(DONNEES, 0U),
			COLONES = lireUInt(DONNEES, 4U);

		//// Composants ////
		const size_t TAILLE = LIGNES * COLONES;

		// Allocation
		float *donneesMat = new float[TAILLE];

		// Copie
		std::memcpy(donneesMat, &DONNEES[8U], TAILLE * sizeof(float));

		// Création de la matrice
		return Matrice(donneesMat, LIGNES, COLONES);
	}

	Matrice chargerImage(const std::string & EMPLACEMENT)
	{
		// Chargement de l'image
		int largeur,
			hauteur,
			couches;
		unsigned char* image = stbi_load(EMPLACEMENT.c_str(), &largeur, &hauteur, &couches, STBI_rgb);

		// Erreur de lecture de l'image
		if (image == nullptr)
			return Matrice();

		// Allocation des données
		float *donnees = new float[hauteur * largeur];

		// Ajout des données avec normalisation
		for (int i = 0; i < largeur * hauteur; i++)
			donnees[i] = static_cast<float>(image[i * 3]) * (1.F / 255.F);

		return Matrice(donnees, hauteur, largeur);
	}
};

