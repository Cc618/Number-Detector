#include <iostream>

#include "ReseauNeurone.h"
#include "Afficheur.h"
#include "traitementImage.h"
#include "couches.h"
#include "constantes.h"
#include "maths.h"
#include "debug.h"

// Nom du réseau de neurones utilisé
#define NOM_RESEAU "CcNet2"



// Lecture des entrées de l'utilisateur
void interactionUtilisateur(),

	// Prédiction d'un nombre d'une image
	predireNombre(),

	// Prédiction d'un chiffre d'une image 28 * 28 px
	predireChiffre(),

	// Entraînement du réseau de neurones
	entrainer();

// Crée le réseau par son nom
ReseauNeurone creerReseau();


#ifdef _DEBUG
extern int mainDebug();
#endif



int main()
{
	//return mainDebug();


	interactionUtilisateur();

	/*
	// Pour éviter les erreurs
	try
	{
		interactionUtilisateur();
	}
	catch (MatriceException e)
	{
		std::cerr << e.what() << '\n';
	}
	*/

	return 0;
}



void interactionUtilisateur()
{
	// Interface utilisateur
	int mode;
	
	std::cout << "Que voulez-vous ?\n"
		"- (1) De l'aide\n"
		"- (2) Predire un nombre\n"
		"- (3) Predire un chiffre\n"
		"- (4) Entrainer le reseau\n"
		"\n> ";
	std::cin >> mode;


	switch (mode)
	{
	case 1:				// Aide
	{
		std::cout <<
			"\t- Predire un nombre\n"
			"Predit le nombre dessine sur une image\n"

			"\t- Predire un chiffre\n"
			"Predit le chiffre dessine sur une image de 28 * 28 px\n"
			
			"\t- Entrainement:\n"
			"Permet d'entrainer le reseau sur la base de donnees MNIST.\n";
	}
	break;

	case 2:
	{
		predireNombre();
	}
	break;

	case 3:
	{
		predireChiffre();
	}
	break;

	case 4:
	{
		entrainer();
	}
	break;


	// Il y a erreur
	default:
	{
		std::cerr << "Veuillez entrer un bon mode.\n\n";
		system("pause");
	}
	return;
	}


	bool continuer;

	std::cout << "\n\nQuitter (0) ou continuer (1) ?\n> ";

	// On attend
	std::cin >> continuer;

	// On recommence par réccurence
	if (continuer)
		interactionUtilisateur();
}


// Vérifie si l'image a bien été prédite
bool estimerChiffre(const Matrice &PRED, const Matrice &y)
{
	return PRED.argmax().second == y.argmax().second;
}



/*
 *		La phase d'entraînement se fait sur les échantillons train,
 * celle d'efficacité sur test.
 */
void entrainer()
{
	//	Si on est en debug l'entraînement est beaucoup plus long,
	// on demande confirmation
#ifdef _DEBUG
	std::cout << "Attention! Vous etes en mode debug, voulez-vous quitter? (0 / 1)\n> ";

	bool quitter;
	std::cin >> quitter;

	if (quitter)
		return;
#endif

	//// Paramètres ////
	bool erreur = 0,
		valider = 0;
	
	size_t epoques,
		lotsParEpoque,
		tailleLot;

	do
	{
		std::cout << "\n- Nombre d'epoques:\n> ";
		erreur |= !(std::cin >> epoques);
		std::cout << "- Lots par epoque:\n> ";
		erreur |= !(std::cin >> lotsParEpoque);
		std::cout << "- Taille du lot:\n> ";
		erreur |= !(std::cin >> tailleLot);

		std::cout << "- Valider (0) / (1) ?\n> ";
		erreur |= !(std::cin >> valider);


		if (erreur)
		{
			std::cerr << "Mauvais nombre\n";
			std::exit(-1);
			return;
		}

		// Tant qu'on ne valide pas on recommence
	} while (!valider);


	//// Init ////
	// Faux aléatoire
	srand(1234U);

	//// Affichage ////
	Afficheur affichage;

	affichage.epoques = 1U;
	affichage.afficherChargement = true;
	affichage.afficherEnregistrement = true;
	affichage.afficherEntrainement = true;
	affichage.afficherLot = true;


	//// Réseau de neuronnes ////
	ReseauNeurone nn = creerReseau();
		

	// Pour afficher les bonnes informations
	nn.affichage = affichage;


	//// Bases de données ////
	std::cout << "\nChargement des images...\n";
	const Dataset DONNEES_X = io::chargerImages(FICHIER_IMAGES_TRAIN, true);

	std::cout << "Chargement des labels...\n";
	const Dataset DONNEES_Y = io::chargerLabels(FICHIER_LABELS_TRAIN);

	std::cout << "Nombre d'echantillons: " << DONNEES_Y.size() << '\n';


	//////// Chargement & entraînement /////////
	// Si on ne le recrée pas alors qu'il est trouvé
	// on test juste l'efficacité
	bool recreer;
	std::cout << "\nRecreer le reseau ? (0 / 1)\n> ";
	std::cin >> recreer;
	if (!nn.charger(recreer))
	{
		// Entraînement
		nn.entrainement(DONNEES_X, DONNEES_Y, epoques, lotsParEpoque, tailleLot);

		// Enregistrement
		nn.enregistrer();
	}


	//////// Test ////////
	//// Datasets ////
	std::cout << "Chargement des donnees de test...\n";
	const Dataset TEST_X = io::chargerImages(FICHIER_IMAGES_TEST, true),
		TEST_Y = io::chargerLabels(FICHIER_LABELS_TEST);


	//// Efficacite ////
	std::cout << "Calcul de l'efficacite...\n";
	std::cout << "Efficacite: " << nn.efficacite(TEST_X, TEST_Y, estimerChiffre) * 100.F << "%\n";


	//// Prédictions ////
	const size_t TAILLE_TEST = 20U;

	std::cout << "Predictions...\n";
	const Dataset PRED = nn.prediction(TEST_X);

	// Affichage de chaque prédiction
	for (unsigned int i = 0U; i < TAILLE_TEST; i++)
	{
		std::cout << "Prediction, image d'un " << TEST_Y[i].argmax().second <<
			" : " << PRED[i].argmax().second << '\n';
	}
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
	//// Init ////
	using namespace maths;

	// Faux aléatoire
	srand(1234U);


	//// Réseau de neuronnes ////
	ReseauNeurone nn = creerReseau();


	//////// Chargement /////////
	std::cout << "Chargement du reseau...\n";
	if (!nn.charger())
	{
		std::cerr << "Erreur de chargement du reseau\n";

		return;
	}

	std::cout << "Reseau charge\n";


	//// Entrées ////
	// Emplacement du fichier image
	std::string fichier;

	// Interface utilisateur
	std::cout << "\nVeuillez entrer l'emplacement de l'image\n> ";

	// Pour vérouiller l'entrée
	std::cin.ignore();

	// Entrée
	std::getline(std::cin, fichier);

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


#ifdef _DEBUG
	// Affiche les chiffres seulement en debug
	for (const Matrice &c : chiffres)
	{
		std::cout << "----------------------------\n";
		afficherChiffre(c);
	}
#endif

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


	// Pour la lisibilité
	std::cout << "\n\n";
}



void predireChiffre()
{
	//// Init ////
	using namespace maths;

	// Faux aléatoire
	srand(1234U);


	//// Réseau de neuronnes ////
	ReseauNeurone nn = creerReseau();


	//////// Chargement /////////
	std::cout << "Chargement du reseau...\n";
	if (!nn.charger())
	{
		std::cerr << "Erreur de chargement du reseau\n";

		return;
	}

	std::cout << "Reseau charge\n";


	//// Entrées ////
	// Emplacement du fichier image
	std::string fichier;

	// Interface utilisateur
	std::cout << "\nVeuillez entrer l'emplacement de l'image\n> ";

	// Pour vérouiller l'entrée
	std::cin.ignore();

	// Entrée
	std::getline(std::cin, fichier);

	// Quand on fait un glissé-déposé il y a des guillemets, on doit les enlever
	if (fichier.size() > 2U && fichier.front() == '"' && fichier.back() == '"')
		fichier = std::string(fichier.begin() + 1U, fichier.end() - 1U);


	//// Lecture ////
	std::cout << "Lecture de l'image...\n";
	const Matrice IMAGE = io::chargerImage(fichier);

	if (IMAGE.getLignes() != 28U || IMAGE.getColones() != 28U)
	{
		std::cerr << "Erreur de lecture de l'image ou l'image n'a pas une bonne taille (28 * 28 px)\n";

		return;
	}


#ifdef _DEBUG
	// Affiche le chiffre seulement en debug
	afficherChiffre(IMAGE);
#endif

	//// Prédictions ////
	std::cout << "\n-> Le chiffre est: ";
	std::cout << nn.prediction(Matrice(IMAGE).dimensionner(1U, 784U)).argmax().second << "\n\n";
}




ReseauNeurone creerReseau()
{
	using namespace maths;

	std::string nom;

	std::cout << "\nEntrez le nom du reseau a charger\n> ";
	std::cin >> nom;


	if (nom == "CcNet1")
	{
		return ReseauNeurone({
				new CoucheDense(784U, 200U, aleatoireMir),
				new CoucheActivation(sigmoide, sigmoidePrime),

				new CoucheDense(200U, 128U, aleatoireMir),
				new CoucheActivation(sigmoide, sigmoidePrime),

				new CoucheDense(128U, 10U, aleatoireMir),
				new CoucheActivation(sigmoide, sigmoidePrime),
			},
			.01F,
			nom,
			"donnees");
	}
	else if (nom == "CcNet2")
	{
		return ReseauNeurone({
				new CoucheDense(784U, 128U, aleatoireMir),
				new CoucheActivation(sigmoide, sigmoidePrime),

				new CoucheLache(.5F),

				new CoucheDense(128U, 10U, aleatoireMir),
				new CoucheActivation(sigmoide, sigmoidePrime),
			},
			.0065F,
			nom,
			"donnees");
	}
	else if (nom == "CcNet3")
	{
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


			//// 10 * 5 * 1024 ////
			//.3F,		91.4	instable
			//.2F,		92.53	instable
			//.01F,		74.11
			//.02F,		86.25
			//.035F,	88.79
			//.05F,		89.74



			//// 5 * 5 * 256 ////
			//.02F,		35
			//.01F,		23
			//.008F,	21
			//.0065F,	20
			//.005F,	17
			nom,
			"donnees");
	}

	std::cerr << "Erreur de creation du reseau (mauvais nom)\n";

	return ReseauNeurone({}, 0.F);
}


