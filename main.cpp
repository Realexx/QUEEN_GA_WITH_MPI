#include <mpi.h>
#include <iostream>
#include "vector"
#include <cstdlib>
#include <random>
#include <utility>
#include <algorithm>

using Population = std::vector<std::vector<int>>;

/**
 * Fonction d'affichage d'un individu
 * @param individu l'individu à afficher
 */
void afficheIndividu(const std::vector<int> &individu) {
    std::cout << "[ ";
    for (int x: individu) {
        std::cout << x << " ";
    }
    std::cout << "]" << std::endl;
}

/**
 * Fonction d'évaluation d'un individu
 * @param individu individu à évaluer
 * @return le nombre de conflits de l'individu passé en paramètre
 */
int evaluation(const std::vector<int> &individu) {
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
        for (int j = 0; j < taille; ++j) {
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
std::vector<int> rechercheMeilleur(const Population &pop) {
    int nbConflitsMeilleur = 6; // On initialise la variable du nombre conflit au nb de conflits maximum possible
    std::vector<int> meilleurIndividu = pop[0];
    size_t indexDernierElement = meilleurIndividu.size() - 1;

    for (std::vector<int> individu: pop) {
        int nbConflitIndividuCourant = individu[indexDernierElement];

        if (nbConflitIndividuCourant < nbConflitsMeilleur) {
            meilleurIndividu = individu;
            nbConflitsMeilleur = nbConflitIndividuCourant;
        }
    }
    return meilleurIndividu;
}

/**
 * Fonction qui recherche plusieurs meilleurs individus d'une population
 * @param pop la population sur laquelle rechercher les meilleurs individus
 * @param nb nombre de meilleurs individus à extraire
 * @return le tableau des meilleurs individus
 */
Population rechercheMeilleurs(const Population &pop, int nb) {
    Population populationSub = pop;
    Population meilleurs;

    for (int i = 0; i < nb; ++i) {
        std::vector<int> meilleur = rechercheMeilleur(populationSub); // Appel de rechercheMeilleur
        meilleurs.push_back(meilleur); // Ajout du meilleur trouvé au tableau à renvoyer
        populationSub.erase(std::remove(populationSub.begin(), populationSub.end(), meilleur), populationSub.end());
        // Pb : Si plusieurs éléments sont identiques dans les meilleurs, il n'y en aura que 1 qui sera considéré et donc renvoyé par la fonction
        // ⇛ Pas plus mal pour la diversité cependant
    }

    return meilleurs;
}

/**
 * Fonction permettant de croiser un individu avec un autre
 * @param individu1
 * @param individu2
 * @return un individu issu du croisement entre 'individu1' et 'individu2'
 */
std::vector<int> croisement(std::vector<int> individu1, std::vector<int> individu2) {
    std::vector<int> individuApresCroisement = std::move(individu1);
    std::vector<int> individu = std::move(individu2);
    individuApresCroisement.pop_back(); // Suppression de l'évaluation précédente

    size_t nbElementsACroiser = individuApresCroisement.size() / 2;

    /*
    if (individuApresCroisement.size() % 2 == 0) {
        nbElementsACroiser =
                individuApresCroisement.size() / 2; // Exemple pour un tableau de 4 → On croisera 4/2 = 2 éléments.
    } else {
        nbElementsACroiser = individuApresCroisement.size() / 2 + 1; // Exemple pour un tableau de 5 → On croisera 5/2 + 1 = 3 éléments.
    }
     */

    // Génération de la liste des index des éléments qui vont être remplacés par des éléments de l'individu n°2
    std::vector<int> listeIndexAChanger;
    for (int i = 0; i < nbElementsACroiser; ++i) {
        int random = rand() % individuApresCroisement.size();

        for (int x: listeIndexAChanger) {
            while (x == random) random = rand() % individuApresCroisement.size(); // Vérification des doublons
        }

        listeIndexAChanger.push_back(random);
    }

    // Remplacement
    for (int indexAChanger: listeIndexAChanger) {
        individuApresCroisement[indexAChanger] = individu[indexAChanger];
    }

    // Ré-évaluation
    individuApresCroisement.push_back(evaluation(individuApresCroisement));

    return individuApresCroisement;
}

/** @deprecated mélanger les individus à muter n'est pas une bonne chose
 * Fonction permettant de muter un individu en mélangeant ses éléments
 * @param individu l'individu à muter
 * @return l'individu muté
 */
std::vector<int> mutation(std::vector<int> individu) {
    std::vector<int> individuApresMutation = std::move(individu);
    individuApresMutation.pop_back(); // Suppression de l'évaluation précédente

    // Ici, on fait au plus simple, la mutation d'un individu est représenté par un mélange de ses éléments.
    std::shuffle(individuApresMutation.begin(), individuApresMutation.end(), std::mt19937(std::random_device()()));

    // Ré-évaluation
    individuApresMutation.push_back(evaluation(individuApresMutation));

    return individuApresMutation;
}

/**
 * Fonction permettant de muter un individu en modifiant une minorité de ses éléments de manière aléatoire
 * @param individu l'individu à muter
 * @return l'individu muté
 */
std::vector<int> mutationV2(std::vector<int> individu) {
    std::vector<int> individuApresMutation = std::move(individu);
    individuApresMutation.pop_back();

    size_t nbElementsAMuter = individuApresMutation.size() / 3;

    // Génération de la liste des index des éléments qui vont être remplacés par d'autres éléments aléatoires
    std::vector<int> listeIndexAMuter;
    for (int i = 0; i < nbElementsAMuter; ++i) {
        int random = rand() % individuApresMutation.size();

        for (int x : listeIndexAMuter) {
            while (x == random) random = rand() % individuApresMutation.size();
        }

        listeIndexAMuter.push_back(random);
    }

    // Remplacement
    for (int indexAChanger: listeIndexAMuter) {
        int random = rand() % individuApresMutation.size();
        while (random == individuApresMutation[indexAChanger]) random = rand() % individuApresMutation.size();
        individuApresMutation[indexAChanger] = random;
    }

    // Ré-évaluation
    individuApresMutation.push_back(evaluation(individuApresMutation));

    return individuApresMutation;

}

/**
 * Fonction générale qui résout le problème des reines
 * @param nbIndividus nombre d'individus par générations
 * @param taille taille du tableau dans lequel résoudre le problème des reines (Ex : si taille = 4 -> tableau 4x4)
 * @param nbGenerations nombre de génération à itérer
 * @param p probabilité de croisement
 */
void QueenAlgorithm(int nbIndividus = 15, int taille = 4, int nbGenerations = 100, float p = 0.5,
                    int nbIterationAvantSwap = 3, int nbIndividuASwap = 5, bool print = true,
                    int mode = 0, float pCroisement = 0.5, float pMutation = 0.5) {

    int nbProcessus, rang;
    MPI_Comm_size(MPI_COMM_WORLD, &nbProcessus);
    MPI_Comm_rank(MPI_COMM_WORLD, &rang);

    srand(time(nullptr) + rang); // Pour assurer une meilleure génération des randoms

    // printf("Processus %d sur %d processus\n", rang, nbProcessus);

    int compteurGeneration = 0;
    Population generation = init(nbIndividus, taille);

    std::vector<int> meilleurIndividuGlobal = rechercheMeilleur(generation);

    std::vector<int> x(meilleurIndividuGlobal.size()); // Copie de meilleurIndividuGlobal dans une variable 'x'.
    copy(meilleurIndividuGlobal.begin(), meilleurIndividuGlobal.end(), x.begin());

    bool solution = false;

    while (compteurGeneration <= nbGenerations && !solution) { // Itération sur les générations
        // Toutes les 'nbIterationAvantSwap' iterations, trouver les 'nbIndividuASwap' meilleurs individus de la population
        // et les envoyer au processus 'rang + 1' sauf si c'est le processus 2 il envoie au processus 0
        if (compteurGeneration % nbIterationAvantSwap == 0) {
            Population tabToSend = rechercheMeilleurs(generation, nbIndividuASwap);

            if (rang == 2)
                return; // TODO A faire
        }

        // Print pour chaque génération
        if (print) {
            // On affiche le meilleur individu pour chaque itération avec le nombre de conflits(s) en dernière position du tableau.
            std::cout << "(P" << rang << ") Iteration : " << compteurGeneration << " | Meilleur individu global : ";
            afficheIndividu(meilleurIndividuGlobal);
        }

        for (int i = 0; i < nbIndividus; ++i) { // Pour chaque individu de la génération

            if (mode == 0) { // Stratégie 1
                float r = static_cast <float> (rand()) /
                          static_cast <float> (RAND_MAX); // Génération d'un float entre 0 et 1.

                if (r < p) { // Soit, on effectue un croisement
                    x = croisement(meilleurIndividuGlobal,
                                   generation[i]); // Croisement entre le meilleur individu et l'individu courant
                } else { // Soit une mutation
                    x = mutationV2(generation[i]); // Mutation de l'individu courant
                }

            } else { // Stratégie 2
                // Génération de 2 randoms, un pour la mutation et un pour le croisement
                float rCroisement = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                float rMutation = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

                bool estCroise = rCroisement < pCroisement;
                bool estMute = rMutation < pMutation;

                if (estCroise) { // Si croisement + mutation => effectuer la mutation sur l'individu croisé au préalable
                    x = croisement(meilleurIndividuGlobal,generation[i]);
                    if (estMute) {
                        x = mutationV2(x);
                    }
                } else if (estMute) {
                    x = mutationV2(generation[i]);
                }
            }

            if (x[x.size() - 1] <= generation[i][x.size() - 1]) {
                // Comparaison du nombre de conflits entre l'individu courant et l'individu qui a été généré via mutation ou croisement (variable 'x').
                // Si l'individu généré est meilleur → remplacement de l'individu courant dans la génération
                for (int j = 0; j < x.size(); ++j) {
                    generation[i][j] = x[j];
                }
            }

            std::vector<int> value;
            value = generation[i];

            if (meilleurIndividuGlobal[meilleurIndividuGlobal.size() - 1] >= value[value.size() - 1])
                // Si cet individu est meilleur (ou tout autant meilleur (cela permet d'avoir plus de diversité dans la variable)) que le meilleur global on remplace le meilleur global en faisant une copie.
                std::copy(value.begin(), value.end(), meilleurIndividuGlobal.begin());
        }
        compteurGeneration += 1;
        if (meilleurIndividuGlobal[meilleurIndividuGlobal.size() - 1] == 0) {
            solution = true; // Si une solution est trouvée → On s'arrête.
            std::cout << "(P" << rang << ") Une solution a été trouvée ! \n" << " -> ";
        }
    }
    std::cout << "Meilleur individu trouvé (en " << compteurGeneration-1 << " itérations) : ";
    int nbConflitsElementFinal = meilleurIndividuGlobal[meilleurIndividuGlobal.size()-1];
    meilleurIndividuGlobal.pop_back(); // Suppression du nombre de conflits pour affichage
    afficheIndividu(meilleurIndividuGlobal);
    std::cout << "(P" << rang << ") Nombre de conflit(s) : " << nbConflitsElementFinal << " conflit(s).";
}

int main(int argc, char **argv) {
    // Initialisation de MPI
    MPI_Init(&argc, &argv);

    // ========================================== TESTS ======================================
    // QueenAlgorithm();
    // QueenAlgorithm(50, 6, 250, 0.5, false, 0, 0.5, 0.5);
    // QueenAlgorithm(10, 5, 1000);
    // QueenAlgorithm(100, 8, 1000, 0.5);
    QueenAlgorithm(75,10, INFINITY, 0.5, 5, 5, false);
    // QueenAlgorithm(75,10, INFINITY, 0.5, false, 1, 0.7, 0.6);

    MPI_Finalize();

}
