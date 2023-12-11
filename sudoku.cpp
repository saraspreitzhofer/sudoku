#include <ga/GA1DArrayGenome.h>
#include <ga/GASimpleGA.h>
#include <ga/std_stream.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include "sudoku_solver.cpp"

using namespace std;

const int POPULATION_SIZE = 500;
const int MAX_GENERATIONS = 2000;
const float CROSSOVER_PROBABILITY = 0.1;
const float MUTATION_PROBABILITY = 0.05;

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
 the lesser numbers remain the better.  // todo
optional: If your sudoku solver is able to return "the choices made" during solving the problem -
 this could be also included in the fittnes function. The more choices have to be made,
 the more complex is the sudoku-problem - therefore the better the sudoku.*/

bool isNumberRepeated(int row, int col, int num, int **sudoku) {
    int count = 0;
    // Check row
    for (int i = 0; i < N; ++i) {
        if (sudoku[row][i] == num) {
            count++;
            if (count > 2) {
                return true;
            }
        }
    }
    // Check column
    count = 0;
    for (int i = 0; i < N; ++i) {
        if (sudoku[i][col] == num) {
            count++;
            if (count > 2) {
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
                if (count > 2) {
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

void sudokuGridFromGenome(const GA1DArrayGenome<int> &genome) {
    genomeToGrid(genome);
    sudokuGrid(grid);
    cout << endl;
}

// Objective function
float objective(GAGenome &g) {
    auto &genome = (GA1DArrayGenome<int> &) g;
    int fitness = N * N;
    genomeToGrid(genome);
    // Check how many numbers are incorrect
    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++) {
            int num = grid[row][col];
            // Check if the number is repeated in the same row, column, or box
            if (isNumberRepeated(row, col, num, grid)) {
                fitness--;
            }
        }
    }
    if (checkSudoku(grid)) fitness = 1000;
    return (float) fitness;
}

// Initializer
void initializer(GAGenome &g) {
    auto &genome = (GA1DArrayGenome<int> &) g;

    grid = new int *[N];
    for (int i = 0; i < N; ++i) {
        grid[i] = new int[N];
        for (int j = 0; j < N; ++j) {
            grid[i][j] = genome.gene(i * N + j, rand() % N + 1);
        }
    }
}

// Mutator
int mutator(GAGenome &g, float p) {
    auto &genome = (GA1DArrayGenome<int> &) g;

    int nMutations = 0;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (GAFlipCoin(p)) {
                int randRow = rand() % N;
                int randCol = rand() % N;
                // Swap two numbers in the grid
                int temp = genome.gene(i * N + j);
                genome.gene(i * N + j, genome.gene(randRow * N + randCol));
                genome.gene(randRow * N + randCol, temp);

                nMutations++;
            }
        }
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

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    GA1DArrayGenome<int> genome(N * N, objective);
    genome.initializer(initializer);
    genome.mutator(mutator);
    genome.crossover(crossover);

    GASimpleGA ga(genome);
    ga.populationSize(POPULATION_SIZE);
    ga.nGenerations(MAX_GENERATIONS);
    ga.pMutation(MUTATION_PROBABILITY);
    ga.pCrossover(CROSSOVER_PROBABILITY);
    ga.evolve();

    // Output the best Sudoku board
    const GA1DArrayGenome<int> &bestGenome = (GA1DArrayGenome<int> &) ga.statistics().bestIndividual();
    cout << "Best solution found: " << endl;
    cout << "Fitness: " << objective((GAGenome &) bestGenome) << endl;
    sudokuGridFromGenome(bestGenome);

    // free the allocated memory
    for (int i = 0; i < N; ++i) {
        delete[] grid[i];
    }
    delete[] grid;
    return 0;
}
