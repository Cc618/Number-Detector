#pragma once

/*
 *		R�seau de neurones qui permet de faire une interface haut niveau.
 */

#include <vector>
#include <string>

#include "Matrice.h"
#include "couches.h"
#include "Afficheur.h"


class ReseauNeurone
{
public:
	// Attention EMPLACEMENT doit avoir tous les dossiers n�cessaires cr��s
	ReseauNeurone(const std::vector<Couche*> &COUCHES, const float TAUX_APPRENTISSAGE,
		const std::string &NOM = "sans_nom", const std::string &EMPLACEMENT = "")
		: _couches(COUCHES), TAUX_APPRENTISSAGE(TAUX_APPRENTISSAGE), 
		NOM(NOM), EMPLACEMENT(EMPLACEMENT)
	{
		assignerEmplacement();
	}


public:
	//////// Un �chantillon ////////
	// Pr�dit une sortie � partir de l'entr�e
	Matrice prediction(const Matrice &x) const;

	// Calcul l'erreur entre la pr�diction et le label (Mean Squared Error)
	float erreur(const Matrice &x, const Matrice &y) const;


	//////// Plusieurs �chantillons ////////
	// Am�liore les pr�dictions
	// - NOMBRE_AFFICHAGES : Pour afficher une trace de l'avancement toutes ses �poques.
	void entrainement(
		const Dataset &x, const Dataset &y,
		const size_t EPOQUES,
		const size_t NOMBRE_LOTS = 1U, const size_t TAILLE_LOTS = 255U);

	// Pr�dit une sortie � partir de l'entr�e
	Dataset prediction(const Dataset &x) const;

	// Calcul l'erreur dans un ensemble d'�chantillons
	// Si on veut faire l'erreur totale on met la taille du lot � 0U
	float erreur(const Dataset &x, const Dataset &y, const size_t TAILLE_LOT = 0U) const;

	//	Calcul entre 0 et 1 le taux de bonnes r�ponses
	// - verification : Fonction renvoyant true si on juge que c'est une pr�diction convenable par rapport au label
	float efficacite(const Dataset &x, const Dataset &y, bool (*verification)(const Matrice &PRED, const Matrice &LABEL));


public:
	// Charge le r�seau avec les fichiers (EMPLACEMENT -> x/y, c'est un dossier)
	// Retourne si tout s'est charg�
	// - FORCER_CHARGEMENT : Si true on n'essaye pas de charger par fichiers (renvoie false)
	bool charger(const bool FORCER_CHARGEMENT = false);
	// Enregistre tout
	void enregistrer() const;


public:
	// Donne les couches en lecture seule
	inline const std::vector<Couche*> getCouches() const
	{
		return _couches;
	}


public:
	Afficheur affichage;


private:
	// Pr�diction pour chaque couche
	Dataset propagationAvant(const Matrice &x) const;

	// Change les poids pour mieux pr�dire
	void propagationArriere(const Matrice &x, const Matrice &y);


private:
	// Change les poids pour mieux pr�dire
	void propagationArriere(const Dataset &x, const Dataset &y, const size_t TAILLE_LOT);


private:
	// Pour chaque couche on assigne le bon emplacement
	void assignerEmplacement();


private:
	// Les couches pour pr�dire la sorties
	std::vector<Couche*> _couches;

	const float TAUX_APPRENTISSAGE;

	// Pour le charger / l'enregistrer
	const std::string NOM,
		EMPLACEMENT;
};

