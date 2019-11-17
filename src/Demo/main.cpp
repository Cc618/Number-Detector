/*
 *		Ce petit projet permet de cr�er un executable pour pr�dire un nombre rapidement.
 */

#include <iostream>

#include "ReseauNeurone.h"
#include "Afficheur.h"
#include "traitementImage.h"
#include "couches.h"
#include "constantes.h"
#include "maths.h"
#include "debug.h"

// Nom du r�seau de neurones utilis�
#define NOM_RESEAU "CcNet3"



// Pr�diction d'un nombre d'une image
void predireNombre();


// Cr�e le r�seau par son nom
ReseauNeurone creerReseau();



int main()
{
	predireNombre();
	/*
	// Pour �viter les erreurs
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



// V�rifie si l'image a bien �t� pr�dite
bool estimerChiffre(const Matrice& PRED, const Matrice& y)
{
	return PRED.argmax().second == y.argmax().second;
}



/*
 *		Pr�dit le nombre que contient une image.
 *
 *	Ordre:
 * - Entr�es utilisateur
 * - Chargement du r�seau
 * - Chargement de l'image avec traitement
 * - Analyse (d�coupage) de l'image en chiffres
 * - Traitement des chiffres
 * - Pr�diction
 */
void predireNombre()
{
	//// Affichage ////
	std::cout <<
		"--------------------------------\n"
		"------ Bienvenue dans le -------\n"
		"----- Detecteur de Nombres -----\n"
		"--------------------------------\n\n";

	//// R�seau de neuronnes ////
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
		//// Entr�es ////
		// Emplacement du fichier image
		std::string fichier;

		// Interface utilisateur
		std::cout << "\nVeuillez entrer l'emplacement de l'image (ou faites un glisser-deposer)\n> ";

		// Entr�e
		std::getline(std::cin, fichier);

		auto d1 = fichier.front(),
			d2 = fichier.back();

		// Quand on fait un gliss�-d�pos� il y a des guillemets, on doit les enlever
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


		//// D�coupe ////
		// D�coupe du nombre en chiffres
		std::cout << "Decoupage de l'image...\n";
		const Dataset ELEMENTS = img::couperImage(IMAGE);
		std::cout << ELEMENTS.size() << " chiffres trouves\n";

		//// Traitement (r�duction) ////
		// Elements 
		// On n'utilise pas CHIFFRES car il peut y en avoir moins
		Dataset chiffres;
		chiffres.reserve(ELEMENTS.size());

		// On r�duit en matrice 28x28 par du max pooling
		for (size_t n = 0U; n < ELEMENTS.size(); n++)
		{
			// R�sultat de la r�duction (peut �tre nul)
			const Matrice CHIFFRE = img::maxPooling(ELEMENTS[n]);

			// Chiffre invalide (trop petit)
			if (CHIFFRE.estVide())
				continue;

			// Si non nul on ajoute le chiffre 
			chiffres.emplace_back(CHIFFRE);
		}


		//// Pr�dictions ////
		std::cout << "\n-> Le nombre est: ";
		const Dataset PRED = nn.prediction(chiffres);

		for (unsigned int i = 0U; i < chiffres.size(); i++)
		{
			// Espace pour mieux lire
			if ((chiffres.size() - i) % 3U == 0U)
				std::cout << ' ';

			// Pr�diction
			std::cout << PRED[i].argmax().second;
		}


		std::cout << "\n\nContinuer (1) ou quitter (0)?\n> ";
		std::cin >> continuer;

		// Pour v�rouiller l'entr�e
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


