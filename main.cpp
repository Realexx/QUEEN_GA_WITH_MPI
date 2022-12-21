#include <mpi.h>
#include <cstdio>
#include <iostream>

#include "vector"
#include <cstdlib>

using Population = std::vector<std::vector<int>>;

// Fonction d'évaluation d'un individu
int evaluation(const std::vector<int>& individu) {
    int nbConflits = 0;
    for (int i = 0; i < individu.size(); ++i) {
        for (int j = i; j < individu.size(); ++j) {
            if (i == j) continue;
            int i1 = individu[i];
            int i2 = individu[j];

            // Conflits dans les lignes
            if (i1 == i2) nbConflits++;

            // Conflits dans les diagonales
            int differenceIndex = abs(i - j);
            int differenceValeur = abs(i1 - i2);

            if (differenceIndex == differenceValeur) nbConflits++;
        }
    }
    return nbConflits;
}

// Fonction d'initialisation d'une population de 'nbIndividus' individus de taille 'taille' chacun
Population init(int nbIndividus, int taille) {
    std::vector<std::vector<int>> population;

    // Création de 'nbIndividus' individus générés aléatoirement
    for (int i = 0; i < nbIndividus; ++i) {
        std::vector<int> individuAAjouter;

        // Remplir l'individu avec des valeurs aléatoires
        for (int j = 0; j < taille ; ++j) {
            individuAAjouter.push_back(rand() % taille);
        }

        // Calculer le nombre de conflits de l'individu et l'ajouter dans son tableau
        // Exemple : Individu [2,0,3,1] -> 0 conflit(s) -> [2,0,3,1,0] -> Meilleur solution
        int nbConflitsIndividuAAjouter = evaluation(individuAAjouter);
        individuAAjouter.push_back(nbConflitsIndividuAAjouter);

        // Ajouter l'individu au tableau 'population'
        population.push_back(individuAAjouter);

    }
    return population;
}

// Fonction qui trouve le meilleur individu d'une population
std::vector<int> rechercheMeilleur(const Population& pop) {
    int nbConflitsMeilleur = 6; // On initialise la variable du nombre conflit au nb de conflits maximum possible
    std::vector<int> meilleurIndividu = pop[0];
    size_t indexDernierElement = meilleurIndividu.size() - 1;

    for (std::vector<int> individu : pop) {
        int nbConflitIndividuCourant = individu[indexDernierElement];

        if (nbConflitIndividuCourant < nbConflitsMeilleur) {
            meilleurIndividu = individu;
            nbConflitsMeilleur = nbConflitIndividuCourant;
        }
    }
    return meilleurIndividu;
}

std::vector<int> croisement() { // TODO
    return std::vector<int>();
}

std::vector<int> mutation() { // TODO
    return std::vector<int>();
}

/**
 * Fonction générale qui résout le problème des reines
 * @param nbIndividus nombre d'individus par générations
 * @param taille taille du tableau dans lequel résoudre le problème des reines (Ex : si taille = 4 -> tableau 4x4)
 * @param nbGenerations
 * @param p
 */
void QueenAlgorithm(int nbIndividus, int taille, int nbGenerations, float p) {
    srand (static_cast <unsigned> (time(nullptr))); // Pour la randomisation
    int compteurGeneration = 0;
    Population generation = init(nbIndividus, taille);

    std::vector<int> meilleurIndividuGlobal = rechercheMeilleur(generation);

    std::vector<int> x(meilleurIndividuGlobal.size()); // Copie de meilleurIndividuGlobal dans une variable 'x'.
    copy(meilleurIndividuGlobal.begin(), meilleurIndividuGlobal.end(), x.begin());

    while (compteurGeneration <= nbGenerations) { // Itération sur les générations
        // Print pour chaque génération
        std::cout << "Iteration : " << compteurGeneration << " | Meilleur individu global : [ ";
        for (int value : meilleurIndividuGlobal) {
            std::cout << value << " ";
        }
        std::cout << ']' << std::endl;

        for (int i = 0; i < nbIndividus; ++i) { // Pour chaque individu de la génération

            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX); // Génération d'un float entre 0 et 1.

            if (r < p) {
                x = croisement(); // Croisement entre le meilleur individu et l'individu courant
            } else {
                x = mutation(); // Mutation de l'individu courant
            }
            // TODO il faut que x contienne le cost mis à jour après mutation ou croisement

            if (x[x.size() - 1] <= generation[i][x.size() - 1]) { // Comparaison du nombre de conflits entre l'individu courant et l'individu qui a été généré via mutation ou croisement (variable 'x').
                // Si l'individu généré est meilleur → remplacement de l'individu courant dans la génération
                for (int j = 0; j < x.size(); ++j) {
                    generation[i][j] = x[j];
                }
            }
            std::vector<int> value = {} ; // TODO que mettre dans cette variable ??
            if (meilleurIndividuGlobal[meilleurIndividuGlobal.size()-1] > value[value.size()-1])
                std::copy(value.begin(), value.end(), meilleurIndividuGlobal.begin());
        }
        compteurGeneration += 1;
    }
}

int main(int argc, char** argv) {
    srand (static_cast <unsigned> (time(nullptr)));

    /*

    Population pop = init(10, 4);
    for (std::vector<int> tab : pop) {
        for (int i : tab) {
            std::cout << i ;
        }
        std::cout << std::endl;
    }

    std::vector<int> best = rechercheMeilleur(pop);
    for (int x : best) std::cout << '\n' << x;

    std::cout << "\n";

    std::vector<int> meilleurIndividuGlobal = rechercheMeilleur(pop);

    std::vector<int> copieMeilleurIndividuGlobal(meilleurIndividuGlobal.size()); // Copie de meilleurIndividuGlobal
    copy(meilleurIndividuGlobal.begin(), meilleurIndividuGlobal.end(), copieMeilleurIndividuGlobal.begin());

    for (int x : meilleurIndividuGlobal) std::cout << x;

    std::cout << "\n";

    for (int y : copieMeilleurIndividuGlobal) std::cout << y;

     */

    /**
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Print off a hello world message
    printf("Hello world from processor %d out of %d processors\n",
           world_rank, world_size);

    // Finalize the MPI environment.
    MPI_Finalize();
     */
}
