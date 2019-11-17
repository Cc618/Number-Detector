#include "traitementImage.h"

#include <Windows.h>

#include "debug.h"


// Taille d'un chiffre (en px)
#define CHIFFRE_TAILLE 28U
// Vide pour la réduction à laisser pour mieux prédire (de chaque côtés, en px)
#define CHIFFRE_ESPACE 2U
// Taille de la zone occupée par le chiffre
#define CHIFFRE_TAILLE_ZONE (CHIFFRE_TAILLE - CHIFFRE_ESPACE * 2U)



namespace img
{
	// Ajoute une bordure (un espace) de 0.F
	Matrice bordure(const Matrice& IMAGE, const size_t GAUCHE = 0U, const size_t HAUT = 0U, const size_t DROITE = 0U, const size_t BAS = 0U)
	{
		// Allocation //
		const size_t LIGNES = IMAGE.getLignes(),
			COLONES = IMAGE.getColones(),
			HAUTEUR = HAUT + LIGNES + BAS,
			LARGEUR = GAUCHE + COLONES + DROITE;

		float* donnees = new float[HAUTEUR * LARGEUR];

		// Remplissage de 0
		ZeroMemory(donnees, HAUTEUR * LARGEUR * sizeof(float));

		// Image //
		for (size_t y = 0U; y < LIGNES; y++)
			for (size_t x = 0U; x < COLONES; x++)
				donnees[(HAUT + y) * LARGEUR + x + GAUCHE] = IMAGE.en(y, x);

		return Matrice(donnees, HAUTEUR, LARGEUR);
	}


	// Ajoute une bordure de 0 autours de l'image pour avoir les mêmes proportions
	// On ajoute aussi de l'espacement
	Matrice conserverProportions(const Matrice & IMAGE, const size_t ESPACEMENT = 0U)
	{
		// On veut conserver les proportions donc on remplit par une bordure de 0
		// Trop étiré en vertical
		if (IMAGE.getLignes() > IMAGE.getColones())
		{
			// Calculs des surplus
			const size_t ESPACE = IMAGE.getLignes() - IMAGE.getColones(),
				ESPACE_GAUCHE = ESPACE / 2U;

			// Si c'est pair (divisible par 2)
			if (ESPACE % 2U == 0U)
				return bordure(IMAGE, ESPACE_GAUCHE + ESPACEMENT, ESPACEMENT, ESPACE_GAUCHE + ESPACEMENT, ESPACEMENT);

			// C'est impair l'image est un peu décallée à droite
			else
				return bordure(IMAGE, ESPACE_GAUCHE + ESPACEMENT + 1U, ESPACEMENT, ESPACE_GAUCHE + ESPACEMENT, ESPACEMENT);
		}
		// Trop étiré à l'horizontale
		else if (IMAGE.getLignes() < IMAGE.getColones())
		{
			// Calculs des surplus
			const size_t ESPACE = IMAGE.getColones() - IMAGE.getLignes(),
				ESPACE_HAUT = ESPACE / 2U;

			// Si c'est pair (divisible par 2)
			if (ESPACE % 2U == 0U)
				return bordure(IMAGE, ESPACEMENT, ESPACE_HAUT + ESPACEMENT, ESPACEMENT, ESPACE_HAUT + ESPACEMENT);

			// C'est impair l'image est un peu décallée vers le bas
			else
				return bordure(IMAGE, ESPACEMENT, ESPACE_HAUT + ESPACEMENT + 1U, ESPACEMENT, ESPACE_HAUT + ESPACEMENT);
		}

		return IMAGE;
	}



	// Coupe une image à l'horizontale
	Matrice couperImageH(const Matrice &IMAGE)
	{
		// Extrémités
		size_t haut = -1,
			bas = -1;

		// Recherche du haut
		for (size_t l = 0U; l < IMAGE.getLignes(); l++)
			for (size_t c = 0U; c < IMAGE.getColones(); c++)
				// Haut trouvé
				if (IMAGE.en(l, c) > SEUIL_SEPARATION_FOND)
				{
					haut = l;

					goto couperImageH_hautTrouve;
				}

		// Label atteint quand on trouve le haut
	couperImageH_hautTrouve:

		// Recherche du bas (on peut sauter la première ligne)
		for (size_t l = IMAGE.getLignes() - 1U; l > 0U; l--)
			for (size_t c = 0U; c < IMAGE.getColones(); c++)
				// Bas trouvé
				if (IMAGE.en(l, c) > SEUIL_SEPARATION_FOND)
				{
					bas = l - 1U;

					goto couperImageH_basTrouve;
				}

		// Label atteint quand on trouve le bas
	couperImageH_basTrouve:
		// Découpage en matrice //
		// Erreur
		if (bas == -1 || haut == -1)
			return Matrice();

		// Tailles
		const size_t LIGNES = bas - haut + 1U,
			COLS = IMAGE.getColones();

		float *donnees = new float[LIGNES  * COLS];
		for (size_t l = 0U; l < LIGNES; l++)
			for (size_t c = 0U; c < COLS; c++)
				donnees[l * COLS + c] = IMAGE.en(haut + l, c);

		return Matrice(donnees, LIGNES, IMAGE.getColones());
	}


	/*
	 *		Algorithme:
	 * - On cherche le pixel du premier nombre (verticalement)
	 * - On continue jusqu'à la fin du nombre
	 * - On enregistre les extremités du nombre
	 * - Pour chaque nombre on trouve le haut et le bas
	 * - On enregistre l'AABB
	 * - Retour
	 */
	Dataset couperImage(const Matrice & IMAGE)
	{
		const size_t COLONES = IMAGE.getColones(),
			LIGNES = IMAGE.getLignes();

		Dataset nombres;

		// Points de début ou fin de chiffre (juste la colone)
		std::vector<size_t> separations;

		// Détection des séparations //
		bool coloneContenaitNombre = false;
		for (size_t c = 0U; c < COLONES; c++)
		{
			bool coloneContientNombre = false;

			for (size_t l = 0U; l < LIGNES; l++)
				if (IMAGE.en(l, c) > SEUIL_SEPARATION_FOND)
				{
					// S'il n'y avait pas déjà un nombre on ajoute la séparation
					if (!coloneContenaitNombre)
					{
						separations.push_back(c + 1U);

						coloneContenaitNombre = true;
					}

					coloneContientNombre = true;

					break;
				}
			
			// Pas de nombre trouvé
			if (!coloneContientNombre)
			{
				// On sort d'un nombre, ajout de la séparation
				if (coloneContenaitNombre)
					separations.push_back(c);

				coloneContenaitNombre = false;
			}
		}

		// Test si le nombre touche le bord droit
		if (coloneContenaitNombre)
			separations.push_back(COLONES - 1U);

		// Découpage en matrices //
		// Pour chaque paire de séparation (separation.size() paire)
		for (size_t i = 0U; i < separations.size(); i += 2U)
		{
			// Coupe verticale //
			// Largeur de coupe
			const size_t LARGEUR = separations[i + 1U] - separations[i];

			// Récupération des données
			float *donnees = new float[LIGNES * LARGEUR];
			for (size_t l = 0U; l < LIGNES; l++)
				for (size_t c = 0U; c < LARGEUR; c++)
					donnees[l * LARGEUR + c] = IMAGE.en(l, separations[i] + c);

			nombres.push_back(conserverProportions(couperImageH(Matrice(donnees, LIGNES, LARGEUR))));
			//nombres.push_back(couperImageH(Matrice(donnees, LIGNES, LARGEUR)));
		}

		return nombres;
	}


	/*
	 *	Algorithme:
	 * - On découpe en zones l'image
	 * - On ajoute la valeur du pixel le plus clair de la zone précédement découpée
	 */
	Matrice maxPooling(Matrice image)
	{
		//// Vérification ////
		const size_t LIGNES = image.getLignes(),
			COLONES = image.getColones();

		// L'image est trop petite, l'algorithme ici
		// ne fait que de réduire pas d'agrandir
		if (LIGNES < CHIFFRE_TAILLE)
			return Matrice();


		// On agrandit virtuellement si besoin les côtés de l'image pour qu'elle conserve 
		// son ratio d'aspect (qu'elle ne soit pas étirée)
		if (COLONES < LIGNES)
		{
			Matrice nouvelleImage(LIGNES, LIGNES);

			// Espace à ajouter
			const size_t ESPACE = LIGNES - COLONES,
				MI_ESPACE = ESPACE / 2U;

			// True si l'espace est divisible par 2
			// Si on ne fait pas ça on peut avoir des nombres
			// qui ont fuit dans une colones (la mémoire est juste reservée)
			const bool ESPACE_PAIRE = ESPACE % 2U == 0U;

			for (size_t y = 0U; y < LIGNES; y++)
			{
				// Espace à gauche
				for (size_t x = 0U; x < MI_ESPACE; x++)
					nouvelleImage.en(y, x) = 0.F;

				if (ESPACE_PAIRE)
				{
					// Partie remplie par le chiffre
					for (size_t x = MI_ESPACE; x < LIGNES - MI_ESPACE; x++)
						nouvelleImage.en(y, x) = image.en(y, x - MI_ESPACE);

					// Espace à droite
					for (size_t x = LIGNES - MI_ESPACE; x < LIGNES; x++)
						nouvelleImage.en(y, x) = 0.F;
				}
				else
				{
					// Partie remplie par le chiffre
					for (size_t x = MI_ESPACE; x < LIGNES - MI_ESPACE - 1U; x++)
						nouvelleImage.en(y, x) = image.en(y, x - MI_ESPACE);

					// Espace à droite
					for (size_t x = LIGNES - MI_ESPACE - 1U; x < LIGNES; x++)
						nouvelleImage.en(y, x) = 0.F;
				}
			}

			image = nouvelleImage;
		}




		//std::cout << 



	

		// Allocation des données //
		float *donnees = new float[CHIFFRE_TAILLE * CHIFFRE_TAILLE];
		
		//memset(donnees, 0, 784U * 4U);

		size_t y = 0U,
			x;


		// Pour chaque zone
		for (y = 0U; y < CHIFFRE_TAILLE - CHIFFRE_ESPACE * 2U; y++)
		{
			for (x = 0U; x < CHIFFRE_TAILLE - CHIFFRE_ESPACE * 2U; x++)
			{
				// Valeur max du pixel
				float max = 0.F;

				// Fins de la zone
				size_t maxY = ((y + 1U) * LIGNES) / CHIFFRE_TAILLE_ZONE,
					maxX = ((x + 1U) * COLONES) / CHIFFRE_TAILLE_ZONE;

				if (maxY > LIGNES)
					maxY = LIGNES;

				if (maxX > COLONES)
					maxX = COLONES;

				// Pour chaque pixel de la zone
				for (size_t pxY = (y * LIGNES) / CHIFFRE_TAILLE_ZONE; pxY < maxY; pxY++)
					for (size_t pxX = (x * COLONES) / CHIFFRE_TAILLE_ZONE; pxX < maxX; pxX++)
					{
						const float PX = image.en(pxY, pxX);

						// Changement du maximum
						if (PX > max)
							max = PX;
					}

				donnees[(y + CHIFFRE_ESPACE) * CHIFFRE_TAILLE + (x + CHIFFRE_ESPACE)] = max;
			}
		}

		
		// Remplissage de l'espace vide //
		// Gauche
		for (size_t l = 0U; l < CHIFFRE_TAILLE; l++)
			for (size_t c = 0U; c < CHIFFRE_ESPACE; c++)
				donnees[l * CHIFFRE_TAILLE + c] = 0.F;

		// Droite
		for (size_t l = 0U; l < CHIFFRE_TAILLE; l++)
			for (size_t c = CHIFFRE_TAILLE - CHIFFRE_ESPACE; c < CHIFFRE_TAILLE; c++)
				donnees[l * CHIFFRE_TAILLE + c] = 0.F;

		// Haut
		for (size_t l = 0U; l < CHIFFRE_ESPACE; l++)
			for (size_t c = CHIFFRE_ESPACE; c < CHIFFRE_TAILLE - CHIFFRE_ESPACE; c++)
				donnees[l * CHIFFRE_TAILLE + c] = 0.F;

		// Bas
		for (size_t l = CHIFFRE_TAILLE - CHIFFRE_ESPACE; l < CHIFFRE_TAILLE; l++)
			for (size_t c = CHIFFRE_ESPACE; c < CHIFFRE_TAILLE - CHIFFRE_ESPACE; c++)
				donnees[l * CHIFFRE_TAILLE + c] = 0.F;


		// On renvoie un vecteur ligne
		return Matrice(donnees, 1U, CHIFFRE_TAILLE * CHIFFRE_TAILLE);
	}
};

