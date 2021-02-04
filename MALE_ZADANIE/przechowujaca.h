#ifndef MALE_ZADANIE_PRZECHOWUJACA_H
#define MALE_ZADANIE_PRZECHOWUJACA_H

#include <stdbool.h>

struct bst {
    //argument drzewa, po którym jest sortowane
    char *nazwa;
    struct bst *lsyn;
    struct bst *psyn;
    //drzewo posiada poziomy odpowiadające strukturze danych z zadania, wskaźnik na "niższy" poziom
    struct bst *nast;
};
typedef struct bst bst_t;

//Pod podanym wskaźnikiem alokuje strukturę bst_t z argumentem "nazwa"
bool dodaj(bst_t **wezel, char *nazwa);

//Usuwa pojedyńczy węzeł wskazany przez wskaźnik
void usun(bst_t **wezel);

//Zwalnia strukturę drzewa bst, nie zwalnia argumentów
void wyczysc(bst_t *wezel);

//Wyszukuje na danym poziomie wskazywanego drzewa bst węzeł o zadanym argumencie
bst_t **znajdz(bst_t **wezel, char *nazwa);

#endif //MALE_ZADANIE_PRZECHOWUJACA_H
