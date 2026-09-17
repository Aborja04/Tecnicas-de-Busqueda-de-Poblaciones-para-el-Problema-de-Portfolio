#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility> // Para std::pair
#include <numeric>

#include "ils.h"
#include "portfolio.h"
#include "busqueda_local_auxiliar.h"

using namespace std;

ResultMHDouble ILS::optimize(ProblemDouble &problem, int maxevals) {
    this->evaluaciones = 0;
    this->mejor_fitness_global = -numeric_limits<double>::infinity(); 
    this->mejor_global.clear();

    int tam_sol = problem.getSolutionSize();
    auto rango  = problem.getSolutionDomainRange();
    double lo = rango.first;
    double hi = rango.second;
    int numero_empresas_barajar = PORCENTAJE_MUTACION * tam_sol;
    // Inicializamos el vector de posiciones para solo tener que hacerlo una vez
    inicializarPosiciones(tam_sol);
    int tam_pos = tam_sol*tam_sol;
    vector<int> indices_empresas(tam_sol);
    inicializarIndicesEmpresas(tam_sol);

    // Instancia de clase Busqueda Local
    BusquedaLocalAuxiliar bl(lo, hi, RATIO, MAX_EVALS_BL, maxevals);


    // Inicializamos con una solucion aleatoria
    mejor_global = problem.createSolution();
    mejor_fitness_global = problem.fitness(mejor_global);
    evaluaciones++;

    // Aplicamos la primera BL
    int evals_BL = 0;
    // puedo llamar con el mejor global directamente porque en BL siempre mejoro
    bl.aplicarBL(problem, mejor_global, mejor_fitness_global, posiciones, tam_pos, evals_BL);
    evaluaciones+=evals_BL;
    for(int i = 1; i < N_VUELTAS; i++){
        // Mutamos la mejor solucion
        tSolution<double> solucion_aux = mejor_global;
        mutar(solucion_aux, numero_empresas_barajar);
        double fitness_aux = problem.fitness(solucion_aux);
        evaluaciones++;
        evals_BL = 0;
        bl.aplicarBL(problem, solucion_aux, fitness_aux, posiciones, tam_pos, evals_BL);
        evaluaciones+=evals_BL;

        // Actualizamos si hemos mejorado la global
        if(fitness_aux > mejor_fitness_global){
            mejor_global = solucion_aux;
            mejor_fitness_global = fitness_aux;
        }

    }

    return ResultMH(mejor_global, mejor_fitness_global, evaluaciones);
}


void ILS::mutar(tSolution<double>& solucion, int numero_empresas_barajar){
    Random::shuffle(indices_empresas);

    for(int i = 0; i < numero_empresas_barajar; i+=2){
        double sol_aux = solucion[indices_empresas[i]];
        solucion[indices_empresas[i]] = solucion[indices_empresas[i+1]];
        solucion[indices_empresas[i+1]] = sol_aux;
    }
}


void ILS::inicializarPosiciones(int tam_sol){
    posiciones.clear();
    posiciones.reserve(tam_sol * tam_sol);
    for (int i = 0; i < tam_sol; ++i) {
        for (int j = 0; j < tam_sol; ++j) {
            posiciones.push_back({i, j});
        }
    }
}

void ILS::inicializarIndicesEmpresas(int tam){
    indices_empresas.clear();
    indices_empresas.reserve(tam);
    for (int i = 0; i < tam; ++i) {
        indices_empresas.push_back(i);
    }
}