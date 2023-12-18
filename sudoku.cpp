#include <ga/GA1DArrayGenome.h>
#include <ga/GASimpleGA.h>
#include <ga/std_stream.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <random>
#include "sudoku_solver.cpp"

using namespace std;

const int POPULATION_SIZE = 5000;
const int MAX_GENERATIONS = 5000;
const float CROSSOVER_PROBABILITY = 0.05;
const float MUTATION_PROBABILITY = 0.1;

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

bool isNumberRepeated(int row, int col, int num, int **sudoku) {
    int count = 0;
    // Check row
    for (int i = 0; i < N; ++i) {
        if (sudoku[row][i] == num) {
            count++;
            if (count > 1) {
                return true;
            }
        }
    }
    // Check column
    count = 0;
    for (int i = 0; i < N; ++i) {
        if (sudoku[i][col] == num) {
            count++;
            if (count > 1) {
                return true;
            }
        }
    }
    // Check box
    count = 0;
    int boxStartRow = row - row % 3;
    int boxStartCol = col - col % 3;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (sudoku[boxStartRow + i][boxStartCol + j] == num) {
                count++;
                if (count > 1) {
                    return true;
                }
            }
        }
    }
    return false;
}


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
        cout << "valid " << N * N * N << endl;
        return N * N * N;
    } else {
        // Check how many numbers are incorrect
        int fitness = N * N;
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                int num = grid[row][col];
                // Check if the number is repeated in the same row, column or box
                if (isNumberRepeated(row, col, num, grid)) {
                    fitness--;
                }
            }
        }
        return (float) fitness;
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

    // Set values in each row such that each number from 1 to N appears exactly once
    for (int i = 0; i < N; ++i) {
        std::vector<int> rowValues;
        for (int j = 1; j <= N; ++j) {
            rowValues.push_back(j);
        }

        // Shuffle the values for the current row
        random_device rd;
        mt19937 generator(rd());
        shuffle(rowValues.begin(), rowValues.end(), generator);

        // Set the shuffled values in the current row of the grid
        for (int j = 0; j < N; ++j) {
            grid[i][j] = rowValues[j];
        }
    }

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
        if (pos1 > pos2) {
            swap(pos1, pos2);
        }
        // Move the second allele to follow the first, shifting the rest
        int tmp = genome.gene(pos2);
        for (int j = pos2; j > pos1; j--) {
            genome.gene(j, genome.gene(j - 1));
        }
        genome.gene(pos1 + 1, tmp);
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

        int cut = rand() % (N * N);
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

bool allZeros(const GA1DArrayGenome<int> &genome) {
    for (int i = 0; i < N * N; ++i) {
        if (genome.gene(i) != 0) {
            return false;
        }
    }
    return true;
}

bool backtrackRemoveNumbers(GA1DArrayGenome<int> &genome) {
    if (allZeros(genome)) {
        return true;
    }

    for (int i = 0; i < N * N; ++i) {
        if (genome.gene(i) != 0) {
            int originalValue = genome.gene(i);
            genome.gene(i, 0);

            if (isSolvable(grid)) {
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
    backtrackRemoveNumbers(bestGenomeCopy);
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

        // Output the best solution for each generation
        auto &bestGenome = (GA1DArrayGenome<int> &) ga.statistics().bestIndividual();
        float currentBestFitness = objective((GAGenome &) bestGenome);

        cout << "Generation " << generation + 1 << ": Fitness = " << currentBestFitness << endl;

        // Adjust parameters based on progress
        if (currentBestFitness > bestFitness) {
            // Fitness has improved
            generationsWithoutImprovement = 0;
            bestFitness = currentBestFitness;
        } else {
            // Fitness hasn't improved
            generationsWithoutImprovement++;
        }
        if (generationsWithoutImprovement > 100) {
            // If fitness hasn't improved for the last 10 generations, reduce population size
            populationSize = max(500, populationSize - 10); // Minimum population size is 500
        }
        if(generationsWithoutImprovement > 200){
            maxGenerations = maxGenerations - 50;
            generationsWithoutImprovement = 0;
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
