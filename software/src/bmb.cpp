#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility> // Para std::pair
#include <numeric>

#include "bmb.h"
#include "portfolio.h"
#include "busqueda_local_auxiliar.h"

using namespace std;

ResultMHDouble BMB::optimize(ProblemDouble &problem, int maxevals) {
    this->evaluaciones = 0;

    int tam_sol = problem.getSolutionSize();
    auto rango  = problem.getSolutionDomainRange();
    double lo = rango.first;
    double hi = rango.second;
   
    // Inicializamos el vector de posiciones para solo tener que hacerlo una vez
    inicializarPosiciones(tam_sol);
    int tam_pos = tam_sol*tam_sol;

    // Instancia de clase Busqueda Local
    BusquedaLocalAuxiliar bl(lo, hi, RATIO, MAX_EVALS_BL, maxevals);

    vector<tSolution<double>> soluciones(N_SOLUCIONES_INICIALES);
    vector<double> fitness_soluciones(N_SOLUCIONES_INICIALES);


    // Inicializacion soluciones iniciales
    for(int i = 0; i < N_SOLUCIONES_INICIALES; i++){
        soluciones[i] = problem.createSolution();
        fitness_soluciones[i] = problem.fitness(soluciones[i]);
        evaluaciones++;
        int evals_BL = 0;
        bl.aplicarBL(problem, soluciones[i], fitness_soluciones[i], posiciones, tam_pos, evals_BL);
        evaluaciones+=evals_BL;
    }

    int indice_mejor = seleccionaMejor(fitness_soluciones);

    return ResultMH(soluciones[indice_mejor], fitness_soluciones[indice_mejor], evaluaciones);
}

void BMB::inicializarPosiciones(int tam_sol){
    posiciones.clear();
    posiciones.reserve(tam_sol * tam_sol);
    for (int i = 0; i < tam_sol; ++i) {
        for (int j = 0; j < tam_sol; ++j) {
            posiciones.push_back({i, j});
        }
    }
}

int BMB::seleccionaMejor(const vector<double>& fitness_sol){
    double mejor =  fitness_sol[0];
    int pos_mejor = 0;
    for(int i = 1; i < N_SOLUCIONES_INICIALES; i++){
        if(fitness_sol[i] > mejor){
            pos_mejor = i;
            mejor = fitness_sol[i];
        }
    }
    return pos_mejor;
}