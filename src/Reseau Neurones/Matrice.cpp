#include "Matrice.h"

#include <iostream>
#include <array>

#include "fichiers.h"



std::string MatriceException::what() const noexcept
{
	switch (TYPE)
	{
	case TYPES::SPECIALE:
		return *static_cast<const std::string*>(DONNEES_SPECIALES);

	case TYPES::DIMENSIONS_NON_ALIGNEES:
	{
		const std::array<unsigned int, 4U> DONNEES =
			*reinterpret_cast<const std::array<unsigned int, 4U>*>(
				DONNEES_SPECIALES
				);

		std::string msg = "Exception: Dimensions non alignees\n"
			"Les deux dimensions ("
			+ std::to_string(DONNEES[0U]) +
			", "
			+ std::to_string(DONNEES[1U]) +
			") et ("
			+ std::to_string(DONNEES[2U]) +
			", "
			+ std::to_string(DONNEES[3U]) +
			") sont invalides pour cette operation";

		return msg;
	}

	case TYPES::ELEMENT_DEHORS:
	{
		const std::array<unsigned int, 4U> DONNEES =
			*reinterpret_cast<const std::array<unsigned int, 4U>*>(
				DONNEES_SPECIALES
				);

		std::string msg = "Exception: Element en dehors\n"
			"L'element desire a l'indice ("
			+ std::to_string(DONNEES[0U]) +
			", "
			+ std::to_string(DONNEES[1U]) +
			") est en dehors de la matrice de dimension ("
			+ std::to_string(DONNEES[2U]) +
			", "
			+ std::to_string(DONNEES[3U]) +
			")";

		return msg;
	}

	case TYPES::TAILLE_INVALIDE:
	{
		const std::array<unsigned int, 4U> DONNEES =
			*reinterpret_cast<const std::array<unsigned int, 4U>*>(
				DONNEES_SPECIALES
				);

		std::string msg = "Exception: Taille invalide\n"
			"Les donnees fournies ne contiennent pas assez / trop d'elements ("
			+ std::to_string(DONNEES[0U]) +
			") pour la taille donnee ("
			+ std::to_string(DONNEES[1U]) +
			", "
			+ std::to_string(DONNEES[2U]) +
			"), la taille doit etre "
			+ std::to_string(DONNEES[1U] * DONNEES[2U]);

		return msg;
	}

	}

	return "Unknown exception";
}


Matrice Matrice::charger(const std::string & EMPLACEMENT)
{
	return io::chargerMatrice(EMPLACEMENT);
}

Matrice Matrice::avecValeur(const float VALEUR, const unsigned int LIGNES, const unsigned int COLONES)
{
	// Taille à allouer
	const size_t TAILLE = LIGNES * COLONES;

	// Allocation
	float *donnees = new float[TAILLE];

	// Copie de VALEUR pour chaque élément
	std::fill(&donnees[0U], &donnees[TAILLE], VALEUR);

	// Création de la matrice
	return Matrice(donnees, LIGNES, COLONES);
}

Matrice Matrice::avecFonction(float(*f)(), const unsigned int LIGNES, const unsigned int COLONES)
{
	// Taille à allouer
	const size_t TAILLE = LIGNES * COLONES;

	// Allocation
	float *donnees = new float[TAILLE];

	// Distribution
	for (unsigned int i = 0U; i < TAILLE; i++)
		donnees[i] = f();

	// Création de la matrice
	return Matrice(donnees, LIGNES, COLONES);
}

Matrice Matrice::distribAleatoire(const unsigned int LIGNES, const unsigned int COLONES)
{
	// Taille à allouer
	const size_t TAILLE = LIGNES * COLONES;

	// Allocation
	float *donnees = new float[TAILLE];

	// Distribution
	for (unsigned int i = 0U; i < TAILLE; i++)
		donnees[i] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

	// Création de la matrice
	return Matrice(donnees, LIGNES, COLONES);
}

Matrice Matrice::vecteurColone(const std::initializer_list<float>& DONNEES)
{
	const unsigned int TAILLE = DONNEES.size();

	// Copie //
	float *matDonnees = new float[TAILLE];
	std::copy(DONNEES.begin(), DONNEES.end(), matDonnees);

	return Matrice(matDonnees, TAILLE, 1U);
}

Matrice Matrice::vecteurLigne(const std::initializer_list<float>& DONNEES)
{
	const unsigned int TAILLE = DONNEES.size();

	// Copie //
	float *matDonnees = new float[TAILLE];
	std::copy(DONNEES.begin(), DONNEES.end(), matDonnees);

	return Matrice(matDonnees, 1U, TAILLE);
}

Dataset Matrice::datasetLigne(const std::initializer_list<float>& DONNEES, const unsigned int ELEMENTS_PAR_VECTEUR)
{
	// Il n'y a pas assez / trop d'éléments donnés
	if (DONNEES.size() % ELEMENTS_PAR_VECTEUR != 0U)
		throw MatriceException(MatriceException::TAILLE_INVALIDE,
			reinterpret_cast<const void*>(&std::array<unsigned int, 3U>(
				{ DONNEES.size() % ELEMENTS_PAR_VECTEUR, ELEMENTS_PAR_VECTEUR, 1U })));

	std::vector<Matrice> dataset;

	const unsigned int TAILLE = DONNEES.size();

	dataset.reserve(TAILLE / ELEMENTS_PAR_VECTEUR);

	for (unsigned int s = 0U; s < TAILLE; s += ELEMENTS_PAR_VECTEUR)
		dataset.emplace_back(Matrice(
			std::vector<float>(DONNEES.begin() + s, DONNEES.begin() + s + ELEMENTS_PAR_VECTEUR),
			1U, ELEMENTS_PAR_VECTEUR
		));

	return dataset;
}

Matrice Matrice::oneHot(const unsigned int i, const unsigned int PROFONDEUR)
{
	Matrice mat = Matrice::avecValeur(0.F, 1U, PROFONDEUR);

	mat._en(0U, i) = 1.F;

	return mat;
}

Matrice::Matrice(const unsigned int LIGNES, const unsigned int COLONES)
	: _lignes(LIGNES), _colones(COLONES)
{
	_donnees = new float[LIGNES * COLONES];
}

Matrice::Matrice(const std::initializer_list<float>& DONNEES, const unsigned int LIGNES, const unsigned int COLONES)
	: _lignes(LIGNES), _colones(COLONES)
{
	// Vérification //
	// Les données sont de mauvaise taille
	if (_lignes * _colones != DONNEES.size())
		throw MatriceException(MatriceException::TAILLE_INVALIDE,
			reinterpret_cast<const void*>(&std::array<unsigned int, 3U>(
				{ DONNEES.size(), LIGNES, COLONES })));

	// Copie //
	_donnees = new float[LIGNES * COLONES];
	std::copy(DONNEES.begin(), DONNEES.end(), _donnees);
}

Matrice::Matrice(const std::vector<float>& DONNEES, const unsigned int LIGNES, const unsigned int COLONES)
	: _lignes(LIGNES), _colones(COLONES)
{
	// Vérification //
	// Les données sont de mauvaise taille
	if (_lignes * _colones != DONNEES.size())
		throw MatriceException(MatriceException::TAILLE_INVALIDE,
			reinterpret_cast<const void*>(&std::array<unsigned int, 3U>(
				{ DONNEES.size(), LIGNES, COLONES })));

	// Copie //
	_donnees = new float[LIGNES * COLONES];
	std::copy(DONNEES.begin(), DONNEES.end(), _donnees);
}

Matrice::Matrice(const Matrice & MAT)
{
	// Dimensions
	_lignes = MAT._lignes;
	_colones = MAT._colones;

	// Allocation
	_donnees = new float[_lignes * _colones];

	// Copie
	std::copy(MAT._donnees, &MAT._donnees[MAT._lignes * MAT._colones], _donnees);
}

void Matrice::afficher() const
{
	std::string sortie = "{\n";

	for (unsigned int y = 0U; y < _lignes; y++)
	{
		sortie += '\t';

		for (unsigned int x = 0U; x < _colones; x++)
			sortie += std::to_string(_donnees[y * _colones + x]) + ", ";

		sortie += '\n';
	}

	std::cout << sortie + "}\n";
}

void Matrice::afficher(const std::string& FORMAT) const
{
	std::string sortie = "{\n";

	for (unsigned int y = 0U; y < _lignes; y++)
	{
		sortie += '\t';

		for (unsigned int x = 0U; x < _colones; x++)
		{
			char *nb = new char[16U];

			// Format
			sprintf_s(&nb[0U], 16U, FORMAT.c_str(), _donnees[y * _colones + x]);
			
			sortie += nb;
			sortie += ", ";
		}

		sortie += '\n';
	}

	std::cout << sortie + "}\n";
}

Matrice & Matrice::dimensionner(const unsigned int LIGNES, const unsigned int COLONES)
{
	// Vérification //
	// La nouvelle dimension ne va pas avec l'ancienne
	if (_lignes * _colones != LIGNES * COLONES)
		throw MatriceException(MatriceException::DIMENSIONS_NON_ALIGNEES,
			reinterpret_cast<const void*>(&std::array<unsigned int, 4U>(
				{ _lignes, _colones, LIGNES, COLONES })));

	_lignes = LIGNES;
	_colones = COLONES;

	return *this;
}

float & Matrice::en(const unsigned int LIGNE, const unsigned int COLONE)
{
	// Test si x et y sont dans la matrice
	if (LIGNE >= _lignes || COLONE >= _colones)
		throw MatriceException(MatriceException::ELEMENT_DEHORS,
			reinterpret_cast<const void*>(&std::array<unsigned int, 4U>(
				{ LIGNE, COLONE, _lignes, _colones })));

	return _donnees[LIGNE * _colones + COLONE];
}

float Matrice::en(const unsigned int LIGNE, const unsigned int COLONE) const
{
	// Test si x et y sont dans la matrice
	if (LIGNE >= _lignes || COLONE >= _colones)
		throw MatriceException(MatriceException::ELEMENT_DEHORS,
			reinterpret_cast<const void*>(&std::array<unsigned int, 4U>(
				{ LIGNE, COLONE, _lignes, _colones })));

	return _donnees[LIGNE * _colones + COLONE];
}

std::pair<size_t, size_t> Matrice::argmax() const
{
	std::pair<size_t, size_t> amax = { 0U, 0U };

	for (unsigned int y = 0U; y < _lignes; y++)
		// On commence à 1 car on dit au début que le max est en (0, 0)
		for (unsigned int x = 1U; x < _colones; x++)
			if (_en(amax.first, amax.second) < _en(y, x))
				amax = { y, x };

	return amax;
}

Matrice & Matrice::operator=(const Matrice & MAT)
{
	// Dimensions
	_lignes = MAT._lignes;
	_colones = MAT._colones;

	// Allocation des données
	float *const newData = new float[MAT._lignes * MAT._colones];

	// Copie
	std::copy(MAT._donnees, &MAT._donnees[MAT._lignes * MAT._colones], newData);

	// Libération des anciennes données
	delete[] _donnees;

	// Modification du pointeur
	_donnees = newData;

	return *this;
}

Matrice Matrice::prod(const Matrice & MAT) const
{
	// Vérification //
	// Les dimensions des matrices ne sont pas alignées
	if (_colones != MAT._lignes)
		throw MatriceException(MatriceException::DIMENSIONS_NON_ALIGNEES,
			reinterpret_cast<const void*>(&std::array<unsigned int, 4U>(
				{ _lignes, _colones, MAT._lignes, MAT._colones })));

	// Dimensions du produit
	const unsigned int PROD_LIGNES = _lignes,
		PROD_COLONES = MAT._colones;

	// Allocation
	float *prodDonnees = new float[PROD_LIGNES * PROD_COLONES];

	// Produit //
	// Depuis wikipedia:
	// C(i, j) = somme(n, k=1){ a(i, k) * b(k, j) } avec A € (m, n); B € (n, p) ; C € (m, p) 
	// A est *this, B est MAT
	for (unsigned int i = 0U; i < PROD_LIGNES; i++)
		for (unsigned int j = 0U; j < PROD_COLONES; j++)
		{
			float somme = 0.F;

			for (unsigned int k = 0U; k < _colones; k++)
				somme += _en(i, k) * MAT._en(k, j);

			prodDonnees[i * PROD_COLONES + j] = somme;
		}

	return Matrice(prodDonnees, PROD_LIGNES, PROD_COLONES);
}

Matrice Matrice::transpose() const
{
	// Nouvelles données
	float *matData = new float[_lignes * _colones];

	// Changement de places des composants
	for (unsigned int y = 0U; y < _lignes; y++)
		for (unsigned int x = 0U; x < _colones; x++)
			matData[x * _lignes + y] = _donnees[y * _colones + x];

	// Retour de la matrice transposée
	return Matrice(matData, _colones, _lignes);
}

Matrice Matrice::map(float(*f)(float val)) const
{
	// Allocation //
	const size_t TAILLE = _lignes * _colones;
	float *nouvellesDonnees = new float[TAILLE];

	// Calculs //
	for (size_t i = 0U; i < TAILLE; i++)
		nouvellesDonnees[i] = f(_donnees[i]);

	return Matrice(nouvellesDonnees, _lignes, _colones);
}

float Matrice::somme() const
{
	float somme = 0.F;

	for (unsigned int i = 0U; i < _lignes * _colones; i++)
		somme += _donnees[i];

	return somme;
}

Matrice Matrice::operator+(const float VALEUR) const
{
	// Allocation de la matrice
	Matrice mat(*this);

	for (unsigned int r = 0U; r < _lignes; r++)
		for (unsigned int c = 0U; c < _colones; c++)
			mat._en(r, c) += VALEUR;

	return mat;
}

Matrice Matrice::operator-(const float VALEUR) const
{
	// Allocation de la matrice
	Matrice mat(*this);

	for (unsigned int r = 0U; r < _lignes; r++)
		for (unsigned int c = 0U; c < _colones; c++)
			mat._en(r, c) -= VALEUR;

	return mat;
}

Matrice Matrice::operator*(const float VALEUR) const
{
	// Allocation de la matrice
	Matrice mat(*this);

	for (unsigned int r = 0U; r < _lignes; r++)
		for (unsigned int c = 0U; c < _colones; c++)
			mat._en(r, c) *= VALEUR;

	return mat;
}

Matrice Matrice::operator/(const float VALEUR) const
{
	// Allocation de la matrice
	Matrice mat(*this);

	for (unsigned int r = 0U; r < _lignes; r++)
		for (unsigned int c = 0U; c < _colones; c++)
			mat._en(r, c) /= VALEUR;

	return mat;
}

Matrice Matrice::operator+(const Matrice & MAT) const
{
	// Vérification //
	// Les matrices doivent avoir les mêmes dimensions
	if (_colones != MAT._colones || _lignes != MAT._lignes)
		throw MatriceException(MatriceException::DIMENSIONS_NON_ALIGNEES,
			reinterpret_cast<const void*>(&std::array<unsigned int, 4U>(
				{ _lignes, _colones, MAT._lignes, MAT._colones })));

	// Allocation //
	const size_t TAILLE = _lignes * _colones;
	float *nouvellesDonnees = new float[TAILLE];

	// Calculs
	for (size_t i = 0U; i < TAILLE; i++)
		nouvellesDonnees[i] = _donnees[i] + MAT._donnees[i];

	return Matrice(nouvellesDonnees, _lignes, _colones);
}

Matrice Matrice::operator-(const Matrice & MAT) const
{
	// Vérification //
	// Les matrices doivent avoir les mêmes dimensions
	if (_colones != MAT._colones || _lignes != MAT._lignes)
		throw MatriceException(MatriceException::DIMENSIONS_NON_ALIGNEES,
			reinterpret_cast<const void*>(&std::array<unsigned int, 4U>(
				{ _lignes, _colones, MAT._lignes, MAT._colones })));

	// Allocation //
	const size_t TAILLE = _lignes * _colones;
	float *nouvellesDonnees = new float[TAILLE];

	// Calculs
	for (size_t i = 0U; i < TAILLE; i++)
		nouvellesDonnees[i] = _donnees[i] - MAT._donnees[i];

	return Matrice(nouvellesDonnees, _lignes, _colones);
}

Matrice Matrice::operator*(const Matrice & MAT) const
{
	// Vérification //
	// Les matrices doivent avoir les mêmes dimensions
	if (_colones != MAT._colones || _lignes != MAT._lignes)
		throw MatriceException(MatriceException::DIMENSIONS_NON_ALIGNEES,
			reinterpret_cast<const void*>(&std::array<unsigned int, 4U>(
				{ _lignes, _colones, MAT._lignes, MAT._colones })));

	// Allocation //
	const size_t TAILLE = _lignes * _colones;
	float *nouvellesDonnees = new float[TAILLE];

	// Calculs
	for (size_t i = 0U; i < TAILLE; i++)
		nouvellesDonnees[i] = _donnees[i] * MAT._donnees[i];

	return Matrice(nouvellesDonnees, _lignes, _colones);
}

Matrice Matrice::operator/(const Matrice & MAT) const
{
	// Vérification //
	// Les matrices doivent avoir les mêmes dimensions
	if (_colones != MAT._colones || _lignes != MAT._lignes)
		throw MatriceException(MatriceException::DIMENSIONS_NON_ALIGNEES,
			reinterpret_cast<const void*>(&std::array<unsigned int, 4U>(
				{ _lignes, _colones, MAT._lignes, MAT._colones })));

	// Allocation //
	const size_t TAILLE = _lignes * _colones;
	float *nouvellesDonnees = new float[TAILLE];

	// Calculs
	for (size_t i = 0U; i < TAILLE; i++)
		nouvellesDonnees[i] = _donnees[i] / MAT._donnees[i];

	return Matrice(nouvellesDonnees, _lignes, _colones);
}

Matrice & Matrice::operator+=(const float VALEUR)
{
	const unsigned int TAILLE = _lignes * _colones;
	for (unsigned int i = 0U; i < TAILLE; i++)
		_donnees[i] += VALEUR;

	return *this;
}

Matrice & Matrice::operator-=(const float VALEUR)
{
	const unsigned int TAILLE = _lignes * _colones;
	for (unsigned int i = 0U; i < TAILLE; i++)
		_donnees[i] -= VALEUR;

	return *this;
}

Matrice & Matrice::operator*=(const float VALEUR)
{
	const unsigned int TAILLE = _lignes * _colones;
	for (unsigned int i = 0U; i < TAILLE; i++)
		_donnees[i] *= VALEUR;

	return *this;
}

Matrice & Matrice::operator/=(const float VALEUR)
{
	const unsigned int TAILLE = _lignes * _colones;
	for (unsigned int i = 0U; i < TAILLE; i++)
		_donnees[i] /= VALEUR;

	return *this;
}

Matrice & Matrice::operator+=(const Matrice & MAT)
{
	// Vérification //
	// Les matrices doivent avoir les mêmes dimensions
	if (_colones != MAT._colones || _lignes != MAT._lignes)
		throw MatriceException(MatriceException::DIMENSIONS_NON_ALIGNEES,
			reinterpret_cast<const void*>(&std::array<unsigned int, 4U>(
				{ _lignes, _colones, MAT._lignes, MAT._colones })));

	// Calcul //
	const unsigned int TAILLE = _lignes * _colones;
	for (unsigned int i = 0U; i < TAILLE; i++)
		_donnees[i] += MAT._donnees[i];

	return *this;
}

Matrice & Matrice::operator-=(const Matrice & MAT)
{
	// Vérification //
	// Les matrices doivent avoir les mêmes dimensions
	if (_colones != MAT._colones || _lignes != MAT._lignes)
		throw MatriceException(MatriceException::DIMENSIONS_NON_ALIGNEES,
			reinterpret_cast<const void*>(&std::array<unsigned int, 4U>(
				{ _lignes, _colones, MAT._lignes, MAT._colones })));

	// Calcul //
	const unsigned int TAILLE = _lignes * _colones;
	for (unsigned int i = 0U; i < TAILLE; i++)
		_donnees[i] -= MAT._donnees[i];

	return *this;
}

Matrice & Matrice::operator*=(const Matrice & MAT)
{
	// Vérification //
	// Les matrices doivent avoir les mêmes dimensions
	if (_colones != MAT._colones || _lignes != MAT._lignes)
		throw MatriceException(MatriceException::DIMENSIONS_NON_ALIGNEES,
			reinterpret_cast<const void*>(&std::array<unsigned int, 4U>(
				{ _lignes, _colones, MAT._lignes, MAT._colones })));

	// Calcul //
	const unsigned int TAILLE = _lignes * _colones;
	for (unsigned int i = 0U; i < TAILLE; i++)
		_donnees[i] *= MAT._donnees[i];

	return *this;
}

Matrice & Matrice::operator/=(const Matrice & MAT)
{
	// Vérification //
	// Les matrices doivent avoir les mêmes dimensions
	if (_colones != MAT._colones || _lignes != MAT._lignes)
		throw MatriceException(MatriceException::DIMENSIONS_NON_ALIGNEES,
			reinterpret_cast<const void*>(&std::array<unsigned int, 4U>(
				{ _lignes, _colones, MAT._lignes, MAT._colones })));

	// Calcul //
	const unsigned int TAILLE = _lignes * _colones;
	for (unsigned int i = 0U; i < TAILLE; i++)
		_donnees[i] /= MAT._donnees[i];

	return *this;
}

