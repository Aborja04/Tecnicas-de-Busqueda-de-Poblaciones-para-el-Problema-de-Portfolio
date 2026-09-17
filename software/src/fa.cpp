#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility> // Para std::pair
#include <numeric>

#include "fa.h"
#include "portfolio.h"
#include "busqueda_local_auxiliar.h"

using namespace std;

ResultMHDouble FA::optimize(ProblemDouble &problem, int maxevals) {
    this->evaluaciones = 0;
    this->mejor_fitness_global = -numeric_limits<double>::infinity(); 
    this->mejor_global.clear();

    int tam_sol = problem.getSolutionSize();
    auto rango  = problem.getSolutionDomainRange();
    double lo = rango.first;
    double hi = rango.second;
   
    // La aleatoriedad dependera del rango y del tamanio de la sol
    //double alpha = lo *(50/tam_sol + 2);
    double alpha = 3*lo;

    // Inicializacion
    vector<tSolution<double>> luciernagas(N_LUCIERNAGAS);
    vector<double> intensidad(N_LUCIERNAGAS);

    // Inicializacion poblacion
    for(int i = 0; i < N_LUCIERNAGAS; i++){
        luciernagas[i] = problem.createSolution();
        intensidad[i] = problem.fitness(luciernagas[i]);
        evaluaciones++;
        if(intensidad[i] > mejor_fitness_global){
            mejor_global = luciernagas[i];
            mejor_fitness_global = intensidad[i];
        }
    }


    // La temperatura incial debe ser mayor a la final
    while(evaluaciones < maxevals){
        for(int i = 0; i < N_LUCIERNAGAS && evaluaciones < maxevals; i++){
            int contador_mejores = 0;
            for(int j = 0; j < N_LUCIERNAGAS && evaluaciones < maxevals; j++){
                if(intensidad[j] > intensidad[i]){
                    contador_mejores++;
                    double rand = Random::get<double>(0.0, 1.0);
                    double r_2 = distanciaEuclideaCuadrado(luciernagas[i], luciernagas[j]);
                    for(int t = 0; t < tam_sol; t++){
                        //double ind_rand = Random::get<int>(0, 1);
                        //double rand = Random::get<double>(0.0, 1.0);
                        luciernagas[i][t] = max(0.0, luciernagas[i][t] + BETA_0 * exp(-GAMMA*r_2) * (luciernagas[j][t] - luciernagas[i][t]) + alpha*(rand - 0.5));
                        //luciernagas[i][ind_rand] = max(0.0, luciernagas[i][ind_rand] + BETA_0 * exp(-GAMMA*r_2) * (luciernagas[j][ind_rand] - luciernagas[i][ind_rand]) + alpha*(rand - 0.5));
                    }
                    // Reparamos
                    problem.fix(luciernagas[i]);
                    evaluaciones++;
                    // Actualizamos intensidad
                    intensidad[i] = problem.fitness(luciernagas[i]);
                    if(intensidad[i] > mejor_fitness_global){
                        mejor_global = luciernagas[i];
                        mejor_fitness_global = intensidad[i];
                    }
                }
            }
            // La mejor la movemos de forma aleatoria
            if(contador_mejores == 0){
                double rand = Random::get<double>(0.0, 1.0);
                for(int t = 0; t < tam_sol; t++){
                        //double ind_rand = Random::get<int>(0, 1);
                        //double rand = Random::get<double>(0.0, 1.0);
                        //luciernagas[i][ind_rand] = max(0.0, luciernagas[i][ind_rand] + alpha*(rand - 0.5));
                        luciernagas[i][t] = max(0.0, luciernagas[i][t] + alpha*(rand - 0.5));

                }
                problem.fix(luciernagas[i]);
                intensidad[i] = problem.fitness(luciernagas[i]);
                evaluaciones++;
                if(intensidad[i] > mejor_fitness_global){
                    mejor_global = luciernagas[i];
                    mejor_fitness_global = intensidad[i];
                }
            }
        }
        //alpha = alpha * 0.98;

    }

    return ResultMH(mejor_global, mejor_fitness_global, evaluaciones);
}


double FA::distanciaEuclideaCuadrado(const tSolution<double>& sol1, const tSolution<double>& sol2){
    double suma = 0.0;
    for(size_t i = 0; i < sol1.size(); i++){
        double diff = sol1[i] - sol2[i];
        suma += diff * diff;
    }
    return suma;
}