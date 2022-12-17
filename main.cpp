#include <mpi.h>
#include <cstdio>
#include <iostream>

#include "vector"
#include <cstdlib>

using Population = std::vector<std::vector<int>>;

Population init(int nbIndividus) {
    std::vector<std::vector<int>> population;

    // Création de 'nbIndivus' individus générée aléatoirement
    for (int i = 0; i < nbIndividus; ++i) {
        std::vector<int> individuAAjouter;

        // Remplir l'individu avec des valeurs aléatoires
        for (int j = 0; j < 4 ; ++j) {
            individuAAjouter.push_back(rand() % 4);
        }

        // Ajouter l'individu au tableau 'population'
        population.push_back(individuAAjouter);

    }

    return population;
}

int evaluation(std::vector<int> individu) {
    int nbConflits = 0;

    return nbConflits;
}

void QueenAlgorithm(int nbIndidividus, int nbGenerations) {
    int countGeneration = 0;
    Population generation = init(nbIndidividus);

    std::vector<int> meilleurIndividuGlobal; // TODO Réussir à trier le tableau en fonction du nombre de conflits de chaque individu

    std::vector<int> copieMeilleurIndividuGlobal(meilleurIndividuGlobal.size()); // Copie de meilleurIndividuGlobal
    copy(meilleurIndividuGlobal.begin(), meilleurIndividuGlobal.end(), copieMeilleurIndividuGlobal.begin());



    while (countGeneration <= nbGenerations) { // Itération sur les générations
        // Print pour chaque génération
        std::cout << "Iteration 1 : " << countGeneration << " | Meilleur individu global : [ ";
        for (int value : meilleurIndividuGlobal) {
            std::cout << value << " ";
        }
        std::cout << ']' << std::endl;

        for (int i = 0; i < nbIndidividus; ++i) { // Pour chaque individu
            int indexIndividu1 = rand() % nbIndidividus;
            int indexIndividu2 = rand() % nbIndidividus;
            while (indexIndividu1 == indexIndividu2) indexIndividu1 = rand() % nbIndidividus;

            float r; // TODO génération nb aléatoire pour choix entre mutation et croisement


            // TODO reste de l'algo
        }


        countGeneration += 1;
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
