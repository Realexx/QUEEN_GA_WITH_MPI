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
std::vector<int> rechercheMeilleur(Population &pop, bool modeSuppression = false) {
    int nbConflitsMeilleur = INFINITY; // On initialise la variable du nombre conflit sur un très gros nombre
    std::vector<int> meilleurIndividu = pop[0];
    size_t indexDernierElement = meilleurIndividu.size() - 1;
    int indexMeilleur;

    for (int i = 0; i < pop.size(); ++i) {
        int nbConflitIndividuCourant = pop[i][indexDernierElement];

        if (nbConflitIndividuCourant < nbConflitsMeilleur) {
            meilleurIndividu = pop[i];
            nbConflitsMeilleur = nbConflitIndividuCourant;
            indexMeilleur = i;
        }
    }
    if (modeSuppression)
        pop.erase(pop.begin() + indexMeilleur);

    return meilleurIndividu;
}

/**
 * Fonction qui recherche plusieurs meilleurs individus d'une population
 * @param pop la population sur laquelle rechercher les meilleurs individus
 * @param nb nombre de meilleurs individus à extraire
 * @return le tableau des meilleurs individus
 */
Population rechercheMeilleurs(Population &pop, int nb) {
    Population meilleurs;

    for (int i = 0; i < nb; ++i) {
        std::vector<int> meilleur = rechercheMeilleur(pop, true); // Appel de rechercheMeilleur en mode suppression
        meilleurs.push_back(meilleur); // Ajout du meilleur trouvé au tableau à renvoyer
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
 * Fonction utilisant MPI, pour gérer l'envoi d'un vector c++
 * @param vec le vector à envoyer
 * @param dest le processus de destination
 * @param tag l'étiquette
 * @param comm communicateur
 */
void mpiEnvoyerVec(std::vector<int>& vec, int dest, int tag, MPI_Comm comm) {
    int size = vec.size();
    MPI_Send(&size, 1, MPI_INT, dest, tag, comm);
    MPI_Send(vec.data(), size, MPI_INT, dest, tag, comm);
}

/**
 * Fonction utilisant MPI, pour gérer la réception d'un vector c++
 * @param vec le vector dans lequel les datas sont reçus
 * @param source le processus source
 * @param tag l'étiquette
 * @param comm communicateur
 */
void mpiRecevoirVec(std::vector<int>& vec, int source, int tag, MPI_Comm comm) {
    int size;
    MPI_Status status;
    MPI_Recv(&size, 1, MPI_INT, source, tag, comm, &status);
    vec.resize(size);
    MPI_Recv(vec.data(), size, MPI_INT, source, tag, comm, &status);
}

/**
 * Fonction résolvant le problème des reines avec MPI et 3 processus
 * @param nbIndividus taille des populations sur lesquelles il faudra itérer
 * @param taille taille de la grille sur laquelle résoudre le problème
 * @param nbGenerations nombre d'itérations maximum pour résoudre le problème
 * @param p probabilité de croisement (UNIQUEMENT STRATEGIE 1)
 * @param nbIterationAvantSwap nombre d'itérations avant chaque échange de meilleurs individus entre les processus
 * @param nbIndividuASwap nombre de meilleurs individus que s'échangent les processus
 * @param print activation de l'affichage ou non
 * @param mode2 activation de la stratégie 2 ou non
 * @param pCroisement probabilité de croisement (UNIQUEMENT STRATEGIE 2)
 * @param pMutation probabilité de mutation (UNIQUEMENT STRATEGIE 2)
 */
void QueenAlgorithm(int nbIndividus = 15, int taille = 4, int nbGenerations = 100, float p = 0.5,
                    int nbIterationAvantSwap = 3, int nbIndividuASwap = 5, bool print = true,
                    int mode2 = false, float pCroisement = 0.5, float pMutation = 0.5) {


    int rang, tag = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rang);
    MPI_Status status;

    srand(time(nullptr) + rang); // Pour assurer une meilleure génération des randoms


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
            Population tabAEnvoyer = rechercheMeilleurs(generation, nbIndividuASwap); // La fonction s'occupe de la suppression des individus
            Population tabRecu(nbIndividuASwap); // Initialisation du vector de reception en indiquant la taille (IMPORTANT)

            // Envois et réceptions pour chaque processus
            // Envois de 'tabAEnvoyer' (meilleurs individus)
            // Reception dans 'tabRecu'
            if (rang == 2) {
                // Envoi 1 vector par 1 vector
                for (std::vector<int> individusAEnvoyer : tabAEnvoyer) {
                    mpiEnvoyerVec(individusAEnvoyer, 0, tag, MPI_COMM_WORLD);
                }

                // Reçois 1 vector par 1 vector
                for (int i = 0; i < tabAEnvoyer.size(); ++i) {
                    mpiRecevoirVec(tabRecu[i], 1, tag, MPI_COMM_WORLD);
                }
            }
            if (rang == 1) {
                for (std::vector<int> individusAEnvoyer : tabAEnvoyer) {
                    mpiEnvoyerVec(individusAEnvoyer, 2, tag, MPI_COMM_WORLD);
                }

                for (int i = 0; i < tabAEnvoyer.size(); ++i) {
                    mpiRecevoirVec(tabRecu[i], 0, tag, MPI_COMM_WORLD);
                }
            }
            if (rang == 0) {
                for (std::vector<int> individusAEnvoyer : tabAEnvoyer) {
                    mpiEnvoyerVec(individusAEnvoyer, 1, tag, MPI_COMM_WORLD);
                }

                for (int i = 0; i < tabAEnvoyer.size(); ++i) {
                    mpiRecevoirVec(tabRecu[i], 2, tag, MPI_COMM_WORLD);
                }
            }

            // Injection du tableau reçu dans la génération courante du process
            for (const std::vector<int>& individu : tabRecu) {
                generation.push_back(individu);
            }

        }

        // Print pour chaque génération
        if (print) {
            // On affiche le meilleur individu pour chaque itération avec le nombre de conflits(s) en dernière position du tableau.
            std::cout << "(P" << rang << ") Iteration : " << compteurGeneration << " | Meilleur individu global : ";
            afficheIndividu(meilleurIndividuGlobal);
        }

        for (int i = 0; i < nbIndividus; ++i) { // Pour chaque individu de la génération

            if (!mode2) { // Stratégie 1
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
    // int nbConflitsElementFinal = meilleurIndividuGlobal[meilleurIndividuGlobal.size()-1];
    meilleurIndividuGlobal.pop_back(); // Suppression du nombre de conflits pour affichage
    afficheIndividu(meilleurIndividuGlobal);
    // std::cout << nbConflitsElementFinal;
    MPI_Abort(MPI_COMM_WORLD, MPI_SUCCESS); // Quand un processus a trouvé la solution, on coupe les processus après avoir affiché les résultats
    MPI_Finalize();
}

int main(int argc, char **argv) {
    // Initialisation de MPI
    MPI_Init(&argc, &argv);

    // ========================================== TESTS ======================================
    //QueenAlgorithm(); // Problème de base : 4x4, 100 itérations max, population : 15

    // ===== Test 10x10, boucle infinie, population : 10 =====
    // STRATEGIE 1
    // QueenAlgorithm(10,10, INFINITY, 0.5, 5, 5, false);
    // STRATEGIE 2
    // QueenAlgorithm(10,10, INFINITY, 0.5, 5, 10, false, true, 0.7, 0.2);

    // =======================================================================================

}
