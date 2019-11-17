#pragma once

/*
 *		L'afficheur permet de gérer toutes les sorties dans la console.
 */

#include <iostream>
#include <string>
#include <stdarg.h>

#include "Matrice.h"


struct Afficheur
{
public:
	static void afficher(const Dataset &DONNEES);


public:
	// Comme un printf
	inline void affiche(const char *MSG, ...) const
	{
		// Arguments
		va_list args;
		va_start(args, MSG);

		// Affichage
		vfprintf(stdout, MSG, args);

		va_end(args);
	}

	inline void erreur(const std::string &MSG) const
	{
		std::cerr << "Erreur: " << MSG << '\n';
	}

	inline void debug(const std::string &MSG) const
	{
#ifdef _DEBUG
		std::cout << "Debug: " << MSG << '\n';
#endif
	}

public:
	// Nombre d'éléments sans messages
	// (si epoques == 10U alors toutes les 10 époques on laisse une trace)
	size_t epoques = -1;

	bool afficherChargement = false,
		afficherEnregistrement = false,
		afficherEntrainement = false,
		afficherLot = false;
};

