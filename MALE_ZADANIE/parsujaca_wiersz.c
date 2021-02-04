#include "parsujaca_wiersz.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

//Najmniejszy poprawny, nie biały znak
#define ZNAK 33

//Sprawdza czy wiersz składa się z poprawnych znaków
static bool poprawny_wiersz(char *wiersz, ssize_t znaki) {
    bool wynik = true;
    if ((znaki > 1 && wiersz) || (znaki == 0 && wiersz[0] != '\n')) {
        bool poprawny_znak;
        bool bialy;
        bool wszystkie_biale = true;
        unsigned char znak;
        for (ssize_t i = 0; i < znaki; ++i) {
            znak = (unsigned char) wiersz[i];
            bialy = isspace(wiersz[i]);
            poprawny_znak = znak >= ZNAK || bialy;
            wszystkie_biale = wszystkie_biale && bialy;
            wynik = wynik && poprawny_znak;
        }
        wynik = wynik && !wszystkie_biale;
    }
    return wynik;
}

void doloz(lista_t **lista, char *na_koniec) {
    if (lista && (*lista) && na_koniec) {
        while ((*lista)->nast) {
            lista = &(*lista)->nast;
        }
        (*lista)->nast = malloc(sizeof(lista_t));
        if (!(*lista)->nast) {
            fprintf(stderr, "Blad alokacji pamieci\n");
            exit(1);
        }
        lista = &(*lista)->nast;
        (*lista)->arg = na_koniec;
        (*lista)->nast = NULL;
    } else if (lista && !(*lista) && na_koniec) {
        (*lista) = malloc(sizeof(lista_t));
        if (!(*lista)) {
            fprintf(stderr, "Blad alokacji pamieci\n");
            exit(1);
        }
        (*lista)->arg = na_koniec;
        (*lista)->nast = NULL;
    }
}

void zwolnij_liste(lista_t *l) {
    if (l) {
        zwolnij_liste(l->nast);
        free(l);
    }
}

void wyczysc_liste(lista_t *l) {
    if (l) {
        wyczysc_liste(l->nast);
        free(l->arg);
        free(l);
    }
}

int dlugosc_listy(lista_t *l) {
    int dlugosc = 0;
    while (l) {
        l = l->nast;
        dlugosc++;
    }
    return dlugosc;
}

lista_t *ostatni_element(lista_t *l) {
    while (l && l->nast) {
        l = l->nast;
    }
    return l;
}

lista_t *parsuj_wiersz(lista_t **histria_wierszy) {
    char *wiersz = NULL;
    ssize_t przeczytane_znaki;
    size_t zaalokowana_pamiec = 0;
    przeczytane_znaki = getline(&wiersz, &zaalokowana_pamiec, stdin);
    if (przeczytane_znaki == -1) {
        if (feof(stdin)) {
            free(wiersz);
            return NULL;
        } else {
            free(wiersz);
            zwolnij_liste(*histria_wierszy);
            fprintf(stderr, "Blad funkcji getline\n");
            exit(1);
        }
    } else if (!poprawny_wiersz(wiersz, przeczytane_znaki)) {
        free(wiersz);
        fprintf(stderr, "ERROR\n");
        return NULL;
    }
    doloz(histria_wierszy, wiersz);
    lista_t *wers = NULL;
    lista_t **koniec = &wers;
    char *argument = strtok(wiersz, " \t\v\f\r\n");
    while (argument) {
        doloz(koniec, argument);
        koniec = &(*koniec)->nast;
        argument = strtok(NULL, " \t\v\f\r\n");
    }
    return wers;
}