#include "maths.h"

#include <algorithm>
#include <math.h>


namespace maths
{
	float sigmoide(float x)
	{
		return 1.F / (1.F + expf(-x));
	}

	float sigmoidePrime(float x)
	{
		const float EXP = expf(-x),
			DIV = (1.F + EXP);

		return EXP / (DIV * DIV);
	}

	float relu(float x)
	{
		return x < 0.F ? 0.F : x;
	}

	float reluPrime(float x)
	{
		return x < 0.F ? 0.F : 1.F;
	}

	float tanh(float x)
	{
		return tanhf(x);
	}

	float tanhPrime(float x)
	{
		const float DIV = coshf(x);

		return 1.F / (DIV * DIV);
	}

	float aleatoire()
	{
		return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}

	float aleatoireMir()
	{
		return aleatoire() * 2.F - 1.F;
	}

	std::vector<size_t> tableauAleatoire(const size_t TAILLE)
	{
		//// Allocation ////
		std::vector<size_t> tab;
		tab.reserve(TAILLE);

		//// Remplissage ////
		for (size_t i = 0U; i < TAILLE; i++)
			tab.emplace_back(i);

		//// Mélange ////
		std::random_shuffle(std::begin(tab), std::end(tab));

		return tab;
	}

	std::vector<size_t> choixAleatoire(const size_t MAX, const size_t TAILLE, const bool FORCER_DIFFERENTS)
	{
		//// Allocation ////
		std::vector<size_t> tab;
		tab.reserve(TAILLE);

		//// Remplissage ////
		if (FORCER_DIFFERENTS)
		{
			for (size_t i = 0U; i < TAILLE; i++)
			{
				size_t valeur = -1;

				// Tant que la valeur est présente on continue
				do
				{
					valeur = rand() % MAX;
				} while (std::find(std::begin(tab), std::end(tab), valeur) != std::end(tab));
			
				// Ajout
				tab.emplace_back(valeur);
			}
		}
		else
			for (size_t i = 0U; i < TAILLE; i++)
				tab.emplace_back(rand() % MAX);

		return tab;
	}
};
