#pragma once

/*
 *		Toutes les couches sont regroupées ici.
 */

#include <vector>
#include <iostream>

#include "Matrice.h"
#include "Fichiers.h"



// Classe abstraite de base de toutes les couches
class Couche
{
public:
	// Feed Forward
	// - x : L'entrée à propager
	virtual Matrice propagationAvant(const Matrice &x) const = 0;

	// Back Propagation
	// - x : L'entrée à propagée en avant en premier
	// - DELTA : Matrice de dérivée partielle (celle située juste après cette couche)
	virtual void propagationArriere(const Matrice &x, const Matrice &DELTA)
	{}

	// Calcul le prochain delta (dans le sens de la propagation arrière, celui avant cette couche)
	// - x : Entrée 
	// - DELTA_PRECEDENT : Matrice de dérivée partielle (celle située juste après cette couche)
	virtual Matrice delta(const Matrice &x, const Matrice &DELTA_PRECEDENT) const = 0;

};


//	Une couche contenant des poids / d'autres données pouvant
// s'enregistrer et se charger
class ICoucheEnregistrable : public Couche
{
public:
	// Pour initialisé les données
	struct initialiseur
	{
	public:
		initialiseur(const std::string &EMPLACEMENT)
			: emplacement(EMPLACEMENT)
		{}

	public:
		std::string emplacement;
	};


public:
	ICoucheEnregistrable(initialiseur *initialisation)
		: initialisation(initialisation)
	{}

	virtual ~ICoucheEnregistrable()
	{
		if (initialisation != nullptr)
		{
			std::cout << "DELETE\n";
			delete initialisation;
		}
	}


public:
	// Renvoie si chargé ou pas
	virtual bool charger(const bool FORCER_CHARGEMENT = false) = 0;
	virtual void enregistrer() const = 0;


public:
	initialiseur *initialisation = nullptr;
};


// Couche entièrement connectée
class CoucheDense : public ICoucheEnregistrable
{
public:
	// Permet d'initialisé les poids
	struct initialiseur : public ICoucheEnregistrable::initialiseur
	{
		//////// Constructeurs ////////
		// - TAILLE_X : Nombre de poids en entrée
		// - TAILLE_Y : Nombre de poids en sortie
		// - fonctionInit : Fonction de distribution des valeurs aléatoires de poids
		initialiseur(const unsigned int TAILLE_X, const unsigned int TAILLE_Y,
			float(*distribution)(), const std::string &EMPLACEMENT = "")
			: ICoucheEnregistrable::initialiseur(EMPLACEMENT),
			tailleX(TAILLE_X), tailleY(TAILLE_Y), distribution(distribution)
		{}

		//////// Attributs ////////
		unsigned int tailleX,
			tailleY;
		
		float (*distribution)();
	};


public:
	// - TAILLE_X : Nombre de poids en entrée
	// - TAILLE_Y : Nombre de poids en sortie
	// - distribution : Fonction de distribution des valeurs aléatoires de poids
	// - EMPLACEMENT : (format x/y/z -> fichiers x/y/z.biais.mat et x/y/z.poids.mat)
	CoucheDense(const unsigned int TAILLE_X, const unsigned int TAILLE_Y,
		float(*distribution)(), const std::string &EMPLACEMENT = "")
		: ICoucheEnregistrable(new CoucheDense::initialiseur(TAILLE_X, TAILLE_Y, distribution, EMPLACEMENT))
	{}
	
	CoucheDense(CoucheDense::initialiseur *initialisation)
		: ICoucheEnregistrable(initialisation)
	{}
	

public:
	virtual Matrice propagationAvant(const Matrice &x) const override;

	virtual void propagationArriere(const Matrice &x, const Matrice &DELTA) override;

	virtual Matrice delta(const Matrice &x, const Matrice &DELTA_PRECEDENT) const override;


public:
	// Enregistrement (format x/y/z -> fichiers x/y/z.biais.mat et x/y/z.poids.mat)
	virtual bool charger(const bool FORCER_CHARGEMENT = false) override;
	virtual void enregistrer() const override;

private:
	// Poids
	Matrice _poids,
		_biais;
};


// Couche d'activation
class CoucheActivation : public Couche
{
public:
	CoucheActivation(float (*fonction)(float), float (*derivee)(float))
		: fonction(fonction), derivee(derivee)
	{}


public:
	virtual Matrice propagationAvant(const Matrice &x) const override;

	virtual Matrice delta(const Matrice &x, const Matrice &DELTA_PRECEDENT) const override;


private:
	// Sa fonction avec sa dérivée
	float (*fonction)(float),
		(*derivee)(float);
};


// Couche de laché (Dropout)
// Sert à éviter l'overfitting
class CoucheLache : public Couche
{
public:
	// Le coefficient est le taux entre 0 et 1 de chance que la node
	// ne réponde pas
	CoucheLache(const float COEFFICIENT)
		: COEFFICIENT(COEFFICIENT)
	{}


public:
	// Ne change pas la propagation avant
	inline virtual Matrice propagationAvant(const Matrice& x) const override
	{
		return x;
	}

	virtual Matrice delta(const Matrice& x, const Matrice& DELTA_PRECEDENT) const override;


private:
	const float COEFFICIENT;
};


// Couche parallèle qui va juste
// additioner le résultat de plusieurs
// couches
// Couche entièrement connectée
/*
class CoucheParallele : public ICoucheEnregistrable
{
public:
	// Permet d'initialisé les poids
	struct initialiseur : public ICoucheEnregistrable::initialiseur
	{
		//////// Constructeurs ////////
		// - TAILLE_X : Nombre de poids en entrée
		// - TAILLE_Y : Nombre de poids en sortie
		// - fonctionInit : Fonction de distribution des valeurs aléatoires de poids
		initialiseur(const unsigned int TAILLE_X, const unsigned int TAILLE_Y,
			float(*distribution)(), const std::string& EMPLACEMENT = "")
			: ICoucheEnregistrable::initialiseur(EMPLACEMENT),
			tailleX(TAILLE_X), tailleY(TAILLE_Y), distribution(distribution)
		{}

		//////// Attributs ////////
		unsigned int tailleX,
			tailleY;

		float (*distribution)();
	};


public:
	// - TAILLE_X : Nombre de poids en entrée
	// - TAILLE_Y : Nombre de poids en sortie
	// - distribution : Fonction de distribution des valeurs aléatoires de poids
	// - EMPLACEMENT : (format x/y/z -> fichiers x/y/z.biais.mat et x/y/z.poids.mat)
	CoucheParallele(const unsigned int TAILLE_X, const unsigned int TAILLE_Y,
		float(*distribution)(), const std::string& EMPLACEMENT = "")
		: ICoucheEnregistrable(new CoucheDense::initialiseur(TAILLE_X, TAILLE_Y, distribution, EMPLACEMENT))
	{}

	CoucheDense(CoucheDense::initialiseur* initialisation)
		: ICoucheEnregistrable(initialisation)
	{}


public:
	virtual Matrice propagationAvant(const Matrice& x) const override;

	virtual void propagationArriere(const Matrice& x, const Matrice& DELTA) override;

	virtual Matrice delta(const Matrice& x, const Matrice& DELTA_PRECEDENT) const override;


public:
	// Enregistrement (format x/y/z -> fichiers x/y/z.biais.mat et x/y/z.poids.mat)
	virtual bool charger(const bool FORCER_CHARGEMENT = false) override;
	virtual void enregistrer() const override;

private:
	// Poids
	Matrice _poids,
		_biais;
};
*/



// Couche de convolution
// Garde la même taille d'entrée en sortie
class CoucheConvolution : public Couche
{
public:
	CoucheConvolution(const Matrice &FILTRE)
		: FILTRE(FILTRE)
	{}


public:
	virtual Matrice propagationAvant(const Matrice& x) const override;

	virtual Matrice delta(const Matrice& x, const Matrice& DELTA_PRECEDENT) const override;


private:
	const Matrice FILTRE;
};




// Couche de max pooling.
// Diminue la taille de la matrice
// Attention: DIM_X et DIM_Y doivent être des diviseurs
// des dimensions de ENTREE (pas de vérification)
// sinon il y a des pertes
class CoucheMaxPooling : public Couche
{
public:
	CoucheMaxPooling(const size_t DIM_X, const size_t DIM_Y)
		: DIM_X(DIM_X), DIM_Y(DIM_Y)
	{}


public:
	virtual Matrice propagationAvant(const Matrice& x) const override;

	virtual Matrice delta(const Matrice& x, const Matrice& DELTA_PRECEDENT) const override;


private:
	const size_t DIM_X,
		DIM_Y;
};




