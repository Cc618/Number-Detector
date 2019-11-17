#pragma once

/*
 *		Pour le calcul matricel.
 */

#include <exception>
#include <vector>
#include <initializer_list>
#include <string>


// Pour lever les exceptions de matrices correctement
struct MatriceException
{
public:
	// Toutes les exceptions
	enum TYPES
	{
		// DONNEES_SPECIALES = &std::string, texte � afficher
		SPECIALE,

		// Les deux dimensions ne sont pas align�es (dimensions incorrectes)
		// DONNEES_SPECIALES = &std::array<uint, 4U>{ A_LIGNES, A_COLONES, B_LIGNES, B_COLONES }
		DIMENSIONS_NON_ALIGNEES,

		// L'�l�ment d�sir� est � l'ext�rieur de la matrice
		// DONNEES_SPECIALES = &std::array<uint, 4U>{ LIGNE, COLONE, MAX_LIGNES, MAX_COLONES }
		ELEMENT_DEHORS,

		// Les donn�es fournies ne comportent pas assez / trop d'�l�ment
		// pour les dimensions donn�es
		// DONNEES_SPECIALES = &std::array<uint, 3U>{ TAILLE, LIGNES, COLONES }
		TAILLE_INVALIDE,
	};


public:
	// DONNEES_SPECIALES est utile pour ajouter des donn�es personalis�es
	MatriceException(const TYPES TYPE, const void *DONNEES_SPECIALES = nullptr)
		: TYPE(TYPE), DONNEES_SPECIALES(DONNEES_SPECIALES)
	{}


public:
	std::string what() const noexcept;


public:
	const TYPES TYPE;
	const void *DONNEES_SPECIALES;
};


// Dataset avec 1 dimension
class Matrice;
typedef std::vector<Matrice> Dataset;
// Dataset virtuel 
typedef std::vector<Matrice*> VDataset;


class Matrice
{
public:
	//////////////// Constructeurs Statiques ////////////////
	// Init � partir d'un fichier, la matrice est vide s'il y a une erreur
	static Matrice charger(const std::string &EMPLACEMENT);

	// Init en copiant la valeur pour chaque composant
	static Matrice avecValeur(const float VALEUR,
		const unsigned int LIGNES,
		const unsigned int COLONES);

	// Init en faisant appel � une fonction
	static Matrice avecFonction(float (*f)(),
		const unsigned int LIGNES,
		const unsigned int COLONES);

	// Init avec des valeurs al�atoires entre 0 et 1 (distribution purement al�atoire)
	static Matrice distribAleatoire(const unsigned int LIGNES, const unsigned int COLONES);

	// Init comme un vecteur colone
	static Matrice vecteurColone(const std::initializer_list<float> &DONNEES);

	// Init comme un vecteur ligne
	static Matrice vecteurLigne(const std::initializer_list<float> &DONNEES);

	// Cr�e un vecteur de matrices (vecteurs lignes)
	static Dataset datasetLigne(
		const std::initializer_list<float> &DONNEES,
		const unsigned int ELEMENTS_PAR_VECTEUR
	);

	// Cr�e un one hot encoded vector (vecteur colone)
	static Matrice oneHot(const unsigned int i, const unsigned int PROFONDEUR);


public:
	//////////////// Constructeurs Publiques ////////////////
	// Matrice vide
	// Ne pas l'utiliser car _donnees == nullptr
	Matrice()
		: _donnees(nullptr), _lignes(0U), _colones(0U)
	{}

	// Matrice r�serv�e (les donn�es sont inchang�es)
	// !!! Assigner les donn�es juste apr�s
	Matrice(const unsigned int LIGNES,
		const unsigned int COLONES);

	// Init avec std::initializer_list
	Matrice(const std::initializer_list<float> &DONNEES,
		const unsigned int LIGNES,
		const unsigned int COLONES);

	// Init avec std::vector
	Matrice(const std::vector<float> &DONNEES,
		const unsigned int LIGNES,
		const unsigned int COLONES);

	// Attention n'utiliser �a que dans des fonctions internes
	// !!! Ne v�rifie pas si les donn�es ont la bonne dimension
	Matrice(float *DONNEES,
		const unsigned int LIGNES,
		const unsigned int COLONES)
		: _donnees(DONNEES), _lignes(LIGNES), _colones(COLONES)
	{}

	// Copie
	Matrice(const Matrice &MAT);

	// Lib�ration
	~Matrice()
	{
		delete[] _donnees;
	}



public:
	//////////////// M�thodes ////////////////
	// Affiche la matrice dans stdout
	void afficher() const;
	void afficher(const std::string &FORMAT) const;


public:
	//////////////// Setters ////////////////
	// Change les dimensions de la matrice
	Matrice &dimensionner(const unsigned int LIGNES, const unsigned int COLONES);


public:
	//////////////// Getters ////////////////
	// Retourne le composant � la bonne position en v�rifiant
	float &en(const unsigned int LIGNE, const unsigned int COLONE);
	float en(const unsigned int LIGNE, const unsigned int COLONE) const;

	// Retourne la dimension
	inline unsigned int getLignes() const
	{
		return _lignes;
	}

	inline unsigned int getColones() const
	{
		return _colones;
	}

	// Si elle est vide ou pas
	inline bool estVide() const
	{
		return _lignes == 0U && _colones == 0U;
	}

	// Retourne les donn�es en �criture seulement
	inline const float *const getDonnees() const
	{
		return _donnees;
	}

	// Retourne l'argument maximum c'est-�-dire l'indice
	// de l'�l�ment le plus grand
	std::pair<size_t, size_t> argmax() const;


public:
	// Les op�rations forment une autre matrice
	//////////////// Op�rations ////////////////
	//// Divers ////
	Matrice& operator=(const Matrice& MAT);

	// Produit matriciel (A . B)
	Matrice prod(const Matrice &MAT) const;

	// Transposition
	Matrice transpose() const;

	// Mappage par fonction
	Matrice map(float(*f)(float val)) const;

	// Somme de tous les �l�ments
	float somme() const;

	//// Scalaires ////
	// Op�rations �l�ment par �l�ment
	Matrice operator+(const float VALEUR) const;
	Matrice operator-(const float VALEUR) const;
	Matrice operator*(const float VALEUR) const;
	Matrice operator/(const float VALEUR) const;

	//// Matrices ////
	Matrice operator+(const Matrice &MAT) const;
	Matrice operator-(const Matrice &MAT) const;
	Matrice operator*(const Matrice &MAT) const;
	Matrice operator/(const Matrice &MAT) const;
	

public:
	// Elles changent cette matrice
	//////////////// Op�rations relatives ////////////////
	//// Scalaires ////
	Matrice &operator+=(const float VALEUR);
	Matrice &operator-=(const float VALEUR);
	Matrice &operator*=(const float VALEUR);
	Matrice &operator/=(const float VALEUR);

	//// Matrices ////
	Matrice &operator+=(const Matrice &MAT);
	Matrice &operator-=(const Matrice &MAT);
	Matrice &operator*=(const Matrice &MAT);
	Matrice &operator/=(const Matrice &MAT);


private:
	//////////////// M�thodes Priv�es ////////////////
	// en() sans v�rification
	inline float &_en(const unsigned int LIGNE, const unsigned int COLONE)
	{
		return _donnees[LIGNE * _colones + COLONE];
	}

	inline float &_en(const unsigned int LIGNE, const unsigned int COLONE) const
	{
		return _donnees[LIGNE * _colones + COLONE];
	}


private:
	//////////////// Attributs Priv�s ////////////////
	// Donn�es
	float *_donnees;

	// Dimensions
	unsigned int _lignes,
		_colones;
};

