#include <cassert>
#include <iostream>

#include "actualizador.h"
#include "portfolio.h"

using namespace std;

void Actualizador::ActualizarFitness(Problem<double>& problem, const vector<double>& fitness_padres, const vector<tSolution<double>>& hijos, vector<double>& fitness_hijos, int &indice_mejor_fitness, int &indice_peor_fitness, vector<bool>& indices_mutados, int &evaluaciones){
    double mejor_fitness_local = -numeric_limits<double>::infinity();
    double peor_fitness_local = numeric_limits<double>::infinity();
    int indice_mejor = 0;
    int indice_peor = 0;

    for(int i = 0; i < tam_poblacion; i++){
        if(i < 2*num_cruces_esperado or indices_mutados[i]){
            fitness_hijos[i] = problem.fitness(hijos[i]);
            evaluaciones++;
        }
        else{
            fitness_hijos[i] = fitness_padres[i];
        }
        

        // Ver si es mejor
        if(fitness_hijos[i] > mejor_fitness_local){
            mejor_fitness_local = fitness_hijos[i];
            indice_mejor = i;
        }
        // Ver si es el peor
        if(fitness_hijos[i] < peor_fitness_local){
            peor_fitness_local = fitness_hijos[i];
            indice_peor = i;
        }
    }
    indice_mejor_fitness = indice_mejor;
    indice_peor_fitness = indice_peor;
}