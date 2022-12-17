#include <mpi.h>
#include <cstdio>
#include <iostream>

#include "vector"
#include <cstdlib>

using Population = std::vector<std::vector<int>>;

// Fonction d'initialisation d'une population de 'nbIndividus' individus de taille 'taille' chacun
Population init(int nbIndividus, int taille) {
    std::vector<std::vector<int>> population;

    // Création de 'nbIndividus' individus générée aléatoirement
    for (int i = 0; i < nbIndividus; ++i) {
        std::vector<int> individuAAjouter;

        // Remplir l'individu avec des valeurs aléatoires
        for (int j = 0; j < taille ; ++j) {
            individuAAjouter.push_back(rand() % taille);
        }

        // Ajouter l'individu au tableau 'population'
        population.push_back(individuAAjouter);

    }
    return population;
}

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

void QueenAlgorithm(int nbIndividus, int taille, int nbGenerations) {
    int compteurGeneration = 0;
    Population generation = init(nbIndividus, taille);

    std::vector<int> meilleurIndividuGlobal; // TODO Réussir à trier le tableau en fonction du nombre de conflits de chaque individu OU réussir a trouver le meilleur individu d'une population ==> Le mettre dans cette variable

    std::vector<int> copieMeilleurIndividuGlobal(meilleurIndividuGlobal.size()); // Copie de meilleurIndividuGlobal
    copy(meilleurIndividuGlobal.begin(), meilleurIndividuGlobal.end(), copieMeilleurIndividuGlobal.begin());



    while (compteurGeneration <= nbGenerations) { // Itération sur les générations
        // Print pour chaque génération
        std::cout << "Iteration 1 : " << compteurGeneration << " | Meilleur individu global : [ ";
        for (int value : meilleurIndividuGlobal) {
            std::cout << value << " ";
        }
        std::cout << ']' << std::endl;

        for (int i = 0; i < nbIndividus; ++i) { // Pour chaque individu
            int indexIndividu1 = rand() % nbIndividus;
            int indexIndividu2 = rand() % nbIndividus;
            while (indexIndividu1 == indexIndividu2) indexIndividu1 = rand() % nbIndividus;

            float r; // TODO génération nb aléatoire pour choix entre mutation et croisement


            // TODO reste de l'algo
        }


        compteurGeneration += 1;
    }
}

int main(int argc, char** argv) {
    /* TEST INIT
    Population pop = init(10);
    for (std::vector<int> tab : pop) {
        for (int i : tab) {
            std::cout << i ;
        }
        std::cout << std::endl;
    }
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