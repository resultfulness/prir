#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int cmp_int(const void *a, const void *b) {
    return (*(int*) a - *(int *)b);
}

int main(int argc, char **argv) {
    long int *tmp;
    long int *pierwsze;
    long i, j, liczba;
    long int liczba_podzielników;
    long int liczba_liczb_pierwszych = 0; /*l. liczb pierwszych w tablicy pierwsze*/
    double czas; /*zmienna do  mierzenia czasu*/
    FILE *fp;
    int n_wątków, górny_limit_liczb;
    int sqrt_górny_limit_liczb, max_liczba_liczb_pierwszych_w_przedziale;

    if (argc < 3) {
        printf("Użycie: %s <n wątków> <górny limit liczb>\n", argv[0]);
        return 1;
    }

    if ((n_wątków = atoi(argv[1])) <= 0) {
        printf("n wątków musi być liczbą > 0\n");
        return 1;
    }
    omp_set_num_threads(n_wątków);

    if ((górny_limit_liczb = atoi(argv[2])) <= 0) {
        printf("górny limit liczb musi być liczbą > 0\n");
        return 1;
    }

    sqrt_górny_limit_liczb = (int)sqrt(górny_limit_liczb);


    max_liczba_liczb_pierwszych_w_przedziale = górny_limit_liczb > 100000 ? górny_limit_liczb / 10 : górny_limit_liczb;

    tmp = malloc(sizeof(long int) * (sqrt_górny_limit_liczb + 1));
    if (tmp == NULL) {
        printf("błąd alokacji\n");
        return 1;
    };

    pierwsze = malloc(
        sizeof(long int) * max_liczba_liczb_pierwszych_w_przedziale
    );
    if (pierwsze == NULL) {
        printf("błąd alokacji\n");
        free(tmp);
        return 1;
    }

    /*wyznaczanie podzielnikow z przedzialow 2..S*/
    for (i = 2; i <= sqrt_górny_limit_liczb; i++) {
        tmp[i] = 1; /*inicjowanie*/
    }

    for (i = 2; i <= sqrt_górny_limit_liczb; i++) {
        if (tmp[i] == 1) {
            pierwsze[liczba_liczb_pierwszych++] = i; /*zapamietanie podzielnika*/

            /*wykreslanie liczb zlozonych bedacych wielokrotnosciami i*/
            for (j = i + i; j <= sqrt_górny_limit_liczb; j += i) {
                tmp[j] = 0;
            }
        }
    }

    free(tmp);

    liczba_podzielników = liczba_liczb_pierwszych; /*zapamietanie liczby podzielnikow*/

    #pragma omp parallel for private(i)
    for (
        liczba = sqrt_górny_limit_liczb + 1;
        liczba <= górny_limit_liczb;
        liczba++
    ) {
        for (i = 0; i < liczba_podzielników; i++) {
            if (liczba % pierwsze[i] == 0) { /* liczba zlozona */
                break;
            }
        }

        if (i == liczba_podzielników) {
            #pragma omp critical
            {
                pierwsze[liczba_liczb_pierwszych] = liczba; /*zapamietanie liczby pierwszej*/
                liczba_liczb_pierwszych += 1;
            }
        }
    }

    // qsort(pierwsze, liczba_liczb_pierwszych, sizeof(long int), cmp_int);

    if ((fp = fopen("primes.txt", "w")) == NULL) {
        printf("Nie moge otworzyc pliku do zapisu\n");
        free(pierwsze);
        exit(1);
    }

    for (i = 0; i < liczba_liczb_pierwszych; i++) {
        fprintf(fp, "%ld ", pierwsze[i]);
    }

    fclose(fp);
    free(pierwsze);
    return 0;
}
