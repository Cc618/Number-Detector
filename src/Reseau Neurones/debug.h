#pragma once

/*
 *		Pour débugger.
 */

#ifdef _DEBUG

#include <iostream>

#include "Matrice.h"



// Affiche un chiffre en 0 et 1
// Le chiffre doit être une matrice de 1 x 784
inline void afficherChiffre(const Matrice &CHIFFRE)
{
	for (size_t y = 0U; y < 28U; y++)
	{
		for (size_t x = 0U; x < 28U; x++)
			std::cout << (CHIFFRE.en(0U, y * 28 + x) > .5F ? 1U : 0U);

		std::cout << '\n';
	}
}

// afficherChiffre pour les matrices 2D
inline void afficherMatrice(const Matrice &CHIFFRE)
{
	for (size_t y = 0U; y < CHIFFRE.getLignes(); y++)
	{
		for (size_t x = 0U; x < CHIFFRE.getColones(); x++)
			std::cout << (CHIFFRE.en(y, x) > .5F ? 1U : 0U);

		std::cout << '\n';
	}
}

#endif // _DEBUG
