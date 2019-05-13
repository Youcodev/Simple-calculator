/**********************************************************************************************************
 * La calculatrice implémentée ici ressemble à celle sous windows dans sa forme standard, i.e. elle ne
 * tient pas compte de l'ordre des opérations.
 * Lors d'une opération binaire, et pour une meilleure visibilité de celle-ci, la calculatrice affiche
 * le premier opérande suivi de l'opérateur et puis du second opérande. Si à la suite du second opérande,
 * la touche "=" est appuyée, le résultat de l'opération est affiché. Si à la place, une autre touche est
 * appuyée(addition, soustraction, multiplication ou division), l'opérateur "=" est automatiquemet appelé.
 * Si en revanche, un opérateur unaire est appuyé, un message d'erreur est affiché invitant l'utilisateur
 * à terminer d'abord l'opération en cours. La gestion d'exceptions dans le cas d'une division par zéro ou
 * de la racine carrée d'un nombre négatif est également prise en compte.
 * Réalisé sous Qt 5.8.0 et les fichiers encodés en UTF-8
 **********************************************************************************************************/

#include<QtGui>
#include<math.h>
#include"Calculatrice.h"
#include"Erreur.h"

// Implémentation du constructeur
Calculatrice::Calculatrice(QWidget *parent): QWidget(parent)
{
    sumInMemory = 0.0; // Initialisation de la valeur en mémoire de la calculatrice
    sumSoFar = 0.0; // Initialisation de la somme temporaire lors des opérations d'addition/soustraction
    factorSoFar = 0.0; // Initialisation du produit temporaire lors des opérations de multiplication/division
    waitingForOperand = true; // Calculatrice prête à recevoir un opérande de l'utilisateur

    // Création d'un objet de type QLineEdit avec par défaut la chaine de caractère 0
    // Cet objet représente une zone de texte qui sera utilisée comme afficheur de la calculatrice
    display = new QLineEdit("0");

    // la zone de texte est paramétrée en lecture seule
    display->setReadOnly(true);

    // l'affichage se fera de la droite vers la gauche
    display->setAlignment(Qt::AlignRight);

    // définition de la largeur de la zone de texte à 15
    display->setMaxLength(15);

    // Création d'un objet de type QFont avec la police par défaut de l'application
    // Cet objet représente la police d'affichage de la zone de texte
    QFont font = display->font();
    // Augmentation de 8 points de la taille de la police
    font.setPointSize(font.pointSize() + 8);
    // Appliquer ce changement de taille à la zone de texte
    display->setFont(font);

    //MaFenetre  fenetre1;
    for(int i=0; i<NumDigitButtons; ++i)
    {
        // Création des touches numériques (de 0 à 9)
        digitButtons[i] = createButton(QString::number(i), SLOT(digitClicked()));
    }
    // Création de la touche "point" pour les nombres décimaux
    Bouton *pointButton = createButton(tr("."), SLOT(pointClicked()));
    // Création de la touche "plus ou moins" pour changer le signe d'un nombre
    // "\302\261" étant le code octal du signe "±" en UTF-8
    Bouton *changeSignButton = createButton(tr("\302\261"), SLOT(changeSignClicked()));
    // Création de la touche "backspace" pour effacer le dernier chiffre rentré par l'utilisateur
    // Touche équivalente à celle de la "flèche vers la gauche" d'une calculatrice Windows
    Bouton *backspaceButton = createButton(tr("Backspace"), SLOT(backspaceClicked()));
    // Création de la touche "Clear" pour effacer le dernier nombre rentré par l'utilisateur
    // Touche équivalente à CE (Clear Entry) d'une calculatrice Windows
    Bouton *clearButton = createButton(tr("Clear"), SLOT(clear()));
    // Création de la touche "Clear All" pour tout effacer
    // Touche équivalente à C (Clear) d'une calculatrice Windows
    Bouton *clearAllButton = createButton(tr("Clear All"), SLOT(clearAll()));

   // Créez les boutons manquants ici
   // Création de la touche "MC" pour effacer la mémoire (la mettre à 0)
   Bouton *clearMemoryButton = createButton(tr("MC"), SLOT(clearMemory()));
   // Création de la touche "MR" pour appeler le nombre actuellement en mémoire
   Bouton *readMemoryButton = createButton(tr("MR"), SLOT(readMemory()));
   // Création de la touche "MS" pour stocker en mémoire le nombre affiché
   Bouton *setMemoryButton = createButton(tr("MS"), SLOT(setMemory()));
   // Création de la touche "M+" pour ajouter à la mémoire le nombre affiché
   Bouton *addToMemoryButton = createButton(tr("M+"), SLOT(addToMemory()));

    // Création de la touche "division"
    // "\303\267" étant le code octal du signe "÷" en UTF-8
    Bouton *divisionButton = createButton(tr("\303\267"), SLOT(multiplicativeOperatorClicked()));
    // Création de la touche "multiplication"
    // "\303\227" étant le code octal du signe "×" en UTF-8
    Bouton *timesButton = createButton(tr("\303\227"), SLOT(multiplicativeOperatorClicked()));
    // Création de la touche "soustraction"
    Bouton *minusButton = createButton(tr("-"), SLOT(additiveOperatorClicked()));
    // Création de la touche "addition"
    Bouton *plusButton = createButton(tr("+"), SLOT(additiveOperatorClicked()));
    // Création de la touche "racine carrée"
    Bouton *squareRootButton = createButton(tr("Sqrt"), SLOT(unaryOperatorClicked()));
    // Création de la touche "carré"
    // "\302\261" étant le code octal du carré (exposant 2) en UTF-8
    Bouton *powerButton = createButton(tr("x\302\262"), SLOT(unaryOperatorClicked()));
    // Création de la touche "inverse"
    Bouton *reciprocalButton = createButton(tr("1/x"), SLOT(unaryOperatorClicked()));
    // Création de la touche "égal"
    Bouton *equalButton = createButton(tr("="), SLOT(equalClicked()));

    //Déclarez votre layout ici
    // Création d'un objet de type QGridLayout (qui sera ajouté à la fenêtre principale)
    // Cet objet représente une grille de cellules à deux dimensions pour disposer les widgets (boutons et afficheur)
    QGridLayout *mainLayout = new QGridLayout;

    //Gerer le positionnement des boutons sur le layout ici
    // Pour ajouter un widget à une position donnée de la grille, on fait appel à la méthode addWidget()
    // qui prend en paramètre le widget en question et les coordonnées (ligne, colonne) où le placer sur la grille
    // Dans le cas où le widget occupe plusieurs cases, on fait appel à la version surchargée de la fonction addWiget()
    // qui prend deux paramètres supplémentaires, le nombre de lignes et le nombre de colonnes qu'occupe le widget

    // L'afficheur est placé tout en haut de la fenêtre et occupe six cases en largeur
    mainLayout->addWidget(display, 0, 0 , 1, 6);
    // Les touches numériques 0 -> 9 occupent les cellules (5,1) -> (2, 3)
    for(int i=0; i<NumDigitButtons; ++i)
    {
    // Comme on a 3 cellules successives par ligne, on peut utiliser la division entière par 3
    // pour déterminer la position de la ligne dans la grille
    // On commence par 15/3 pour la touche 0 et on décrémente cette position par ensemble de trois touches successives
        int ligne = (15-i)/3;

    // Comme les colonnes passent successivement de la position 1 à 3 pour chaque ligne, on peut utiliser le reste de division par 3
    // pour déterminer la position de chaque colonne dans la grille. Le dernier terme de l'équation permet de tenir compte de la touche 0
        int colonne = (i-1)%3 + 1 + (9-i)/9;

    // On ajoute chaque touche à la cellule(ligne, colonne) de la grille
        mainLayout->addWidget(digitButtons[i], ligne, colonne);
    }
    // La touche "point" occupe la cellule (5, 2)
    mainLayout->addWidget(pointButton, 5, 2);
    // La touche "plus ou moins" occupe la cellule (5, 3)
    mainLayout->addWidget(changeSignButton, 5, 3);
    // La touche "backspace" occupe deux cases de largeur et se situe sous l'afficheur du côté gauche
    mainLayout->addWidget(backspaceButton, 1, 0, 1, 2);
    // La touche "Clear" occupe deux cases de largeur et se situe à droite de la touche backspace
    mainLayout->addWidget(clearButton, 1, 2, 1, 2);
    // La touche "Clear All" occupe deux cases de largeur et se situe à droite de la touche Clear
    mainLayout->addWidget(clearAllButton, 1, 4, 1, 2);
    // La touche "MC" occupe la cellule (2, 0)
    mainLayout->addWidget(clearMemoryButton, 2, 0);
    // La touche "MR" occupe la cellule (3, 0)
    mainLayout->addWidget(readMemoryButton, 3, 0);
    // La touche "MS" occupe la cellule (4, 0)
    mainLayout->addWidget(setMemoryButton, 4, 0);
    // La touche "M+" occupe la cellule (5, 0)
    mainLayout->addWidget(addToMemoryButton, 5, 0);
    // La touche "division" occupe la cellule (2, 4)
    mainLayout->addWidget(divisionButton, 2, 4);
    // La touche "multiplication" occupe la cellule (3, 4)
    mainLayout->addWidget(timesButton, 3, 4);
    // La touche "sosutraction" occupe la cellule (4, 4)
    mainLayout->addWidget(minusButton, 4, 4);
    // La touche "addition" occupe la cellule (5, 4)
    mainLayout->addWidget(plusButton, 5, 4);
    // La touche "racine carrée" occupe la cellule (2, 5)
    mainLayout->addWidget(squareRootButton, 2, 5);
    // La touche "carré" occupe la cellule (3, 5)
    mainLayout->addWidget(powerButton, 3, 5);
    // La touche "inverse" occupe la cellule (4, 5)
    mainLayout->addWidget(reciprocalButton, 4, 5);
    // La touche "égal" occupe la cellule (5, 5)
    mainLayout->addWidget(equalButton, 5, 5);

    // Ajout du layout à la fenetre principale
        setLayout(mainLayout);
    // Ajout du titre de la  fenetre
        setWindowTitle(tr("Calculatrice"));
}

/**********************************************************************
 * Description      : Affiche la valeur de la touche numérique cliquée
 * Paramètres       : Aucun
 * Type de retour   : void
 **********************************************************************/
void Calculatrice::digitClicked()
{
    // Détermination de la touche cliquée à l'aide de la méthode send() de QObject
    // Comme c'est un bouton qui envoie le signal alors que la méthode renvoie un pointeur vers un QObject, on utilise qobject_cast
    Bouton *clickedButton = qobject_cast<Bouton* >(sender());
    int digitValue = clickedButton->text().toInt();
    // Si l'afficheur contient "0" et l'utilisateur clique sur la touche "0", on sort de la fonction
    // pour ne pas afficher deux zéros à la suite
    if(display->text() =="0" && digitValue == 0.0)
        return;

    // Si la calculatrice est prête à recevoir un opérande
    if(waitingForOperand)
    {
        // S'il s'agit du premier opérande, on efface l'afficheur
        if(pendingAdditiveOperator.isEmpty() && pendingMultiplicativeOperator.isEmpty())
        display->clear();
        waitingForOperand = false;
    }
    // Affichage du premier opérande ou de l'opération binaire
    display->setText(display->text() + QString::number(digitValue));

}

/***************************************************************************************
 * Description      : Affiche le carré, la racine carrée ou l'inverse du nombre affiché
 * Paramètres       : Aucun
 * Type de retour   : void
 ***************************************************************************************/
void Calculatrice::unaryOperatorClicked()
{
    // Si l'utilisateur clique sur un opérateur unaire au milieu ou à la suite d'une opération binaire (avant dans ce
    // dernier cas que le signe égal n'ait été cliqué), un message l'invite à terminer l'opération en cours d'abord
    if(!pendingMultiplicativeOperator.isEmpty() || !pendingAdditiveOperator.isEmpty()) {
        QMessageBox::warning(this, "Error", "Vous devez terminer l'opération d'abord !");
        return;
    }
    // Détermination de la touche cliquée à l'aide de la méthode send() de QObject
    // Comme c'est un bouton qui envoie le signal alors que la méthode renvoie un pointeur vers un QObject, on utilise qobject_cast
    Bouton* clickedButton = qobject_cast<Bouton*>(sender());
    // Le nom de l'opérateur unaire est obtenu du champ text du bouton cliqué
    QString clickedOperator = clickedButton->text();
    // L'opérande sur lequel est appliqué l'opérateur est celui déjà affiché
    double operand = display->text().toDouble();
    double result = 0.0;

    if(clickedOperator == tr("Sqrt"))
    {
        try {
            if(operand < 0.0)
            {
                abortOperation();
                throw ErreurRacineNombreNegatif("Racine carrée d'un nombre négatif impossible !");
                return;
            }
        result = sqrt(operand);
        }
        catch(exception const& e) {
            QMessageBox::critical(this, "Error", e.what());
        }
    }
    else if(clickedOperator == tr("x\302\262"))
    {
        result = pow(operand, 2.0);
    }
    else if(clickedOperator == tr("1/x"))
    {
        try {
            if(operand == 0.0)
            {
               abortOperation();
               throw ErreurInverseDeZero("Inverse de 0 impossible !");
              return;
            }
             result = 1.0 / operand;
        }
        catch(exception const& e) {
            QMessageBox::critical(this, "Error", e.what());
        }
    }
    // Si l'opération a réussi (pas de racine carré d'un nombre négatif ou d'inverse d'un nombre nul)
    // le résultat est affiché
    display->setText(QString::number(result));
    // La calculatrice est de nouveau prête à recevoir un nouveau opérande
    waitingForOperand = true;
}

/*********************************************************************************************************
 * Description      : Prépare l'opération d'addition ou de soustraction en attendant l'opérande de droite
 * Paramètres       : Aucun
 * Type de retour   : void
 *********************************************************************************************************/
void Calculatrice::additiveOperatorClicked()
{
    // Si l'utilisateur clique sur "+" ou "-" alors qu'une opération binaire est déjà affichée (en cours)
    // cette opération est d'abord exécutée (appel implicite du signe "=")
    // La priorité des opérations n'est pas prise en compte ici (comme pour la calculatrice standard sous windows, contrairement à la calculatrice scientifique)
    if(!pendingAdditiveOperator.isEmpty() || !pendingMultiplicativeOperator.isEmpty())
        equalClicked();
    // Détermination de la touche cliquée à l'aide de la méthode send() de QObject
    // Comme c'est un bouton qui envoie le signal alors que la méthode renvoie un pointeur vers un QObject, on utilise qobject_cast
    Bouton* clickedButton = qobject_cast<Bouton*>(sender());
    QString clickedOperator = clickedButton->text();
    // L'opérande sur lequel est appliqué l'opérateur est celui déjà affiché
    double operand = display->text().toDouble();

    sumSoFar = operand; // sumSoFar contient maintenant l'opérande de gauche de l'opération d'addition/soustraction

    pendingAdditiveOperator = clickedOperator; // pendingAdditiveOperator contient maintenant l'opérateur "+" ou "-"

    display->setText(display->text() + pendingAdditiveOperator); // L'opérande de gauche est affiché suivi de l'opérateur

    waitingForOperand = true;
}

/************************************************************************************************************
 * Description      : Prépare l'opération de multiplication ou de division en attendant l'opérande de droite
 * Paramètres       : Aucun
 * Type de retour   : void
 ************************************************************************************************************/
void Calculatrice:: multiplicativeOperatorClicked()
{
    // Si l'utilisateur clique sur "x" ou "÷" alors qu'une opération binaire est déjà affichée (en cours)
    // cette opération est d'abord exécutée (appel implicite du signe "=")
    // La priorité des opérations n'est pas prise en compte ici (comme pour la calculatrice standard sous windows, contrairement à la calculatrice scientifique)
    if(!pendingMultiplicativeOperator.isEmpty() || !pendingAdditiveOperator.isEmpty())
        equalClicked();
    // Détermination de la touche cliquée à l'aide de la méthode send() de QObject
    // Comme c'est un bouton qui envoie le signal alors que la méthode renvoie un pointeur vers un QObject, on utilise qobject_cast
    Bouton* clickedButton = qobject_cast<Bouton*>(sender());
    QString clickedOperator = clickedButton->text();
    // L'opérande sur lequel est appliqué l'opérateur est celui déjà affiché
    double operand = display->text().toDouble();

    factorSoFar = operand; // factorSoFar contient maintenant l'opérande de gauche de l'opération de multiplication/division

    pendingMultiplicativeOperator = clickedOperator; // pendingAdditiveOperator contient maintenant l'opérateur "x" ou "/"

    display->setText(display->text() + pendingMultiplicativeOperator); // L'opérande de gauche est affiché suivi de l'opérateur

    waitingForOperand = true;
}

/*********************************************************************************************************************
 * Description      : Afiche le résultat d'une opération binaire (multiplication, division, addition ou soustraction)
 * Paramètres       : Aucun
 * Type de retour   : void
 *********************************************************************************************************************/
void Calculatrice::equalClicked()
{
    // On fait appel à la fonction split() de la classe QString pour diviser le
    // texte affiché à l'aide de délimiteurs afin d'isoler le dernier opérande
    QRegExp sep("(\\+|\\-|\\\303\227|\\\303\267)"); // délimiteurs utilisés
    // On récupère le dernier opérande qui sera l'opérande de droite de la fonction calculate()
    double operand = display->text().split(sep).last().toDouble();
    // Si la touche "x" ou "÷" a été cliquée
    if(!pendingMultiplicativeOperator.isEmpty())
    {
    // Si l'opération binaire (multiplication/division) effectuée par calculate() entre l'opérande de droite (operand)
    // et celui de gauche (factorSoFar) ne s'est pas réalisée avec succès (division par 0), on abandonne l'opération
        if(!calculate(operand, pendingMultiplicativeOperator))
        {
            abortOperation();
            return;
        }
        // Sinon (succès de l'opération)
        // Le résultat de la multiplication/division est placé dans operand
        // pour être affiché par la suite
        operand = factorSoFar;
        factorSoFar = 0.0;
        // Évidemment, l'opérateur contenu dans pendingAdditiveOperator est supprimé car l'opération est maintenant terminée
        pendingMultiplicativeOperator.clear();
    }
    // Si la touche "+" ou "-" a été cliquée
    if(!pendingAdditiveOperator.isEmpty())
    {
    // Si l'opération binaire (addition/soustraction) effectuée par calculate() entre l'opérande de droite (operand)
    // et celui de gauche (SumSoFar) ne s'est pas réalisée avec succès, on abandonne l'opération
        if(!calculate(operand, pendingAdditiveOperator))
        {
            abortOperation();
            return;
        }
        // Sinon (succès de l'opération)
        // Le résultat de l'addition/soustraction sera affiché
        // Évidemment, l'opérateur contenu dans pendingAdditiveOperator est supprimé car l'opération est maintenant terminée
        pendingAdditiveOperator.clear();
    }
    else
    {
        // Sinon (ni la touche "+" ni la touche "-" n'a été cliquée)
        // factorSoFar (qui est maintenant dans "operand") est simplement placé dans sumSoFar
        // pour être affiché par la suite
        sumSoFar = operand;
    }

    display->setText(QString::number(sumSoFar)); // Affichage du résultat (factorSoFar ou sumSoFar)
    sumSoFar = 0.0;
    waitingForOperand = true;
}

/***************************************************************
 * Description      : Ajoute un point décimal au nombre affiché
 * Paramètres       : Aucun
 * Type de retour   : void
 ***************************************************************/
void Calculatrice::pointClicked()
{
    if(waitingForOperand)
    {
    // Si la calculatrice attend un opérande et on appuye sur la touche ".", on affiche un 0
        display->setText("0");
    }
    if(!display->text().split(QRegExp("\\W")).last().contains("."))
    {
    // Si le nombre affiché ne contient pas de point décimal, on lui en ajoute un
    // S'il s'agit du deuxième opérande, on le récupère d'abord à l'aide de la fonction QString::split()
        display->setText(display->text() + tr("."));
    }
    waitingForOperand = false;
}

/*******************************************************
 * Description      : Change le signe du nombre affiché
 * Paramètres       : Aucun
 * Type de retour   : void
 *******************************************************/
void Calculatrice::changeSignClicked()
{
    QString text = display->text();
    double value = text.toDouble();

    if(value > 0.0)
    {
    // Si la valeur affichée est positive, on la fait précéder par un signe "-"
        text.prepend(tr("-"));
    }
    else if(value < 0.0)
    {
    // Si la valeur affichée est négative, on supprime le signe "-"
        text.remove(0, 1);
    }
    display->setText(text);
}

/********************************************************************************
 * Description      : Supprime le dernier chiffre (le plus à droite) d'un nombre
 * Paramètres       : Aucun
 * Type de retour   : void
 ********************************************************************************/
void Calculatrice::backspaceClicked()
{
    if(waitingForOperand)
        return;

    QString text = display->text();
    text.chop(1); // On supprime un caractère (chiffre) à la fin du nombre
    if(text.isEmpty())
    {
        text = "0"; // On affiche "0" s'il n'y a plus de chiffres à effacer
        waitingForOperand = true;
    }
    display->setText(text);
}

/*****************************************************
 * Description      : Réinitialise l'opérande courant
 * Paramètres       : Aucun
 * Type de retour   : void
 *****************************************************/
void Calculatrice::clear()
{
    if(waitingForOperand)
        return;
    // On fait appel à la fonction split() de la classe QString pour diviser le
    // texte affiché à l'aide de délimiteurs afin d'isoler le dernier opérande
    QRegExp sep("(\\+|\\-|\\\303\227|\\\303\267)"); // Délimiteurs utilisés
    QString operand = display->text().split(sep).last();
    display->setText(display->text().remove(operand));
    waitingForOperand = true;
}

/*******************************************************************************
 * Description      : Réinitialise toutes les valeurs et toutes les opérations
 * Paramètres       : Aucun
 * Type de retour   : void
 *******************************************************************************/
void Calculatrice::clearAll()
{
    sumSoFar = 0.0;
    factorSoFar = 0.0;
    pendingAdditiveOperator.clear();
    pendingMultiplicativeOperator.clear();
    display->setText("0");
    waitingForOperand = true;
}

/****************************************
 * Description      : Efface la mémoire
 * Paramètres       : Aucun
 * Type de retour   : void
 ****************************************/
void Calculatrice::clearMemory()
{
    sumInMemory = 0.0;
}

/*************************************************************
 * Description      : Affiche la somme sauvegardée en mémoire
 * Paramètres       : Aucun
 * Type de retour   : void
 *************************************************************/
void Calculatrice::readMemory()
{
    // Si la valeur contenue en mémoire est un opérande de droite, on affiche l'opération entière
    if(!pendingMultiplicativeOperator.isEmpty() || !pendingAdditiveOperator.isEmpty()) {
        if(waitingForOperand)
        display->setText(display->text() + QString::number(sumInMemory));
        waitingForOperand = false;
        return;
    }
    // Sinon (opérande de gauche ou tout simplement une valeur isolée), on l'affiche seule
    else
        display->setText(QString::number(sumInMemory));
    waitingForOperand = true;
}

/*************************************************************************
 * Description      : Remplace la valeur en mémoire par la somme courante
 * Paramètres       : Aucun
 * Type de retour   : void
 *************************************************************************/
void Calculatrice::setMemory()
{
    equalClicked();
    sumInMemory = display->text().toDouble();
}

/*********************************************************************
 * Description      : Ajoute la somme courante à la valeur en mémoire
 * Paramètres       : Aucun
 * Type de retour   : void
 *********************************************************************/
void Calculatrice::addToMemory()
{
    equalClicked();
    sumInMemory += display->text().toDouble();
}

/*********************************************************************************************************************
 * Description      : Crée un nouveau bouton
 * Paramètres       : Un texte (label) qui identifie le bouton et un slot qui sera exécuté quand le botoun est cliqué
 * Type de retour   : Pointeur vers Bouton
 *********************************************************************************************************************/
Bouton *Calculatrice::createButton(const QString &text, const char *member)
{
    Bouton *button = new Bouton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

/******************************************************************************************
 * Description      : Appelle ClearALL() lors d'une erreur de calcul (opération interdite)
 * Paramètres       : Aucun
 * Type de retour   : void
 ******************************************************************************************/
void Calculatrice::abortOperation()
{
    clearAll();
    return;
}

/**********************************************************************************************************
 * Description      : Calcule, en fonction de son deuxième paramètre,
 *                    soit la somme/différence entre son premier paramètre et sumSoFar
 *                    soit la multiplication/division entre son premier paramètre et factorSoFar
 * Paramètres       : Le nombre à ajouter/soustraire à sumSoFar ou celui à multiplier/diviser factorSoFar
 *                    et l'opérateur de calcul
 * Type de retour   : booléen; false si l'opération a échouée
 **********************************************************************************************************/
bool Calculatrice::calculate(double rightOperand, const QString &pendingOperator)
{
    if(pendingOperator == tr("+"))
    {
        sumSoFar +=rightOperand;
    }
    else if(pendingOperator == tr("-"))
    {
        sumSoFar -= rightOperand;
    }
    else if(pendingOperator == tr("\303\227"))
    {
        factorSoFar *= rightOperand;
    }
    else if(pendingOperator == tr("\303\267"))
    {
        try
        {
            if (rightOperand == 0.0)
            {
                abortOperation();
                throw ErreurDivisionParZero("Division par 0 impossible");
                 return false;
            }
        factorSoFar /= rightOperand;
        }
        catch(exception const& e)
        {
            QMessageBox::critical(this, "Error", e.what());
        }
    }
    return true;

}
