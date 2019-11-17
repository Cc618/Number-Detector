#include "Afficheur.h"



void Afficheur::afficher(const Dataset & DONNEES)
{
	std::cout << "--------------------------------\n";

	for (size_t i = 0U; i < DONNEES.size(); i++)
		DONNEES[i].afficher();

	std::cout << "--------------------------------\n";
}
