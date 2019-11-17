/*
 *		Ce petit projet permet de créer un executable pour prédire un nombre rapidement.
 */

#include <iostream>

#include "ReseauNeurone.h"
#include "Afficheur.h"
#include "traitementImage.h"
#include "couches.h"
#include "constantes.h"
#include "maths.h"
#include "debug.h"

// Nom du réseau de neurones utilisé
#define NOM_RESEAU "CcNet3"



// Prédiction d'un nombre d'une image
void predireNombre();


// Crée le réseau par son nom
ReseauNeurone creerReseau();



int main()
{
	predireNombre();
	/*
	// Pour éviter les erreurs
	try
	{
		predireNombre();
	}
	catch (MatriceException e)
	{
		std::cerr << e.what() << '\n';
	}
	*/

	return 0;
}



// Vérifie si l'image a bien été prédite
bool estimerChiffre(const Matrice& PRED, const Matrice& y)
{
	return PRED.argmax().second == y.argmax().second;
}



/*
 *		Prédit le nombre que contient une image.
 *
 *	Ordre:
 * - Entrées utilisateur
 * - Chargement du réseau
 * - Chargement de l'image avec traitement
 * - Analyse (découpage) de l'image en chiffres
 * - Traitement des chiffres
 * - Prédiction
 */
void predireNombre()
{
	//// Affichage ////
	std::cout <<
		"--------------------------------\n"
		"------ Bienvenue dans le -------\n"
		"----- Detecteur de Nombres -----\n"
		"--------------------------------\n\n";

	//// Réseau de neuronnes ////
	ReseauNeurone nn = creerReseau();


	//// Chargement /////
	std::cout << "Chargement du reseau...\n";
	if (!nn.charger())
	{
		std::cerr << "Erreur de chargement du reseau\n";

		return;
	}

	std::cout << "Reseau charge\n";


	bool continuer;

	do 
	{
		//// Entrées ////
		// Emplacement du fichier image
		std::string fichier;

		// Interface utilisateur
		std::cout << "\nVeuillez entrer l'emplacement de l'image (ou faites un glisser-deposer)\n> ";

		// Entrée
		std::getline(std::cin, fichier);

		auto d1 = fichier.front(),
			d2 = fichier.back();

		// Quand on fait un glissé-déposé il y a des guillemets, on doit les enlever
		if (fichier.size() > 2U && fichier.front() == '"' && fichier.back() == '"')
			fichier = std::string(fichier.begin() + 1U, fichier.end() - 1U);


		//// Lecture ////
		std::cout << "Lecture de l'image...\n";
		const Matrice IMAGE = io::chargerImage(fichier);

		if (IMAGE.estVide())
		{
			std::cerr << "Erreur de lecture de l'image\n";

			return;
		}


		//// Découpe ////
		// Découpe du nombre en chiffres
		std::cout << "Decoupage de l'image...\n";
		const Dataset ELEMENTS = img::couperImage(IMAGE);
		std::cout << ELEMENTS.size() << " chiffres trouves\n";

		//// Traitement (réduction) ////
		// Elements 
		// On n'utilise pas CHIFFRES car il peut y en avoir moins
		Dataset chiffres;
		chiffres.reserve(ELEMENTS.size());

		// On réduit en matrice 28x28 par du max pooling
		for (size_t n = 0U; n < ELEMENTS.size(); n++)
		{
			// Résultat de la réduction (peut être nul)
			const Matrice CHIFFRE = img::maxPooling(ELEMENTS[n]);

			// Chiffre invalide (trop petit)
			if (CHIFFRE.estVide())
				continue;

			// Si non nul on ajoute le chiffre 
			chiffres.emplace_back(CHIFFRE);
		}


		//// Prédictions ////
		std::cout << "\n-> Le nombre est: ";
		const Dataset PRED = nn.prediction(chiffres);

		for (unsigned int i = 0U; i < chiffres.size(); i++)
		{
			// Espace pour mieux lire
			if ((chiffres.size() - i) % 3U == 0U)
				std::cout << ' ';

			// Prédiction
			std::cout << PRED[i].argmax().second;
		}


		std::cout << "\n\nContinuer (1) ou quitter (0)?\n> ";
		std::cin >> continuer;

		// Pour vérouiller l'entrée
		std::cin.ignore();
	} while (continuer);
}






ReseauNeurone creerReseau()
{
	using namespace maths;

	return ReseauNeurone({
			new CoucheDense(784U, 200U, aleatoireMir),
			new CoucheActivation(sigmoide, sigmoidePrime),

			new CoucheLache(.1F),

			new CoucheDense(200U, 128U, aleatoireMir),
			new CoucheActivation(sigmoide, sigmoidePrime),

			new CoucheLache(.05F),

			new CoucheDense(128U, 10U, aleatoireMir),
			new CoucheActivation(sigmoide, sigmoidePrime),
		},
		.2F,
		NOM_RESEAU,
		"donnees_demo");
}


