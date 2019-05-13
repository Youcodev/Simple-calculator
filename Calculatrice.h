#ifndef CALCULATRICE_H
#define CALCULATRICE_H

#include<QWidget>
#include<QMessageBox>
#include<QPushButton>
#include<QLineEdit>
#include<QGridLayout>
#include"Bouton.h"

class Calculatrice : public QWidget
{
    Q_OBJECT
public:
    Calculatrice(QWidget *parent = 0); // Constructeur

private slots:
    void digitClicked(); // Appelé quand une touche numérique est appuyée
    void unaryOperatorClicked(); // Appelé quand la touche carré, racine carrée ou inverse est appuyée
    void additiveOperatorClicked(); // Appelé quand la touche "+" ou "-" est appuyée
    void multiplicativeOperatorClicked(); // Appelé quand la touche "÷" ou "×" est appuyée
    void equalClicked(); // Appelé quand la touche "=" est appuyée
    void pointClicked(); // Appelé quand la touche "." est appuyée
    void changeSignClicked(); // Appelé quand la touche "±" est appuyée
    void backspaceClicked(); // Appelé quand la touche backspace est appuyée
    void clear(); // Appelé quand la touche Clear est appuyée
    void clearAll(); // Appelé quand la touche Clear All est appuyée
    void clearMemory(); // Appelé quand la touche MC est appuyée
    void readMemory(); // Appelé quand la touche MR est appuyée
    void setMemory(); // Appelé quand la touche MS est appuyée
    void addToMemory(); // Appelé quand la touche M+ est appuyée

private:
    Bouton *createButton(const QString &text, const char *member); // Appelée lors de la création d'un nouveau bouton
    void abortOperation(); // Appelée pour l'abandon d'une opération interdite (non valide)
    bool calculate(double rightOperand, const QString &spendingOperator); // Appelée lors du calcul d'une opération binaire
    double sumInMemory; // Valeur en mémoire de la calculatrice (accès à l'aide des touches MC, MS ou M+)
    double sumSoFar; // Résultat temporaire d'une opération d'addition/soustraction
    double factorSoFar; // Résultat temporaire d'une opération de multiplication/division
    QString pendingAdditiveOperator; // Opérateur d'addition ou de soustraction cliqué par l'utilisateur
    QString pendingMultiplicativeOperator; // Opérateur de multiplication ou de division cliqué par l'utilisateur
    bool waitingForOperand; // État de réception d'un opérande; True si la calculatrice s'attend à un opérand
    QLineEdit *display; // Zone de texte pour l'affichage des résultats de calcul
    
    enum {NumDigitButtons = 10};
    Bouton *digitButtons[NumDigitButtons]; // Touches numériques (0 à 9)
    QPushButton *m_boutonDialogue; // Bouton poussoir

};

#endif // CALCULATRICE_H
