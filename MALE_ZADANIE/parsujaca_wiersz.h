#ifndef MALE_ZADANIE_PARSUJACA_WIERSZ_H
#define MALE_ZADANIE_PARSUJACA_WIERSZ_H

struct lista {
    char *arg;
    struct lista *nast;
};
typedef struct lista lista_t;

/*
 * Funkcja dokłada element listy z argumentem "na_koniec"
 * Zadaniem funkcji jest alokacja tego elementu
 */
void doloz(lista_t **lista, char *na_koniec);

//Zwraca ilość elementów listy
int dlugosc_listy(lista_t *l);

//Zwalnia wyłącznie strukturę listy
void zwolnij_liste(lista_t *l);

//Czyści listę razem z zaalokowanym tekstem
void wyczysc_liste(lista_t *l);

//Zwraca ostatni element listy
lista_t *ostatni_element(lista_t *l);

//Wczytuje wiersz, dzieli go na słowa i zwraca jako ich listę
//Kolejne zaalokowane wiersze są odkładane w historii wierszy
lista_t *parsuj_wiersz(lista_t **histria_wierszy);

#endif //MALE_ZADANIE_PARSUJACA_WIERSZ_H
