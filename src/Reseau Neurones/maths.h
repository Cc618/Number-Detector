#pragma once

/*
 *		Toutes les fonctions d'axctivation, de distribution...
 */

#include <vector>



namespace maths
{
	// Les activations sont groupées par paires (fonction, activation)
	//// Activations ////
	// Fonction sigmoide
	float sigmoide(float x);
	float sigmoidePrime(float x);

	// Rectified Linear Unit
	float relu(float x);
	float reluPrime(float x);
	
	// Fonction tangente hyperbolique
	float tanh(float x);
	float tanhPrime(float x);


	// Avec SOMME_EXP float : Somme de tous les composants = e ^ z_k avec z_k le composant de la matrice
	template <typename SOMME_EXP>
	float softmax(float x)
	{
		return expf(x) * (1.F / SOMME_EXP);
	}
	// Avec SOMME_EXP float : (voir softmax)
	template <typename SOMME_EXP>
	float softmaxPrime(float x)
	{
		return 1.F / SOMME_EXP;
	}


	//// Distributions ////
	// Aléatoire pure [0, 1]
	float aleatoire();
	// [-1, 1]
	float aleatoireMir();


	//// Divers ////
	// Donne un tableau de size_t mélangés de 0 à TAILLE - 1
	std::vector<size_t> tableauAleatoire(const size_t TAILLE);

	// Crée un tableau de size_t aléatoires
	// - FORCER_DIFFERENTS : Aucun nombre ne sera pareil
	std::vector<size_t> choixAleatoire(const size_t MAX, const size_t TAILLE, const bool FORCER_DIFFERENTS = false);
};

