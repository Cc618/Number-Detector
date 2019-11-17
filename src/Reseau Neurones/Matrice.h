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
		// DONNEES_SPECIALES = &std::string, texte à afficher
		SPECIALE,

		// Les deux dimensions ne sont pas alignées (dimensions incorrectes)
		// DONNEES_SPECIALES = &std::array<uint, 4U>{ A_LIGNES, A_COLONES, B_LIGNES, B_COLONES }
		DIMENSIONS_NON_ALIGNEES,

		// L'élément désiré est à l'extérieur de la matrice
		// DONNEES_SPECIALES = &std::array<uint, 4U>{ LIGNE, COLONE, MAX_LIGNES, MAX_COLONES }
		ELEMENT_DEHORS,

		// Les données fournies ne comportent pas assez / trop d'élément
		// pour les dimensions données
		// DONNEES_SPECIALES = &std::array<uint, 3U>{ TAILLE, LIGNES, COLONES }
		TAILLE_INVALIDE,
	};


public:
	// DONNEES_SPECIALES est utile pour ajouter des données personalisées
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
	// Init à partir d'un fichier, la matrice est vide s'il y a une erreur
	static Matrice charger(const std::string &EMPLACEMENT);

	// Init en copiant la valeur pour chaque composant
	static Matrice avecValeur(const float VALEUR,
		const unsigned int LIGNES,
		const unsigned int COLONES);

	// Init en faisant appel à une fonction
	static Matrice avecFonction(float (*f)(),
		const unsigned int LIGNES,
		const unsigned int COLONES);

	// Init avec des valeurs aléatoires entre 0 et 1 (distribution purement aléatoire)
	static Matrice distribAleatoire(const unsigned int LIGNES, const unsigned int COLONES);

	// Init comme un vecteur colone
	static Matrice vecteurColone(const std::initializer_list<float> &DONNEES);

	// Init comme un vecteur ligne
	static Matrice vecteurLigne(const std::initializer_list<float> &DONNEES);

	// Crée un vecteur de matrices (vecteurs lignes)
	static Dataset datasetLigne(
		const std::initializer_list<float> &DONNEES,
		const unsigned int ELEMENTS_PAR_VECTEUR
	);

	// Crée un one hot encoded vector (vecteur colone)
	static Matrice oneHot(const unsigned int i, const unsigned int PROFONDEUR);


public:
	//////////////// Constructeurs Publiques ////////////////
	// Matrice vide
	// Ne pas l'utiliser car _donnees == nullptr
	Matrice()
		: _donnees(nullptr), _lignes(0U), _colones(0U)
	{}

	// Matrice réservée (les données sont inchangées)
	// !!! Assigner les données juste après
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

	// Attention n'utiliser ça que dans des fonctions internes
	// !!! Ne vérifie pas si les données ont la bonne dimension
	Matrice(float *DONNEES,
		const unsigned int LIGNES,
		const unsigned int COLONES)
		: _donnees(DONNEES), _lignes(LIGNES), _colones(COLONES)
	{}

	// Copie
	Matrice(const Matrice &MAT);

	// Libération
	~Matrice()
	{
		delete[] _donnees;
	}



public:
	//////////////// Méthodes ////////////////
	// Affiche la matrice dans stdout
	void afficher() const;
	void afficher(const std::string &FORMAT) const;


public:
	//////////////// Setters ////////////////
	// Change les dimensions de la matrice
	Matrice &dimensionner(const unsigned int LIGNES, const unsigned int COLONES);


public:
	//////////////// Getters ////////////////
	// Retourne le composant à la bonne position en vérifiant
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

	// Retourne les données en écriture seulement
	inline const float *const getDonnees() const
	{
		return _donnees;
	}

	// Retourne l'argument maximum c'est-à-dire l'indice
	// de l'élément le plus grand
	std::pair<size_t, size_t> argmax() const;


public:
	// Les opérations forment une autre matrice
	//////////////// Opérations ////////////////
	//// Divers ////
	Matrice& operator=(const Matrice& MAT);

	// Produit matriciel (A . B)
	Matrice prod(const Matrice &MAT) const;

	// Transposition
	Matrice transpose() const;

	// Mappage par fonction
	Matrice map(float(*f)(float val)) const;

	// Somme de tous les éléments
	float somme() const;

	//// Scalaires ////
	// Opérations élément par élément
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
	//////////////// Opérations relatives ////////////////
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
	//////////////// Méthodes Privées ////////////////
	// en() sans vérification
	inline float &_en(const unsigned int LIGNE, const unsigned int COLONE)
	{
		return _donnees[LIGNE * _colones + COLONE];
	}

	inline float &_en(const unsigned int LIGNE, const unsigned int COLONE) const
	{
		return _donnees[LIGNE * _colones + COLONE];
	}


private:
	//////////////// Attributs Privés ////////////////
	// Données
	float *_donnees;

	// Dimensions
	unsigned int _lignes,
		_colones;
};

