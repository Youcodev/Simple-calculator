#ifndef ERREUR_H
#define ERREUR_H

#include <stdexcept>
using namespace std;

class ErreurDivisionParZero : public domain_error { // Erreur de domaine mathématique
public:
    ErreurDivisionParZero(string whatArg = "");
};
ErreurDivisionParZero::ErreurDivisionParZero(string whatArg):domain_error(whatArg) {}

class ErreurInverseDeZero : public domain_error { // Erreur de domaine mathématique
public:
    ErreurInverseDeZero(string whatArg = "");
};
ErreurInverseDeZero::ErreurInverseDeZero(string whatArg):domain_error(whatArg) {}

class ErreurRacineNombreNegatif : public domain_error { // Erreur de domaine mathématique
public:
    ErreurRacineNombreNegatif(string whatArg = "");
};
ErreurRacineNombreNegatif::ErreurRacineNombreNegatif(string whatArg):domain_error(whatArg) {}

#endif // ERREUR_H
