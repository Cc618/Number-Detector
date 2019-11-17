#ifdef _DEBUG

// Tests

#include <iostream>

#include "ReseauNeurone.h"
#include "Afficheur.h"
#include "traitementImage.h"
#include "couches.h"
#include "constantes.h"
#include "maths.h"
#include "debug.h"





void imgProc(),
	conv(),
	simpleImage(const std::string&),
	image(const std::string& FICHIER_IMAGE),
	mainXOR(),
	mainMNIST(),
	mainMNIST2();


extern ReseauNeurone creerReseau(const std::string& NOM);



int mainDebug()
{
	imgProc();


	// for (size_t i = 0U; i < 10U; i++)
	// 	simpleImage("D:/donnees/documents/cc/code/cpp/fichiers/projets/Detecteur Nombre/res/images/tests/datasets/test_b/" + std::to_string(i) + ".jpg");

	/*
	try
	{
		image();
	}
	catch (MatriceException e)
	{
		std::cout << e.what() << '\n';
	}
	*/

	system("pause");

	return 0;
}




// Vérifie si la table a bien été prédite
bool verificationXOR(const Matrice &PRED, const Matrice &y)
{
	const bool VAUT_TRUE = y.en(0U, 0U) < y.en(0U, 1U),
		PRED_TRUE = PRED.en(0U, 0U) < PRED.en(0U, 1U);

	return VAUT_TRUE == PRED_TRUE;
}



// Réseau prédisant la table XOR
void mainXOR()
{
	using namespace maths;

	// Faux aléatoire pour les tests
	srand(1234U);

	// Affichage
	Afficheur aff;

	//aff.afficherEntrainement = true;
	//aff.epoques = 600U;


	//// Données ////
	const Dataset DONNEES_X = Matrice::datasetLigne({
			0, 0,
			1, 0,
			0, 1,
			1, 1,
		}, 2U),
		DONNEES_Y = Matrice::datasetLigne({
			1, 0,
			0, 1,
			0, 1,
			1, 0,
		}, 2U);

	Afficheur::afficher(DONNEES_X);
	Afficheur::afficher(DONNEES_Y);
	
	//// Réseau de neuronnes ////
	ReseauNeurone nn = ReseauNeurone({
			new CoucheDense(2U, 4U, aleatoireMir),
			new CoucheActivation(sigmoide, sigmoidePrime),

			new CoucheDense(4U, 2U, aleatoireMir),
			new CoucheActivation(sigmoide, sigmoidePrime),
		},
		.5F,
		"reseau_xor",
		"donnees/tst");
	

	if (!nn.charger(true))
	{
		nn.entrainement(DONNEES_X, DONNEES_Y, 1000U);

		nn.enregistrer();
	}

	std::cout << "Erreur: " << nn.erreur(DONNEES_X, DONNEES_Y);

	std::cout << nn.efficacite(DONNEES_X, DONNEES_Y, verificationXOR) * 100.F << "%\n";

	/*
	const Dataset PRED = nn.prediction(DONNEES_X);
	
	for (unsigned int i = 0U; i < PRED.size(); i++)
		PRED[i].afficher();
	*/
// Vérifie si l'image a bien été prédite
}



// Vérifie si l'image a bien été prédite
bool verificationMNIST(const Matrice &PRED, const Matrice &y)
{
	return PRED.argmax().second == y.argmax().second;
}



// Tests sur MNIST, réseau de neurone petit avec peu d'échantillons
void mainMNIST()
{
	using namespace maths;

	// Faux aléatoire pour les tests
	srand(1234U);

	// Affichage
	Afficheur affichage;

	affichage.epoques = 1U;
	affichage.afficherChargement = true;
	affichage.afficherEnregistrement = true;
	affichage.afficherEntrainement = true;



	//// Données ////
	std::cout << "Chargement des images...\n";
	const Dataset X_DATA = io::chargerImages(FICHIER_IMAGES_TEST, true);

	const size_t S = 300U;

	Dataset donneesX(X_DATA.cbegin(), X_DATA.cbegin() + S);
	


	std::cout << "Chargement des labels...\n";
	const Dataset Y_DATA = io::chargerLabels(FICHIER_LABELS_TEST);


	Dataset donneesY(Y_DATA.cbegin(), Y_DATA.cbegin() + S);


	std::cout << "Nombre d'echantillons: " << donneesY.size() << '\n';




	//// Réseau de neuronnes ////
	ReseauNeurone nn = ReseauNeurone({
			new CoucheDense(784U, 128U, aleatoireMir),
			new CoucheActivation(sigmoide, sigmoidePrime),

			new CoucheDense(128U, 10U, aleatoireMir),
			new CoucheActivation(sigmoide, sigmoidePrime),
		},
		.01F,
		"reseau_mnist",
		"donnees/tst");

	// Pour afficher les bonnes informations
	nn.affichage = affichage;


	if (!nn.charger(true))
	{
		nn.entrainement(donneesX, donneesY, 1U);

		nn.enregistrer();
	}

	//////// Test ////////
	//// Datasets ////
	const size_t TAILLE_TEST = 15U;

	const Dataset TEST_X(donneesX.cbegin(), donneesX.cbegin() + TAILLE_TEST),
		TEST_Y(donneesY.cbegin(), donneesY.cbegin() + TAILLE_TEST);

	std::cout << "Predictions...\n";
	const Dataset PRED = nn.prediction(TEST_X);

	for (unsigned int i = 0U; i < TAILLE_TEST; i++)
	{
		std::cout << "Prediction, image d'un " << TEST_Y[i].argmax().second <<
			" : " << PRED[i].argmax().second << '\n';
	}
}



// Réseau de neuronne complet sur MNIST
void mainMNIST2()
{
	using namespace maths;

	// Faux aléatoire pour les tests
	srand(1234U);


	//// Affichage ////
	Afficheur affichage;

	affichage.epoques = 1U;
	affichage.afficherChargement = true;
	affichage.afficherEnregistrement = true;
	affichage.afficherEntrainement = true;


	//// Données ////
	std::cout << "Chargement des images...\n";
	const Dataset X_DATA = io::chargerImages(FICHIER_IMAGES_TEST, true);

	const size_t S = 1000U;
	const Dataset DONNEES_X(X_DATA.cbegin(), X_DATA.cbegin() + S);

	std::cout << "Chargement des labels...\n";
	const Dataset Y_DATA = io::chargerLabels(FICHIER_LABELS_TEST);
	const Dataset DONNEES_Y(Y_DATA.cbegin(), Y_DATA.cbegin() + S);

	//std::cout << "Nombre d'echantillons: " << donneesY.size() << '\n';


	//// Réseau de neuronnes ////
	ReseauNeurone nn = ReseauNeurone({
			/*
			new CoucheDense(784U, 128U, aleatoireMir),
			new CoucheActivation(sigmoide, sigmoidePrime),


			new CoucheDense(128U, 10U, aleatoireMir),
			new CoucheActivation(sigmoide, sigmoidePrime),
			*/

				
			//new CoucheDense(784U, 200U, aleatoireMir),
			//new CoucheActivation(sigmoide, sigmoidePrime),

			new CoucheDense(784U, 128U, aleatoireMir),
			new CoucheActivation(maths::tanh, tanhPrime),

			new CoucheDense(128U, 10U, aleatoireMir),
			new CoucheActivation(sigmoide, sigmoidePrime),
		},
		.01F,
		"reseau_mnist_2",
		"donnees/tst");

	// Pour afficher les bonnes informations
	nn.affichage = affichage;


	//////// Chargement /////////
	if (!nn.charger(true))
	{
		nn.entrainement(DONNEES_X, DONNEES_Y, 4U);

		nn.enregistrer();
	}


	//////// Test ////////
	//// Datasets ////
	const size_t TAILLE_TEST = 50U;

	const Dataset TEST_X(DONNEES_X.cbegin(), DONNEES_X.cbegin() + TAILLE_TEST),
		TEST_Y(DONNEES_Y.cbegin(), DONNEES_Y.cbegin() + TAILLE_TEST);


	//// Efficacite ////
	//std::cout << "Efficacite: " << nn.efficacite(TEST_X, TEST_Y, verificationMNIST) * 100.F << "%\n";
	std::cout << "Efficacite: " << nn.efficacite(DONNEES_X, DONNEES_Y, verificationMNIST) * 100.F << "%\n";


	//// Prédictions ////
	std::cout << "Predictions...\n";
	const Dataset PRED = nn.prediction(TEST_X);

	for (unsigned int i = 0U; i < TAILLE_TEST; i++)
	{
		std::cout << "Prediction, image d'un " << TEST_Y[i].argmax().second <<
			" : " << PRED[i].argmax().second << '\n';
	}
}



// Tests sur les images
void image(const std::string& FICHIER_IMAGE)
{
	using namespace maths;


	//// Lecture ////
	std::cout << "Lecture de l'image...\n";
	const Matrice IMAGE = io::chargerImage(FICHIER_IMAGE);

	if (IMAGE.estVide())
	{
		std::cerr << "Erreur de lecture de l'image\n";

		return;
	}

	//// Découpe ////
	// Découpe du nombre en chiffres
	std::cout << "Decoupage de l'image...\n";
	const Dataset CHIFFRES = img::couperImage(IMAGE);
	std::cout << CHIFFRES.size() << " chiffres trouves\n";

	//// Traitement (réduction) ////
	// Elements 
	// On n'utilise pas CHIFFRES car il peut y en avoir moins
	Dataset elements;
	elements.reserve(CHIFFRES.size());

	// On réduit en matrice 28x28
	for (size_t n = 0U; n < CHIFFRES.size(); n++)
	{
		// Résultat de la réduction (peut être nul)
		const Matrice CHIFFRE = img::maxPooling(CHIFFRES[n]);

		// Chiffre invalide (trop petit)
		if (CHIFFRE.estVide())
			continue;

		// Si non nul on ajoute le chiffre 
		elements.emplace_back(CHIFFRE);
	}

	/*
	for (const Matrice &c : CHIFFRES)
	{
		std::cout << "----------------------------\n";
		afficherMatrice(c);
	}

	for (const Matrice &c : elements)
	{
		std::cout << "----------------------------\n";
		afficherChiffre(c);
	}
	*/


	//// Réseau de neuronnes ////
	ReseauNeurone nn = ReseauNeurone({
		new CoucheDense(784U, 128U, aleatoireMir),
		new CoucheActivation(maths::tanh, tanhPrime),

		new CoucheDense(128U, 10U, aleatoireMir),
		new CoucheActivation(sigmoide, sigmoidePrime),
		},
		.01F,
		"reseau_mnist_2",
		"donnees/tst");


	//////// Chargement /////////
	std::cout << "Chargement du reseau...\n";
	if (!nn.charger())
	{
		std::cerr << "Erreur de chargement du reseau\n";

		return;

		//nn.entrainement(DONNEES_X, DONNEES_Y, 4U);

		//nn.enregistrer();
	}

	std::cout << "Reseau charge\n";


	//// Prédictions ////
	std::cout << "Predictions...\n";
	const Dataset PRED = nn.prediction(elements);

	for (unsigned int i = 0U; i < elements.size(); i++)
		std::cout << PRED[i].argmax().second << '\n';
}



// Tests sur une image unique
void simpleImage(const std::string &FICHIER_IMAGE)
{
	using namespace maths;


	//// Lecture ////
	std::cout << "Lecture de l'image...\n";
	const Matrice IMAGE = io::chargerImage(FICHIER_IMAGE).dimensionner(1U, 784U);

	if (IMAGE.estVide())
	{
		std::cerr << "Erreur de lecture de l'image\n";

		return;
	}
	else if (IMAGE.getColones() != 784U)
	{
		std::cerr << "L'image doit faire du 28x28 px\n";

		return;
	}


	//// Réseau de neuronnes ////
	ReseauNeurone nn = creerReseau("CcNet2");


	//////// Chargement /////////
	std::cout << "Chargement du reseau...\n";
	if (!nn.charger())
	{
		std::cerr << "Erreur de chargement du reseau\n";

		return;

		//nn.entrainement(DONNEES_X, DONNEES_Y, 4U);

		//nn.enregistrer();
	}

	std::cout << "Reseau charge\n";


	//// Prédictions ////
	std::cout << "Prediction...\n";
	const Matrice PRED = nn.prediction(IMAGE);

	std::cout << "\n-----------------------------\n\n" <<
		"Chiffre predit: " << PRED.argmax().second << '\n';
}



// Garde la même taille
Matrice convolution(const Matrice& ENTREE, const Matrice& FILTRE)
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


// Max pooling, diminue la taille de la matrice
// Attention: DIM_X et DIM_Y doivent être des diviseurs
// des dimensions de ENTREE (pas de vérification)
// sinon il y a des pertes
Matrice maxPooling(const Matrice& ENTREE, const size_t DIM_X, const size_t DIM_Y)
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



// Tests pour la convolution
void conv()
{
	const Matrice ENTREE(
			{
				0, 0, 0, 0, 0, 0,
				0, 0, 1, 1, 0, 0,
				0, 1, 1, 1, 0, 0,
				0, 0, 0, 1, 0, 0,
				0, 0, 0, 1, 0, 0,
				0, 0, 1, 1, 1, 0,
			}, 6U, 6U
		),
		FILTRE_1(
			{
				0, 1, 0,
				0, 1, 0,
				0, 1, 0,
			}, 3U, 3U
		),
		FILTRE_2(
			{
				0, 0, 0,
				1, 1, 1,
				0, 0, 0,
			}, 3U, 3U
		),
		FILTRE_3(
			{
				0, 1, 0,
				1, 1, 1,
				0, 1, 0,
			}, 3U, 3U
		);


	ENTREE.afficher("%.1F");


	maxPooling(ENTREE, 2U, 2U).afficher();
	maxPooling(ENTREE, 4U, 4U).afficher();

/*
	convolution(ENTREE, FILTRE_1).afficher("%.1F");
	convolution(ENTREE, FILTRE_2).afficher("%.1F");
	convolution(ENTREE, FILTRE_3).afficher("%.1F");*/
}







// Tests sur le traitement des images
void imgProc()
{
	const Matrice IMAGE = io::chargerImage("D:/donnees/documents/cc/code/cpp/fichiers/projets/Detecteur Nombre/res/images/tests/mnist_85596423.jpg");


	
	//// Découpe ////
		// Découpe du nombre en chiffres
	std::cout << "Decoupage de l'image...\n";
	const Dataset ELEMENTS = img::couperImage(IMAGE);
	std::cout << ELEMENTS.size() << " chiffres trouves\n";


/*
	for (const Matrice& e : ELEMENTS)
	{
		std::cout << "----------------------------\n";
		afficherMatrice(e);
	}*/



	

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




	// Affiche les chiffres seulement en debug
	for (const Matrice& c : chiffres)
	{
		std::cout << "----------------------------\n";
		afficherChiffre(c);
	}
	















	/*
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
	*/






}









#endif // _DEBUG
