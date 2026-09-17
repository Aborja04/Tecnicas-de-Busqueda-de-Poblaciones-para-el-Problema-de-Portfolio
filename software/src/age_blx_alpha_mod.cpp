#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility> // Para std::pair

#include "age_blx_alpha_mod.h"
#include "portfolio.h"

using namespace std;


ResultMHDouble AGE_BLX_ALPHA_MOD::optimize(ProblemDouble &problem, int maxevals) {
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
        double f1 = 0.0;
        double f2 = 0.0;
        cruceBLX(problem, poblacion[pos_1], poblacion[pos_2], tam_sol, hijo1, hijo2, f1, f2);

        
        evaluaciones+=4;

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

void AGE_BLX_ALPHA_MOD::cruceBLX(Problem<double>& problem, const tSolution<double>& padre1, const tSolution<double>& padre2, int tam_sol, tSolution<double>& hijo1, tSolution<double>& hijo2, double &f1, double &f2){
    
    tSolution<double> hijo_1(tam_sol, 0.0);
    tSolution<double> hijo_2(tam_sol, 0.0);
    tSolution<double> hijo_3(tam_sol, 0.0);
    tSolution<double> hijo_4(tam_sol, 0.0);

    // Cruce BLX
    bool todos_cero_hijo_1 = true;
    bool todos_cero_hijo_2 = true;
    bool todos_cero_hijo_3 = true;
    bool todos_cero_hijo_4 = true;
    for (size_t j = 0; j < tam_sol; j++) {
        double cmin = min(padre1[j], padre2[j]);
        double cmax = max(padre1[j], padre2[j]);
        double I    = cmax - cmin;

        double lo_blx = cmin - I * ALPHA;
        double hi_blx = cmax + I * ALPHA;

        hijo_1[j] = max(Random::get<double>(lo_blx, hi_blx), 0.0);
        // Hay que comprobar que todos los hijos no sean 0.0 (luego al hacer fix dividiria por 0.0)
        if(hijo_1[j] != 0.0){todos_cero_hijo_1 = false;}
        hijo_2[j] = max(Random::get<double>(lo_blx, hi_blx), 0.0);
        if(hijo_2[j] != 0.0){todos_cero_hijo_2 = false;}
        
        hijo_3[j] = max(Random::get<double>(lo_blx, hi_blx), 0.0);
        if(hijo_3[j] != 0.0){todos_cero_hijo_3 = false;}
        hijo_4[j] = max(Random::get<double>(lo_blx, hi_blx), 0.0);
        if(hijo_4[j] != 0.0){todos_cero_hijo_4 = false;}
    }
    // Por seguridad
    // Nos quedamos con los padres si todos los valores de los hijos eran 0 (es decir el aleatorio era negativo siempre)
    if(todos_cero_hijo_1){
        hijo_1 = padre1;
    }
    if(todos_cero_hijo_2){
        hijo_2 = padre2;
    }
    if(todos_cero_hijo_3){
        hijo_3 = padre1;
    }
    if(todos_cero_hijo_4){
        hijo_4 = padre2;
    }
    // Reparamos los hijos para que cumplan las restricciones
    problem.fix(hijo_1);
    problem.fix(hijo_2);
    problem.fix(hijo_3);
    problem.fix(hijo_4);

    double fit_1 = problem.fitness(hijo_1);
    double fit_2 = problem.fitness(hijo_2);
    double fit_3 = problem.fitness(hijo_3);
    double fit_4 = problem.fitness(hijo_4);
    int indice_1 = 0;
    int indice_2 = 0;
    obtenerDosMejores(fit_1, fit_2, fit_3, fit_4, indice_1, indice_2);
    if(indice_1 == 0){
        hijo1 = hijo_1;
        f1 = fit_1;
    }else if(indice_2 == 0){
        hijo2 = hijo_1;
        f2 = fit_1;
    }
    if(indice_1 == 1){
        hijo1 = hijo_2;
        f1 = fit_2;
    }else if(indice_2 == 1){
        hijo2 = hijo_2;
        f2 = fit_2;
    }
    if(indice_1 == 2){
        hijo1 = hijo_3;
        f1 = fit_3;
    }else if(indice_2 == 2){
        hijo2 = hijo_3;
        f2 = fit_3;
    }
    if(indice_1 == 3){
        hijo1 = hijo_4;
        f1 = fit_4;
    }else if(indice_2 == 3){
        hijo2 = hijo_4;
        f2 = fit_4;
    }
}

void AGE_BLX_ALPHA_MOD::obtenerDosMejores(double v0, double v1, double v2, double v3, int& indice_1, int& indice_2) {
    // Inicializamos con los dos primeros elementos de manera hipotetica
    double max1 = v0;
    indice_1 = 0;
    
    double max2 = v1;
    indice_2 = 1;

    // Aseguramos que max1 sea estrictamente el mayor de estos dos iniciales
    if (max2 > max1) {
        std::swap(max1, max2);
        std::swap(indice_1, indice_2);
    }

    // Evaluamos el tercer valor (v2)
    if (v2 > max1) {
        max2 = max1;
        indice_2 = indice_1;
        max1 = v2;
        indice_1 = 2;
    } else if (v2 > max2) {
        max2 = v2;
        indice_2 = 2;
    }

    // Evaluamos el cuarto valor (v3)
    if (v3 > max1) {
        max2 = max1;
        indice_2 = indice_1;
        max1 = v3;
        indice_1 = 3;
    } else if (v3 > max2) {
        max2 = v3;
        indice_2 = 3;
    }
}


int AGE_BLX_ALPHA_MOD::seleccionTorneo(const vector<tSolution<double>>& poblacion, const vector<double>& fitness_pob){
    int indice_ganador = Random::get<int>(0, TAM_POBLACION-1);
    for (int j = 1; j < K_TORNEO; j++) {
        int indice_rival = Random::get<int>(0, TAM_POBLACION-1);
        if (fitness_pob[indice_rival] > fitness_pob[indice_ganador]) {
            indice_ganador = indice_rival;
        }
    }
    return indice_ganador;
}

void AGE_BLX_ALPHA_MOD::seleccionaDosPeores(const vector<double>& fitness_pob, int &indice_peor_1, int &indice_peor_2){
    
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

void AGE_BLX_ALPHA_MOD::seleccionaMejor(const vector<double>& fitness_pob, int &indice_mejor){
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
