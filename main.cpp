#include <mpi.h>
#include <iostream>
#include "vector"
#include <cstdlib>
#include <utility>

using Population = std::vector<std::vector<int>>;

/**
 * Fonction d'évaluation d'un individu
 * @param individu individu à évaluer
 * @return le nombre de conflits de l'individu passé en paramètre
 */
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

/**
 * Fonction d'initialisation d'une population
 * @param nbIndividus nombre d'individus dans la population générée
 * @param taille taille de chaque individu dans la population
 * @return la population générée
 */
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
        // Exemple (taille = 4) : Individu [2,0,3,1] -> 0 conflit(s) -> [2,0,3,1,0] -> Meilleur solution
        int nbConflitsIndividuAAjouter = evaluation(individuAAjouter);
        individuAAjouter.push_back(nbConflitsIndividuAAjouter);

        // Ajouter l'individu au tableau 'population'
        population.push_back(individuAAjouter);

    }
    return population;
}

/**
 * Fonction qui recherche le meilleur individu d'une population donnée, si il y a plusieurs meilleurs individus avec le même nombre de conflit retourne celui croisé en premier
 * dans la population
 * @param pop la population sur laquelle on recherche le meilleur individu
 * @return le meilleur individu de la population passé en paramètre
 */
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

/**
 * Fonction permettant de croiser un individu avec un autre
 * @param individu1
 * @param individu2
 * @return un individu issu du croisement entre 'individu1' et 'individu2'
 */
std::vector<int> croisement(std::vector<int> individu1, std::vector<int> individu2) {
    std::vector<int> individuApresCroisement = std::move(individu1);
    // Tirage aléatoire des deux index à remplacer pour effectuer le croisement
    int indexAChanger1 = rand() % individuApresCroisement.size()-1;
    int indexAChanger2 = rand() % individuApresCroisement.size()-1;
    while (indexAChanger1 == indexAChanger2) indexAChanger1 = rand() % individuApresCroisement.size()-1;

    // Croisement
    individuApresCroisement[indexAChanger1] = individu2[indexAChanger1];
    individuApresCroisement[indexAChanger2] = individu2[indexAChanger2];

    // Evaluation du nouvel individu
    individuApresCroisement[individuApresCroisement.size()-1] = evaluation(individuApresCroisement);

    return individuApresCroisement;
}

/**
 * Fonction permettant de muter un individu en changeant aléatoirement un de ses éléments
 * @param individu l'individu à muter
 * @return l'individu muté
 */
std::vector<int> mutation(std::vector<int> individu) {
    std::vector<int> individuApresMutation = std::move(individu);

    size_t tailleIndividu = individuApresMutation.size();
    int r = rand() % tailleIndividu - 1;
    int r2 = rand() % tailleIndividu - 1;
    while (individuApresMutation[r] == r2) r2 = rand() % tailleIndividu - 1;
    individuApresMutation[r] = r2;

    // Evaluation du nouvel individu
    individuApresMutation[individuApresMutation.size()-1] = evaluation(individuApresMutation);

    return individuApresMutation;
}

/**
 * Fonction générale qui résout le problème des reines
 * @param nbIndividus nombre d'individus par générations
 * @param taille taille du tableau dans lequel résoudre le problème des reines (Ex : si taille = 4 -> tableau 4x4)
 * @param nbGenerations nombre de génération à itérer
 * @param p probabilité de croisement
 */
void QueenAlgorithm(int nbIndividus = 10, int taille = 4, int nbGenerations = 50, float p = 0.2) {
    srand (static_cast <unsigned> (time(nullptr))); // Pour la randomisation
    int compteurGeneration = 0;
    Population generation = init(nbIndividus, taille);

    std::vector<int> meilleurIndividuGlobal = rechercheMeilleur(generation);

    std::vector<int> x(meilleurIndividuGlobal.size()); // Copie de meilleurIndividuGlobal dans une variable 'x'.
    copy(meilleurIndividuGlobal.begin(), meilleurIndividuGlobal.end(), x.begin());

    bool solution = false;

    while (compteurGeneration <= nbGenerations && !solution) { // Itération sur les générations
        // Print pour chaque génération
        std::cout << "Iteration : " << compteurGeneration << " | Meilleur individu global : [ ";
        for (int value : meilleurIndividuGlobal) {
            std::cout << value << " ";
        }
        std::cout << ']' << std::endl;

        for (int i = 0; i < nbIndividus; ++i) { // Pour chaque individu de la génération
            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX); // Génération d'un float entre 0 et 1.

            if (r < p) {
                x = croisement(meilleurIndividuGlobal, generation[i]); // Croisement entre le meilleur individu et l'individu courant // TODO
            } else {
                x = mutation(generation[i]); // Mutation de l'individu courant // TODO
            }

            if (x[x.size() - 1] <= generation[i][x.size() - 1]) { // Comparaison du nombre de conflits entre l'individu courant et l'individu qui a été généré via mutation ou croisement (variable 'x').
                // Si l'individu généré est meilleur → remplacement de l'individu courant dans la génération
                for (int j = 0; j < x.size(); ++j) {
                    generation[i][j] = x[j];
                }
            }
            std::vector<int> value;
            value = generation[i];
            // std::copy(generation[i].begin(), generation[i].end(), value.begin()); // Copie de l'individu courant dans 'value'.


            if (meilleurIndividuGlobal[meilleurIndividuGlobal.size()-1] > value[value.size()-1]) // Si cet individu est meilleur que le meilleur global on remplace le meilleur global en faisant une copie.
                std::copy(value.begin(), value.end(), meilleurIndividuGlobal.begin());
        }
        compteurGeneration += 1;
        if (meilleurIndividuGlobal[meilleurIndividuGlobal.size()-1] == 0) solution = true; // Si une solution est trouvée → On s'arrête.
    }
}

int main(int argc, char** argv) {
    srand (static_cast <unsigned> (time(nullptr)));
    QueenAlgorithm();

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
