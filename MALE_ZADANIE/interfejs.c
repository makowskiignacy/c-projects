#include "interfejs.h"
#include "parsujaca_wiersz.h"
#include "przechowujaca.h"

#include <stdio.h>
#include <string.h>

//Wypisuje argumenty z poziomu drzewa (bez nast) w porządku leksykograficznym
static void wypisz_bst(bst_t *d) {
    if (d) {
        wypisz_bst(d->lsyn);
        fprintf(stdout, "%s\n", d->nazwa);
        wypisz_bst(d->psyn);
    }
}

//Wyszukuje wskazany ścieżką węzeł, jeśli go nie ma zwraca wskaźnik na NULL
static bst_t **wyszukaj(lista_t *sciezka, bst_t **wezel) {
    while (wezel && *wezel && sciezka) {
        wezel = znajdz(wezel, sciezka->arg);
        sciezka = sciezka->nast;
        if (wezel && *wezel && sciezka) {
            wezel = &(*wezel)->nast;
        }
    }
    return wezel;
}

//Wyszukuje węzeł zgodnie z zasadami polecenia check (* jest meta-znakiem)
static bool wyszukaj_gwiazdka(lista_t *sciezka, bst_t **wezel) {
    if (!wezel || !(*wezel)) {
        return false;
    } else if (!sciezka) {
        return *wezel;
    } else if (sciezka->nast) {
        if (strcmp(sciezka->arg, "*") == 0) {
            bool wynik = wyszukaj_gwiazdka(sciezka->nast, &(*wezel)->nast);
            wynik = wynik || wyszukaj_gwiazdka(sciezka, &(*wezel)->lsyn) || wyszukaj_gwiazdka(sciezka, &(*wezel)->psyn);
            return wynik;
        } else {
            wezel = znajdz(wezel, sciezka->arg);
            if (wezel && *wezel) {
                return wyszukaj_gwiazdka(sciezka->nast, &(*wezel)->nast);
            } else {
                return false;
            }
        }
    } else {
        return *znajdz(wezel, sciezka->arg);
    }
}

//Dodaje do drzewa wskazaną w liście ścieżkę
static void polecenie_add(lista_t *polecenie, bst_t **wezel) {
    int dlugosc = dlugosc_listy(polecenie);
    if (dlugosc < 2 || dlugosc > 4) {
        fprintf(stderr, "ERROR\n");
    } else {
        polecenie = polecenie->nast;
        while (polecenie) {
            wezel = znajdz(wezel, polecenie->arg);
            if (!(*wezel)) {
                dodaj(wezel, polecenie->arg);
            }
            wezel = &(*wezel)->nast;
            polecenie = polecenie->nast;
        }
        fprintf(stdout, "OK\n");
    }

}

//Usuwa wskazany w liście poziom drzewa
static void polecenie_del(lista_t *polecenie, bst_t **wezel) {
    int dlugosc = dlugosc_listy(polecenie);
    if (dlugosc < 1 || dlugosc > 4) {
        fprintf(stderr, "ERROR\n");
    } else {
        usun(wyszukaj(polecenie->nast, wezel));
        fprintf(stdout, "OK\n");
    }
}

//Wypisuje na stdout wskazany w liście poziom drzewa
static void polecenie_print(lista_t *polecenie, bst_t **wezel) {
    int dlugosc = dlugosc_listy(polecenie);
    if (dlugosc < 1 || dlugosc > 3) {
        fprintf(stderr, "ERROR\n");
    } else {
        bst_t *znaleziony = *wyszukaj(polecenie->nast, wezel);
        if (znaleziony && strcmp(znaleziony->nazwa, ostatni_element(polecenie)->arg) == 0) {
            wypisz_bst(znaleziony->nast);
        } else {
            wypisz_bst(znaleziony);
        }
    }
}

//Wykonuje sprawdzenie czy w drzewie istnieje element zgodny z listą
static void polecenie_check(lista_t *polecenie, bst_t **wezel) {
    int dlugosc = dlugosc_listy(polecenie);
    if (dlugosc < 2 || dlugosc > 4 || strcmp(ostatni_element(polecenie)->arg, "*") == 0) {
        fprintf(stderr, "ERROR\n");
    } else {
        if (wyszukaj_gwiazdka(polecenie->nast, wezel)) {
            fprintf(stdout, "YES\n");
        } else {
            fprintf(stdout, "NO\n");
        }
    }
}


void uruchom_lasy() {
    lista_t *historia_wierszy = NULL;
    lista_t *aktualna = parsuj_wiersz(&historia_wierszy);
    bst_t *baza_danych = NULL;
    while (aktualna || !feof(stdin)) {
        if (aktualna && aktualna->arg) {
            if (strcmp(aktualna->arg, "ADD") == 0) {
                polecenie_add(aktualna, &baza_danych);
            } else if (strcmp(aktualna->arg, "DEL") == 0) {
                polecenie_del(aktualna, &baza_danych);
            } else if (strcmp(aktualna->arg, "PRINT") == 0) {
                polecenie_print(aktualna, &baza_danych);
            } else if (strcmp(aktualna->arg, "CHECK") == 0) {
                polecenie_check(aktualna, &baza_danych);
            } else if (aktualna->arg[0] == '#') {

            } else {
                fprintf(stderr, "ERROR\n");
            }
        }
        zwolnij_liste(aktualna);
        aktualna = parsuj_wiersz(&historia_wierszy);
    }
    zwolnij_liste(aktualna);
    wyczysc(baza_danych);
    wyczysc_liste(historia_wierszy);
}
