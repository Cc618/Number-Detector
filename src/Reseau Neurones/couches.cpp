#include "couches.h"

#include <iostream>

#include "maths.h"



//////////////// CoucheActivation ////////////////
Matrice CoucheActivation::propagationAvant(const Matrice & x) const
{
	return x.map(fonction);
}

Matrice CoucheActivation::delta(const Matrice & x, const Matrice & DELTA_PRECEDENT) const
{
	return DELTA_PRECEDENT * x.map(derivee);
}

//////////////// CoucheDense ////////////////
Matrice CoucheDense::propagationAvant(const Matrice & x) const
{
	return x.prod(_poids) + _biais;
}

void CoucheDense::propagationArriere(const Matrice & x, const Matrice & DELTA)
{
	_biais -= DELTA;
	_poids -= x.transpose().prod(DELTA);
}

Matrice CoucheDense::delta(const Matrice & x, const Matrice & DELTA_PRECEDENT) const
{
	return DELTA_PRECEDENT.prod(_poids.transpose());
}

bool CoucheDense::charger(const bool FORCER_CHARGEMENT)
{
	if (!FORCER_CHARGEMENT)
	{
		_poids = Matrice::charger(initialisation->emplacement + ".poids.mat");
		_biais = Matrice::charger(initialisation->emplacement + ".biais.mat");
	}

	// Non chargé
	if (_poids.estVide() || _poids.estVide())
	{
		// Conversion
		const CoucheDense::initialiseur *const init = static_cast<const CoucheDense::initialiseur *const>(initialisation);

		_poids = Matrice::avecFonction(
			init->distribution,
			init->tailleX,
			init->tailleY);

		_biais = Matrice::avecFonction(init->distribution, 1U, init->tailleY);

		return false;
	}

	// Chargé
	return true;
}

void CoucheDense::enregistrer() const
{
	io::enregistrerMatrice(_poids, initialisation->emplacement + ".poids.mat");
	io::enregistrerMatrice(_biais, initialisation->emplacement + ".biais.mat");
}

//////////////// CoucheLache ////////////////
Matrice CoucheLache::delta(const Matrice& ENTREE, const Matrice& DELTA_PRECEDENT) const
{
	Matrice sortie = DELTA_PRECEDENT;

	for (size_t y = 0U; y < DELTA_PRECEDENT.getLignes(); y++)
		for (size_t x = 0U; x < DELTA_PRECEDENT.getColones(); x++)
			// On "grille" le poids
			if (maths::aleatoire() <= COEFFICIENT)
				sortie.en(y, x) = 0.F;

	return sortie;
}

//////////////// CoucheConvolution ////////////////
Matrice CoucheConvolution::propagationAvant(const Matrice& ENTREE) const
{
	//// Allocation ////
	const size_t LIGNES = ENTREE.getLignes(),
		COLONES = ENTREE.getColones(),
		LARGEUR_FILTRE = FILTRE.getColones(),
		HAUTEUR_FILTRE = FILTRE.getLignes();

	Matrice sortie(LIGNES, COLONES);


	//// Traitement ////
	// Pour chaque pixel de l'entrée
	for (size_t y = 0U; y <= LIGNES - HAUTEUR_FILTRE; y++)
		for (size_t x = 0U; x <= COLONES - LARGEUR_FILTRE; x++)
		{
			// Somme des pixels de la zone
			float somme = 0.F;

			// Pour chaque pixel du filtre
			for (size_t fy = 0U; fy < HAUTEUR_FILTRE; fy++)
				for (size_t fx = 0U; fx < LARGEUR_FILTRE; fx++)
					somme += FILTRE.en(fy, fx) * ENTREE.en(y + fy, x + fx);

			sortie.en(y, x) = somme;
		}


	//// Espace ////
	// On remplie de 0.F
	// Bas
	for (size_t y = LIGNES - HAUTEUR_FILTRE + 1U; y < LIGNES; y++)
		for (size_t x = 0U; x < COLONES; x++)
			sortie.en(y, x) = 0.F;

	// Droite
	for (size_t y = 0U; y < LIGNES - HAUTEUR_FILTRE + 1U; y++)
		for (size_t x = COLONES - LARGEUR_FILTRE + 1U; x < COLONES; x++)
			sortie.en(y, x) = 0.F;

	return sortie;
}

Matrice CoucheConvolution::delta(const Matrice& x, const Matrice& DELTA_PRECEDENT) const
{
	return Matrice();
}

//////////////// CoucheMaxPooling ////////////////
Matrice CoucheMaxPooling::propagationAvant(const Matrice& ENTREE) const
{
	//// Allocation ////
	const size_t LIGNES = ENTREE.getLignes(),
		COLONES = ENTREE.getColones(),
		HAUTEUR = LIGNES / DIM_Y,
		LARGEUR = COLONES / DIM_X;

	Matrice sortie(HAUTEUR, LARGEUR);


	//// Traitement ////
	// Pour chaque pixel de l'entrée
	for (size_t y = 0U; y < HAUTEUR; y++)
		for (size_t x = 0U; x < LARGEUR; x++)
		{
			// Changement de base
			const size_t ENTREE_X = x * DIM_X,
				ENTREE_Y = y * DIM_Y;

			// Pixel le plus lumineux
			float max = ENTREE.en(ENTREE_Y, ENTREE_X);

			// Pour chaque pixel de la zone
			for (size_t zy = 0U; zy < DIM_Y; zy++)
				for (size_t zx = 0U; zx < DIM_X; zx++)
				{
					const float PX = ENTREE.en(ENTREE_Y + zy, ENTREE_X + zx);

					// Changement du max
					if (PX > max)
						max = PX;
				}

			sortie.en(y, x) = max;
		}


	return sortie;
}

Matrice CoucheMaxPooling::delta(const Matrice& x, const Matrice& DELTA_PRECEDENT) const
{
	return Matrice();
}
