#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility> // Para std::pair

#include "age_blx_alpha.h"
#include "portfolio.h"

using namespace std;


ResultMHDouble AGE_BLX_ALPHA::optimize(ProblemDouble &problem, int maxevals) {
    this->evaluaciones = 0;
    this->mejor_fitness_global = -numeric_limits<double>::infinity(); 
    this->mejor_global.clear();

    vector<tSolution<double>> poblacion(TAM_POBLACION);
    vector<double> fitness_poblacion(TAM_POBLACION);

    // Inicializacion poblacion
    for(int i = 0; i < TAM_POBLACION; i++){
        poblacion[i] = problem.createSolution();
        fitness_poblacion[i] = problem.fitness(poblacion[i]);
        evaluaciones++;
        if(fitness_poblacion[i] > mejor_fitness_global){
            mejor_global = poblacion[i];
            mejor_fitness_global = fitness_poblacion[i];
        }
    }
    
    int tam_sol = problem.getSolutionSize();
    auto rango  = problem.getSolutionDomainRange();
    double lo = rango.first;
    double hi = rango.second;


    while(evaluaciones < maxevals){

        // Torneo
        int pos_1 = seleccionTorneo(poblacion, fitness_poblacion);
        int pos_2 = seleccionTorneo(poblacion, fitness_poblacion);
        
        tSolution<double> hijo1(tam_sol, 0.0);
        tSolution<double> hijo2(tam_sol, 0.0);
        cruceBLX(problem, poblacion[pos_1], poblacion[pos_2], tam_sol, hijo1, hijo2);
        mutar(problem, hijo1, tam_sol, lo , hi);
        mutar(problem, hijo2, tam_sol, lo , hi);

        double f1 = problem.fitness(hijo1);
        double f2 = problem.fitness(hijo2);
        evaluaciones+=2;

        int peor1 = 0;
        int peor2 = 0;
        seleccionaDosPeores(fitness_poblacion, peor1, peor2);

        if(f1 > fitness_poblacion[peor1]){
            fitness_poblacion[peor1] = f1;
            poblacion[peor1] = hijo1;
        }
        if(f2 > fitness_poblacion[peor2]){
            fitness_poblacion[peor2] = f2;
            poblacion[peor2] = hijo2;
        }
    }
    int indice_mejor = 0;
    seleccionaMejor(fitness_poblacion, indice_mejor);
    return ResultMH(poblacion[indice_mejor], fitness_poblacion[indice_mejor], evaluaciones);
}

void AGE_BLX_ALPHA::cruceBLX(Problem<double>& problem, const tSolution<double>& padre1, const tSolution<double>& padre2, int tam_sol, tSolution<double>& hijo1, tSolution<double>& hijo2){
        
    // Cruce BLX
    bool todos_cero_hijo_1 = true;
    bool todos_cero_hijo_2 = true;
    for (size_t j = 0; j < tam_sol; j++) {
        double cmin = min(padre1[j], padre2[j]);
        double cmax = max(padre1[j], padre2[j]);
        double I    = cmax - cmin;

        double lo_blx = cmin - I * ALPHA;
        double hi_blx = cmax + I * ALPHA;

        hijo1[j] = max(Random::get<double>(lo_blx, hi_blx), 0.0);
        // Hay que comprobar que todos los hijos no sean 0.0 (luego al hacer fix dividiria por 0.0)
        if(hijo1[j] != 0.0){todos_cero_hijo_1 = false;}
        hijo2[j] = max(Random::get<double>(lo_blx, hi_blx), 0.0);
        if(hijo2[j] != 0.0){todos_cero_hijo_2 = false;}
    }
    // Por seguridad
    // Nos quedamos con los padres si todos los valores de los hijos eran 0 (es decir el aleatorio era negativo siempre)
    if(todos_cero_hijo_1){
        hijo1 = padre1;
    }
    if(todos_cero_hijo_2){
        hijo2 = padre2;
    }
    // Reparamos los hijos para que cumplan las restricciones
    problem.fix(hijo1);
    problem.fix(hijo2);
}

void AGE_BLX_ALPHA::mutar(Problem<double>& problem, tSolution<double>& hijo, int tam_sol, double lo, double hi){
    double random = Random::get<double>(0.0, 1.0);
    if(random <= PROB_MUTACION){
        int indice_empresa_1  = Random::get<int>(0, tam_sol - 1);
        int indice_empresa_2  = Random::get<int>(0, tam_sol - 1);
        if(indice_empresa_1 != indice_empresa_2){
            double valor_anterior_i = hijo[indice_empresa_1];
            double valor_anterior_j = hijo[indice_empresa_2];
            if(valor_anterior_i > lo && valor_anterior_j < hi){
                double tope_1 = 1.0 - (lo / valor_anterior_i);
                double tope_2 = (hi - valor_anterior_j) / valor_anterior_i;
                double tope_3 = RATIO;
                // Nos quedamos con el menor tope y ese es lo maximo que podemos quitarle a i para darselo a j
                double ratio = min({tope_1, tope_2, tope_3});
                hijo[indice_empresa_1] = (1-ratio)*valor_anterior_i;
                hijo[indice_empresa_2] = valor_anterior_j +  ratio*valor_anterior_i;
            }
        }
    }
}

int AGE_BLX_ALPHA::seleccionTorneo(const vector<tSolution<double>>& poblacion, const vector<double>& fitness_pob){
    int indice_ganador = Random::get<int>(0, TAM_POBLACION-1);
    for (int j = 1; j < K_TORNEO; j++) {
        int indice_rival = Random::get<int>(0, TAM_POBLACION-1);
        if (fitness_pob[indice_rival] > fitness_pob[indice_ganador]) {
            indice_ganador = indice_rival;
        }
    }
    return indice_ganador;
}

void AGE_BLX_ALPHA::seleccionaDosPeores(const vector<double>& fitness_pob, int &indice_peor_1, int &indice_peor_2){
    
    double peor_fitness_local_1 = numeric_limits<double>::infinity();
    double peor_fitness_local_2 = numeric_limits<double>::infinity();
    int ind_peor_1 = 0;
    int ind_peor_2 = 0;
    // Aseguramos que peor_1 sea el peor de los dos iniciales
    if(fitness_pob[1] < fitness_pob[0]){
        ind_peor_1 = 0;
        ind_peor_2 = 1;
    } else {
        ind_peor_1 = 1;
        ind_peor_2 = 0;
    }

    for(int i = 2; i < TAM_POBLACION; i++){
        if(fitness_pob[i] < fitness_pob[ind_peor_1]){
            ind_peor_2 = ind_peor_1;   // el antiguo peor pasa a segundo peor
            ind_peor_1 = i;
        }
        else if(fitness_pob[i] < fitness_pob[ind_peor_2]){
            ind_peor_2 = i;
        }
    }
    indice_peor_1 = ind_peor_1;
    indice_peor_2 = ind_peor_2;
}

void AGE_BLX_ALPHA::seleccionaMejor(const vector<double>& fitness_pob, int &indice_mejor){
    double mejor_fitness_ = -numeric_limits<double>::infinity();
    int ind_mejor = 0;
    for(int i = 0; i < TAM_POBLACION; i++){
        // Ver si es mejor
        if(fitness_pob[i] > mejor_fitness_){
            mejor_fitness_ = fitness_pob[i];
            ind_mejor = i;
        }
    }
    indice_mejor = ind_mejor;
}
