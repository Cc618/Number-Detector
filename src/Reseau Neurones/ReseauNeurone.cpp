#include "ReseauNeurone.h"

#include <Windows.h>

#include "maths.h"



Matrice ReseauNeurone::prediction(const Matrice & x) const
{
	// C'est la pr�diction de la toute derni�re couche
	return propagationAvant(x).back();
}

void ReseauNeurone::propagationArriere(const Matrice & x, const Matrice & y)
{
	// Propagation Avant
	Dataset predictions = propagationAvant(x);

    // Delta de l'erreur qu'on va propager (on met le taux d'apprentissage ici c'est moins co�teux)
	Matrice delta = (predictions.back() - y) * TAUX_APPRENTISSAGE;

	unsigned int couche = _couches.size() - 1U;
	while (true)
	{
		// Propagation arri�re
		_couches[couche]->propagationArriere(predictions[couche], delta);

		// Si on est � la premi�re couche on sort directement
		if (couche == 0U)
			break;

		// Mise � jour du delta
		delta = _couches[couche]->delta(predictions[couche], delta);

		// Mise � jour de la boucle
		couche--;
	}
}

void ReseauNeurone::propagationArriere(const Dataset& x, const Dataset& y, const size_t TAILLE_LOT)
{
	// Nombre d'�chantillons
	const size_t NOMBRE = x.size();

	if (NOMBRE != y.size())
	{
		std::cerr << "Erreur: Il doit y avoir autant d'echantillons d'entree que de sortie\n";
		return;
	}

	// Cr�ation du lot //
	// Tableau d'indices al�atoires qui forment le lot
	const std::vector<size_t> LOT = maths::choixAleatoire(NOMBRE, TAILLE_LOT);

	// Propagation Arriere //
	for (const size_t ECHANTILLON : LOT)
		propagationArriere(x[ECHANTILLON], y[ECHANTILLON]);
}

float ReseauNeurone::erreur(const Matrice & x, const Matrice & y) const
{
	const Matrice COUT = x - y;

	return (COUT * COUT).somme() / static_cast<float>(x.getColones());
}

void ReseauNeurone::entrainement(const Dataset& x, const Dataset& y, const size_t EPOQUES, const size_t NOMBRE_LOTS, const size_t TAILLE_LOT)
{
	if (affichage.afficherEntrainement)
		affichage.affiche("\n----------------------------\n" "Debut de l'entrainement...\n");

	for (unsigned int e = 0U; e < EPOQUES; e++)
	{
		//// Entra�nement ////
		for (size_t lot = 0U; lot < NOMBRE_LOTS; lot++)
		{
			if (affichage.afficherLot)
				affichage.affiche("- Lot: %u\n", lot);

			propagationArriere(x, y, TAILLE_LOT);
		}

		//// Affichage ////
		if (e % affichage.epoques == 0U)
			affichage.affiche("- Epoque: %u, Erreur: %.4F\n\n", e, erreur(x, y, TAILLE_LOT));
	}

	if (affichage.epoques != -1)
		affichage.affiche("- Fin de l'entrainement: Epoque: %u, Erreur: %.4F"
			"\n----------------------------\n", EPOQUES, erreur(x, y, TAILLE_LOT));
}

Dataset ReseauNeurone::prediction(const Dataset & x) const
{
	const unsigned int TAILLE = x.size();

	Dataset y;
	y.reserve(TAILLE);

	// Pour chaque entr�e on fait une pr�diction
	for (const Matrice &ENTREE : x)
		y.emplace_back(prediction(ENTREE));

	return y;
}

float ReseauNeurone::erreur(const Dataset& x, const Dataset& y, const size_t TAILLE_LOT) const
{
	// Nombre d'�chantillons
	const size_t NOMBRE = x.size();

	if (NOMBRE != y.size())
	{
		std::cerr << "Erreur: Il doit y avoir autant d'echantillons d'entree que de sortie\n";
		return -1.F;
	}

	// Somme des erreurs
	float somme = 0.F;

	// Erreur totale
	if (TAILLE_LOT == 0U)
	{

		for (unsigned int echantillon = 0U; echantillon < NOMBRE; echantillon++)
			somme += erreur(prediction(x[echantillon]), y[echantillon]);

		// Renvoie de la moyenne
		return somme / static_cast<float>(NOMBRE);
	}
	// Erreur du lot
	else
	{
		// Cr�ation du lot //
		// Tableau d'indices al�atoires qui forment le lot
		const std::vector<size_t> LOT = maths::choixAleatoire(NOMBRE, TAILLE_LOT);

		// Propagation Arriere //
		for (const size_t ECHANTILLON : LOT)
			somme += erreur(prediction(x[ECHANTILLON]), y[ECHANTILLON]);
	}

	// Renvoie de la moyenne
	return somme / static_cast<float>(TAILLE_LOT);
}

float ReseauNeurone::efficacite(const Dataset & x, const Dataset & y, bool(*verification)(const Matrice &PRED, const Matrice &LABEL))
{
	// Nombre de bonnes pr�dictions
	size_t bonnesPredictions = 0U;

	const size_t TAILLE = y.size();

	const Dataset PRED = prediction(x);

	// On compte le nombre de bonnes pr�dictions
	for (size_t i = 0U; i < TAILLE; i++)
		if (verification(PRED[i], y[i]))
			bonnesPredictions++;

	return static_cast<float>(bonnesPredictions) / static_cast<float>(TAILLE);
}

bool ReseauNeurone::charger(const bool FORCER_CHARGEMENT)
{
	// Test si le dossier existe
	DWORD dossier = GetFileAttributesA(EMPLACEMENT.c_str());

	// On veut pouvoir enregistrer les poids mais le dossier 
	// n'existe pas, inutile d'aller plus loin
	if (!NOM.empty() && (dossier == INVALID_FILE_ATTRIBUTES || !(dossier & FILE_ATTRIBUTE_DIRECTORY)))
		return false;

	if (affichage.afficherChargement)
		affichage.affiche("%s -> Debut du chargement\n", NOM.c_str());

	bool erreur = false;

	for (size_t i = 0U; i < _couches.size(); i++)
	{
		ICoucheEnregistrable *const couche = dynamic_cast<ICoucheEnregistrable*>(_couches[i]);

		// La couche peut se charger
		if (couche != nullptr)
		{
			// Chargement
			erreur |= !couche->charger(FORCER_CHARGEMENT);
		}
	}

	if (!erreur)
	{
		if (affichage.afficherChargement)
			affichage.affiche("%s -> Chargement termine\n", NOM.c_str());

		return true;
	}
	else
		return false;
}

void ReseauNeurone::enregistrer() const
{
	//// V�rification ////
	// Inutile, ne s'enregistre pas si le nom n'est pas renseign�
	if (NOM.empty())
		return;

	//// Affichage ////
	if (affichage.afficherEnregistrement)
		affichage.affiche("%s -> Debut de l'enregistrement...\n", NOM.c_str());

	//// Dossier ////
	// On cr�e le dossier si besoin
	const std::string DOSSIER = EMPLACEMENT + '/' + NOM;

	// Test s'il existe
	DWORD dossier = GetFileAttributesA(DOSSIER.c_str());

	// Il n'existe pas : cr�ation
	if (dossier == INVALID_FILE_ATTRIBUTES || !(dossier & FILE_ATTRIBUTE_DIRECTORY))
		CreateDirectory(DOSSIER.c_str(), NULL);

	//// Couches ////
	// On cr�e chaque fichier couche_0, couche_1 ... couche_n
	for (size_t i = 0U; i < _couches.size(); i++)
	{
		const ICoucheEnregistrable *const couche = dynamic_cast<ICoucheEnregistrable*>(_couches[i]);

		// La couche peut s'enregistrer
		if (couche != nullptr)
			couche->enregistrer();
	}

	//// Affichage ////
	if (affichage.afficherEnregistrement)
		affichage.affiche("%s -> Enregistrement termine\n", NOM.c_str());
}

Dataset ReseauNeurone::propagationAvant(const Matrice & x) const
{
	// Allocation des pr�dictions
	Dataset predictions;
	predictions.reserve(_couches.size() + 1U);
	predictions.emplace_back(x);

	// Pr�diction pour chaque couche
	for (unsigned int couche = 0U; couche < _couches.size(); couche++)
		predictions.emplace_back(
			_couches[couche]->propagationAvant(predictions[couche])
		);

	return predictions;
}

void ReseauNeurone::assignerEmplacement()
{
	// Inutile, ne s'enregistre pas si le nom n'est pas renseign�
	if (NOM.empty())
		return;

	for (size_t i = 0U; i < _couches.size(); i++)
	{
		// Conversion
		const ICoucheEnregistrable *const couche = dynamic_cast<ICoucheEnregistrable*>(_couches[i]);

		// La couche peut s'enregistrer
		if (couche != nullptr)
			// Changement de l'emplacement automatique
			couche->initialisation->emplacement = EMPLACEMENT + '/' + NOM + "/couche_" + std::to_string(i);
	}
}
