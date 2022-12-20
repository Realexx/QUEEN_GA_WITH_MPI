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
        individuAAjouter.push_back(nbConflitsIndividuAAjouter); //

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

void QueenAlgorithm(int nbIndividus, int taille, int nbGenerations, float p) {
    srand (static_cast <unsigned> (time(nullptr))); // Pour la randomisation
    int compteurGeneration = 0;
    Population generation = init(nbIndividus, taille);

    std::vector<int> meilleurIndividuGlobal = rechercheMeilleur(generation);

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

            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

            if (r < p) {
                copieMeilleurIndividuGlobal = croisement();
            } else {
                copieMeilleurIndividuGlobal = mutation();
            }

            if (copieMeilleurIndividuGlobal[copieMeilleurIndividuGlobal.size()-1] <= generation[i][copieMeilleurIndividuGlobal.size()-1]) {
                for (int j = 0; j < copieMeilleurIndividuGlobal.size(); ++j) {
                    generation[i][j] = copieMeilleurIndividuGlobal[j];
                }

            }
            // TODO reste de l'algo
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
