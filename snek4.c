//compilation   x86_64-w64-mingw32-gcc -o snek4.exe snek4.c -Wall -Wextra

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

////////////////////////////

//modifiable here
#define widthGrid 15
#define heightGrid 15

//modifiable inGame
int initSize = 5;      //taille initiale du serpent en debut de partie. Les parties supplémentaires se situront sous le point de départ
int inGameDelay = 100; //delais entre les capture de touches. plus le chiffre est bas plus le jeu sera rapide.
int Xstart = 1;        //1 correspond a gauche apres le mur
int Ystart = 1;        //1 correspond a en haut apres le mur

char borderChar = '#'; //caractere de bordure modifiable [ascii]
char voidChar = ' ';   //caractere quand il n'y a pas de serpent
char snakeChar = 'O';  //caractere composant le corp du serpent (tete et queue incluses)
char appleChar = 'x';  //caractere symbolisant une pomme
char extendChar = ' '; //caractere permettant d'etendre la surface de jeu pour faire un carré

//technically modifiable here but useless
int menuyingDelay = 100;

//////////////////////////// Not modifiable ///////////////

char grid[1 + widthGrid * 2 * heightGrid + 1];     //la grille complete de jeu faite de caracteres
int gridSize = 1 + widthGrid * 2 * heightGrid + 1; //la grille utile
int exLong = widthGrid + widthGrid;                //la longueur apres avoir rajouté des espaces
int updateList[widthGrid * heightGrid + 1];        //un vecteur indiquant les positions a mettre à jour dans la grille dans l'ordre.
int tailleSerpent;                                 //la taille du serpent lors du jeu
int head;                                          //La tete du serpent sur laquelle s'articule deplacements et colisions
bool oneSwitch = true;                             //sert a récupéré une fois un char ne devant pas etre pris en comtpe (normalement c'est un smiley)
char excludeChar;                                  //le char en question
int cursorValue = 1;                               //position du curseur lors d'un menu
bool doBreak = false;                              //permet de sortir d'un menu si true
char moveInstruction;                              //instruction indiquant la direction du serpent
char firstMoveInstruction = 'd';                   //instruction indiquant la direction du serpent choisit par l'utilisateur au début de la partie dans les options
int rawValue;                                      //variable permettant de connaitre les coordo sans prise en compte de la grille réélle
bool showSnakePos = false;                         //devait servir a afficher des options mais printf un bool c'est relou
int gameScore = 0;                                 //compteur de score
int bestScore = 0;                                 //enregistre le meilleur score jusqu'a ce que l'appli soit arreté


////////////////////// MENUS ////////////////////////

char IsCursor(int nb) //fonctino qui renvoie juste un curseur ou pas de curseur
{
    if (nb == cursorValue)
    {
        return '>';
    }
    else
    {
        return '\0';
    }
}
int menuying(int nbElements) //fonctino faisant l'acquisition de la touche du joueur et la retourne au menu correspondant
{
    //kbhit from stackoverflow
    DWORD start_time, check_time;
    start_time = GetTickCount();
    check_time = start_time + menuyingDelay; //GetTickCount returns time in miliseconds, so I add 500 to wait input for half a second.
    char ch = 2;

    //printf("%c pour rentrer une touche", delay);
    while (check_time > GetTickCount())
    {
        if (_kbhit())
        {
            ch = _getch();
        }
    }
    if (oneSwitch) //pour bloquer le smiley
    {
        excludeChar = ch;
        oneSwitch = false;
    }
    char instruction = '.';
    if (ch != excludeChar)
    {
        instruction = ch;
    }
    if (instruction == 'z') //monter le curseur
    {
        if (cursorValue > 1)
        {
            cursorValue--;
        }
    }
    if (instruction == 's') //descendre le curseur
    {
        if (cursorValue < nbElements)
        {
            cursorValue++;
        }
    }
    if (instruction == 'd' || instruction == '\r') //si on valide le choix
    {
        instruction = '.'; //on reset l'instruction pour éviter des boucles
        return cursorValue;
    }
    else if (instruction == 'q') //si on annule
    {
        instruction = '.'; //on reset l'instruction pour éviter des boucles
        return -1;
    }
    else //si on fait rien
    {
        instruction = '.'; //on reset l'instruction pour éviter des boucles
        return 0;
    }
}

int menu(void) //menu principal
{
    int res = 0;
    cursorValue = 1; //position par defaut du curseur dans ce menu
    do
    {

        system("cls"); //"clear" sous linux

        printf("  _____            _    \n");
        printf(" / ____|          | |   \n");
        printf("| (___  _ __   ___| | __\n");
        printf(" \\___ \\| '_ \\ / _ \\ |/ /\n");
        printf(" ____) | | | |  __/   < \n");
        printf("|_____/|_| |_|\\___|_|\\_\\\n");
        printf("   zqsd         by hippo\n");
        printf(" \n");
        printf(" %c Play     \n", IsCursor(1));
        printf(" %c Options  \n", IsCursor(2));
        printf(" %c Credits  \n", IsCursor(3));
        printf(" %c Exit     \n", IsCursor(4));
        printf(" \n");

        res = menuying(4); //indiqué en parametre le nombre de choix possible pour ce menu
    } while (res == 0);
    return res;
}

int score(void) //menu de fin de partie
{
    bool newBest = false;
    int res = 0;
    cursorValue = 1; //position par defaut du curseur dans ce menu
    do
    {
        system("cls");
        printf("\n");
        printf(" GAME SCORE : %d\n", gameScore); //affiche le score
        printf("\n");
        if (bestScore < gameScore)
        {
            newBest = true;
            printf(" NEW RECORD\n");
        }
        else
        {
            printf("best score : %d\n", bestScore);
        }
        printf("\n");
        printf(" %c Menu     \n", IsCursor(1));
        printf(" %c Exit     \n", IsCursor(2));
        printf(" \n");

        res = menuying(2); //indiqué en parametre le nombre de choix possible pour ce menu
    } while (res == 0);
    if (newBest)
    {
        bestScore = gameScore;
    }
    return res;
}

int option(void) //menu des options de la partie
{
    int res = 0;
    cursorValue = 1; //position par defaut du curseur dans ce menu
    do
    {
        system("cls");
        printf(" \n");
        printf(" %c GameSlow             : %d     \n", IsCursor(1), inGameDelay);
        printf(" \n");
        printf(" %c Initial Size         : %d     \n", IsCursor(2), initSize);
        printf(" %c Initial position X   : %d     \n", IsCursor(3), Xstart / 2 + 1);
        printf(" %c Initial position Y   : %d     \n", IsCursor(4), Ystart);
        printf(" %c Initial direction    : %c     \n", IsCursor(5), firstMoveInstruction);
        printf(" \n");
        printf(" %c Border Character     : '%c'   \n", IsCursor(6), borderChar);
        printf(" %c Snek Character       : '%c'   \n", IsCursor(7), snakeChar);
        printf(" %c FreeSpace Character  : '%c'   \n", IsCursor(8), voidChar);
        printf(" %c Apple Character      : '%c'   \n", IsCursor(9), appleChar);
        printf(" %c Extend Character     : '%c'   \n", IsCursor(10), extendChar);
        printf(" \n");
        printf("  Grid dimensions are defined before compilation\n");
        printf(" \n");

        res = menuying(10); //indiqué en parametre le nombre de choix possible pour ce menu
    } while (res == 0);
    return res;
}

int credit(void) //menu pour afficher les crédits mais y'a pas grand chose
{
    int res = 0;
    cursorValue = 1; //position par defaut du curseur dans ce menu
    do
    {
        system("cls");
        printf(" Game created by hippolyte Roussel the 14/05/21.\n");
        printf(" \n");
        res = menuying(0); //indiqué en parametre le nombre de choix possible pour ce menu
    } while (res == 0);
    return res;
}

////////////////////// SETUP GAME ////////////////////////

void setupGrid(void)
{
    //tout le vecteur
    for (int i = 0; i < gridSize; i++)
    {
        grid[i] = voidChar;
    }
    //les \n
    for (int i = 0 + exLong; i < gridSize - 1; i += exLong)
    {
        grid[i] = '\n';
    }
    //le premier et le dernier
    grid[0] = '\n';
    grid[gridSize - 1] = '\0';

    //mur haut
    for (int i = 1; i < exLong; i++)
    {
        grid[i] = borderChar;
    }
    //mur Gauche
    for (int i = 1; i < gridSize - 1; i += exLong)
    {
        grid[i] = borderChar;
    }
    //mur droit
    for (int i = exLong - 1; i < gridSize - 1; i += exLong)
    {
        grid[i] = borderChar;
    }
    //mur bas
    for (int i = gridSize - 1 - exLong; i < gridSize - 2; i++)
    {
        grid[i] = borderChar;
    }

    //trous
    for (int i = 0; i < heightGrid - 0; i++) //repeter de la 2e a l'avant derniere ligne:
    {
        for (int j = 2; j < exLong - 1; j += 2) //toutes les 2 cases
        {
            grid[i * exLong + j] = extendChar; //trou
        }
    }
}

void setupSnake(void)
{
    moveInstruction = firstMoveInstruction;
    tailleSerpent = initSize; //la taille du serpent lors du jeu
    head = Xstart + 2 + Ystart * exLong;
    updateList[0] = head;
    updateList[1] = head;
    updateList[2] = head;
    updateList[3] = head;
    updateList[4] = head;

    grid[head] = snakeChar; //on affiche la premiere tete

    gameScore = 0; //remise du score a 0
}

////////////////////// UPDATE GAME ////////////////////////

void deplacement(void)
{
    DWORD start_time, check_time;
    start_time = GetTickCount();
    check_time = start_time + inGameDelay; //GetTickCount returns time in miliseconds, so I add 500 to wait input for half a second.
    char ch = 2;

    //printf("%c pour rentrer une touche", delay);
    while (check_time > GetTickCount())
    {
        if (_kbhit())
        {
            ch = _getch();
        }
    }
    if (ch != excludeChar)
    {
        moveInstruction = ch;
    }

    switch (moveInstruction)
    {
    case 'z':
        head -= exLong;
        break;
    case 'q':
        head -= 2;
        break;
    case 's':
        head += exLong;
        break;
    case 'd':
        head += 2;
        break;
    default:
        break;
    }
}
void my_delay(int i) /*Pause l'application pour i seconds*/
{
    clock_t start, end;
    start = clock();
    while (((end = clock()) - start) <= i * CLOCKS_PER_SEC)
        ;
}
int updateSnake(void)
{

    updateList[tailleSerpent] = head; //on ajoute la nouvelle position en tete du vecteur

    if (showSnakePos == true) //option de debuggage
    {
        printf("position du serpent sur le vecteur = [ ");
        for (int i = 0; i <= tailleSerpent; i++)
        {
            printf("%d ", updateList[i]);
        }
        printf("]\n");

        //my_delay(2);
    }

    if (updateList[0] == updateList[1]) //si le serpent est enroulé sur lui meme.
    {
        //on efface pas
    }
    else //sinon le serpent n'est pas enroulé sur lui meme
    {
        grid[updateList[0]] = voidChar; //on efface le char
    }
    if (grid[head] == appleChar) // si on mange une pomme
    {
        tailleSerpent++;                  //la taille du serpent est modifié
        updateList[tailleSerpent] = head; //on rajoute la nouvelle pomme en double dans le serpent pour le prolongé
    }
    for (int i = 0; i < tailleSerpent; i++) //on met a jour le vecteur en décalant tout de 1
    {
        updateList[i] = updateList[i + 1];
    }

    ///// Colision /////

    if (grid[head] == appleChar) //pomme
    {
        return 1;
    }
    else if (grid[head] == snakeChar) //queue : fin de la partie
    {
        return 2;
    }
    else if (grid[head] == borderChar) //mur : fin de la partie
    {
        return 2;
    }
    else //le serpent ne tappe rien
    {
        return 0;
    }
}

void randomApple(void)
{
    int applePosition;
    do
    {
        applePosition = rand() % ((1 + exLong * heightGrid) - (1) + 1) + (1);                           //tire unz position pour la pomme d'une valeur aléatoire dans la plage indiqué
    } while (grid[applePosition] != voidChar || applePosition % 2 == 0 || grid[applePosition] == '\n'); //vérifie sir la pomme est placable a cette endroit sinon relance
    grid[applePosition] = appleChar;                                                                    //place la pomme dans la grille
}

void play(void)
{

    setupGrid();  //créé l'espace de jeu
    setupSnake(); //créé un serpent

    randomApple(); //place une premiere pomme
    while (true)
    {
        system("cls");                     //vide le terminal
        printf("%s\n", grid);              //affiche la grille de jeu
        printf("score : %d\n", gameScore); //affiche le score

        deplacement();
        switch (updateSnake())
        {
        case 1: //pomme
            randomApple();
            gameScore += widthGrid * heightGrid - inGameDelay / 2;
            break; //mur : fin de la partie
        case 2:
            return;
            break;
        case 3: // le serpent se situe sur une case extend
            printf("CRTICAL POSITIONNING ERROR\n");
            break;
        default: //Tout va bien le jeu continue
            break;
        }
        gameScore--;
        grid[head] = snakeChar; //enfin on affiche la nouvelle tete
    }
}

////////////////////// MAIN APP ////////////////////////

int main(void)
{
    do //boucle a l'infini sauf si return
    {
        switch (menu()) //resultat du choix du menu principal
        {
        case 1:
            play();
            if (score() == 2) //resultat du choix du menu score, simplifié en si exit
            {
                system("cls");
                return 0; //quitter l'appli
            }
            break;
        case 2:
            doBreak = false;
            do
            {
                switch (option()) //resultat du choix du menu option
                {
                case 1:
                    printf("Enter new delay between Snek refreshing :");
                    scanf(" %d", &inGameDelay);
                    break;
                case 2:
                    printf("Enter new value for Snek's initiale size : ");
                    scanf(" %d", &initSize);
                    break;
                case 3:
                    rawValue = 1;
                    printf("Enter new X coordonate where to start (1 = left) : ");
                    scanf(" %d", &rawValue);
                    Xstart = rawValue * 2 - 1;
                    break;
                case 4:
                    printf("Enter new Y coordonate where to start (1 = top) : ");
                    scanf(" %d", &Ystart);
                    break;
                case 5:
                    printf("Enter new direction to start a game with (zqsd) : ");
                    scanf(" %c", &firstMoveInstruction);
                    break;
                case 6:
                    printf("Enter new character to represent the border : ");
                    scanf(" %c", &borderChar);
                    break;
                case 7:
                    printf("Enter new character to represent the Snek : ");
                    scanf(" %c", &snakeChar);
                    break;
                case 8:
                    printf("Enter new character to represent the free space : ");
                    scanf(" %c", &voidChar);
                    break;
                case 9:
                    printf("Enter new character to represent the apple : ");
                    scanf(" %c", &appleChar);
                    break;
                case 10:
                    printf("Enter new character to represent the extended space (between free spaces) : ");
                    scanf(" %c", &extendChar);
                    break;
                case -1: //pour quitter le menu option
                    doBreak = true;
                    break;
                default:
                    break;
                }
            } while (doBreak == false);

            break;
        case 3:
            credit();
            break;
        case 4:            //si exit
            system("cls"); //effacer l'ecran
            return 0;      //quitter l'appli
            break;
        default:
            break;
        }
    } while (true);
    system("cls");
    return 1; //ne doit pas arriver normalement
}

/*
TODO:
placer des murs
bordure téléportable

*/