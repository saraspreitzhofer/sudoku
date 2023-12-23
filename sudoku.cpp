#include <ga/GA1DArrayGenome.h>
#include <ga/GASimpleGA.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <random>
#include "sudoku_solver.cpp"

using namespace std;

const int POPULATION_SIZE = 20000;
const int MAX_GENERATIONS = 5000;
const float CROSSOVER_PROBABILITY = 0.01;
float MUTATION_PROBABILITY = 0.05;

int **grid;

/*Please create sudoku boards that can be solved in a unique way (there is only one solution) in C++.
The lesser numbers there are in the sudoku the better - the more complex the sudoku is the better
(complexity can be measured in how much choices there are in the CSP (contrain-satisfactory-problem)
You can use any sudoku solver (C/C++) you can find for this problem e.g.:
https://www.tutorialspoint.com/sudoku-solver-in-cplusplus
https://stackoverflow.com/questions/19022739/sudoku-solver-in-c
Tip:
1) create a fully filled 9x9-field (numbers 1-9 in domains, lines and rows)
2) modify the Sodoku-field via EA, to create a valid fully filled Sudoku (checked with a given sudoku solver)
 (similar to the 8-queens problem)
3) remove one number at a time and try to solve the sodoku with a given sudoku solver -
 the lesser numbers remain the better.
optional: If your sudoku solver is able to return "the choices made" during solving the problem -
 this could be also included in the fittnes function. The more choices have to be made,
 the more complex is the sudoku-problem - therefore the better the sudoku.*/


// Convert the genome to a Sudoku grid
void genomeToGrid(const GA1DArrayGenome<int> &genome) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            grid[i][j] = genome.gene(i * N + j);
        }
    }
}

// Objective function
float objective(GAGenome &g) {
    auto &genome = (GA1DArrayGenome<int> &) g;
    genomeToGrid(genome);

    if (checkSudoku(grid)) {
        return N * N * N * 2;
    } else {
        // Check how many numbers are incorrect
        int fitness = N * N * N;
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                int num = grid[row][col];
                // Check if the number is repeated in the same row, column or box
                int repetitions = isNumberRepeated(row, col, num, grid);
                if (repetitions > 0) {
                    fitness = fitness - repetitions;
                }
            }
        }
        return (float) fitness;
    }
}

void fillRemainingCells() {
    for (int row = 0; row < N; ++row) {
        for (int col = 0; col < N; ++col) {
            std::vector<int> randomValues;
            for (int i = 1; i <= N; ++i) {
                randomValues.push_back(i);
            }
            random_device rd;
            mt19937 generator(rd());
            shuffle(randomValues.begin(), randomValues.end(), generator);

            if (grid[row][col] == 0) {
                for (int i = 0; i < N; ++i) {
                    if (!isPresentInRow(row, randomValues[i], grid)) {
                        grid[row][col] = randomValues[i];
                        break;
                    }
                }
            }
        }
    }
}

// Initializer
void initializer(GAGenome &g) {
    auto &genome = (GA1DArrayGenome<int> &) g;

    grid = new int *[N];
    for (int i = 0; i < N; ++i) {
        grid[i] = new int[N];
        for (int j = 0; j < N; ++j) {
            grid[i][j] = 0;  // Initialize to 0 initially
        }
    }

    // Set values in each box so that each number from 1 to N appears exactly once
    for (int box = 0; box < 3; ++box) {
        std::vector<int> boxValues;
        for (int i = 1; i <= N; ++i) {
            boxValues.push_back(i);
        }

        // Shuffle the values for the current box
        random_device rd;
        mt19937 generator(rd());
        shuffle(boxValues.begin(), boxValues.end(), generator);

        // Set the shuffled values in the current box
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                grid[box * 3 + row][col + box * 3] = boxValues[row * 3 + col];
            }
        }
    }
    fillRemainingCells();

// Set the genome with the values from the grid
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            genome.gene(i * N + j, grid[i][j]);
        }
    }
}

// Mutator
int mutator(GAGenome &g, float p) {
    auto &genome = (GA1DArrayGenome<int> &) g;
    int nMutations = 0;
    if (GAFlipCoin(p)) {
        // Pick two random positions
        int pos1 = rand() % (N * N);
        int pos2 = rand() % (N * N);
        // Ensure pos1 and pos2 are different
        while (pos2 == pos1) {
            pos2 = rand() % (N * N);
        }
        int tmp = genome.gene(pos1);
        genome.gene(pos1, genome.gene(pos2));
        genome.gene(pos2, tmp);

        nMutations++;
    }
    return nMutations;
}

// Crossover
int crossover(const GAGenome &p1, const GAGenome &p2, GAGenome *c1, GAGenome *c2) {
    auto &parent1 = (GA1DArrayGenome<int> &) p1;
    auto &parent2 = (GA1DArrayGenome<int> &) p2;
    if (c1 && c2) {
        auto &child1 = (GA1DArrayGenome<int> &) *c1;
        auto &child2 = (GA1DArrayGenome<int> &) *c2;

        // cut at the end of line 3 or 6
        int cut = ((rand() % 2) + 1) * 3 * N;
        for (int i = 0; i < N * N; i++) {
            if (i < cut) {
                child1.gene(i, parent1.gene(i));
                child2.gene(i, parent2.gene(i));
            } else {
                child1.gene(i, parent2.gene(i));
                child2.gene(i, parent1.gene(i));
            }
        }
        return 2;
    } else if (c1) {
        auto &child = (GA1DArrayGenome<int> &) *c1;
        int cut = rand() % (N * N);
        for (int i = 0; i < N * N; i++) {
            if (i < cut) {
                child.gene(i, parent1.gene(i));
            } else {
                child.gene(i, parent2.gene(i));
            }
        }
        return 1;
    } else {
        return 0;
    }
}

bool backtrackRemoveNumbers(GA1DArrayGenome<int> &genome) {
    genomeToGrid(genome);
    if(countZeros(grid) > 55) return true;
    for (int i = 0; i < N * N; ++i) {
        if (genome.gene(i) != 0) {
            int originalValue = genome.gene(i);
            genome.gene(i, 0);
            genomeToGrid(genome);

            if (solveSudoku(grid)) {
                if (backtrackRemoveNumbers(genome)) {
                    // If the remaining sudoku is solvable, we found a solution
                    return true;
                }
            }

            // Revert the change
            genome.gene(i, originalValue);
        }
    }
    return false;  // No solution found
}

void removeNumbers(GA1DArrayGenome<int> &bestGenome) {
    GA1DArrayGenome<int> bestGenomeCopy = bestGenome;
    genomeToGrid(bestGenomeCopy);
    if (backtrackRemoveNumbers(bestGenomeCopy)) {
        // If backtracking was successful, update the original bestGenome
        bestGenome = bestGenomeCopy;
    }
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    GA1DArrayGenome<int> genome(N * N, objective);
    genome.initializer(initializer);
    genome.mutator(mutator);
    genome.crossover(crossover);

    GASimpleGA ga(genome);
    ga.initialize();
    ga.pMutation(MUTATION_PROBABILITY);
    ga.pCrossover(CROSSOVER_PROBABILITY);

    int populationSize = POPULATION_SIZE;
    int maxGenerations = MAX_GENERATIONS;
    int generationsWithoutImprovement = 0;
    float bestFitness = 0.0;

    for (int generation = 0; generation < maxGenerations; ++generation) {
        // Update population size and generations based on the generation number
        ga.populationSize(populationSize);
        ga.nGenerations(maxGenerations - generation);

        // Evolve for the current generation
        ga.step();

        auto &bestGenome = (GA1DArrayGenome<int> &) ga.statistics().bestIndividual();
        float currentBestFitness = objective((GAGenome &) bestGenome);

        cout << "Generation " << generation + 1 << ": Fitness = " << currentBestFitness << endl;

        if (currentBestFitness > bestFitness) {
            generationsWithoutImprovement = 0;
            bestFitness = currentBestFitness;
        } else {
            generationsWithoutImprovement++;
        }
        // If fitness hasn't improved for the last 200 generations
        if (generationsWithoutImprovement > 200) {
            generationsWithoutImprovement = 0;
            maxGenerations = maxGenerations - 50;

            // reduce population size
            populationSize = max(1000, populationSize - 10); // Minimum population size is 1000

            // increase mutation probability
            if (MUTATION_PROBABILITY < 0.2) {
                MUTATION_PROBABILITY += 0.01;
                ga.pMutation(MUTATION_PROBABILITY);
            }
        }
        if (currentBestFitness >= N * N * N) break;
    }

    // Output the best Sudoku board
    auto &bestGenome = (GA1DArrayGenome<int> &) ga.statistics().bestIndividual();
    cout << "Best solution found: " << endl;
    cout << "Fitness: " << objective((GAGenome &) bestGenome) << endl;
    genomeToGrid(bestGenome);
    sudokuGrid(grid);

    if (isSolvable(grid)) {
        removeNumbers(bestGenome);
        genomeToGrid(bestGenome);
        sudokuGrid(grid);
        isSolvable(grid);
    }

    // free the allocated memory
    for (int i = 0; i < N; ++i) {
        delete[] grid[i];
    }
    delete[] grid;
    return 0;
}