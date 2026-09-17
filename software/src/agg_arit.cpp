#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility> // Para std::pair

#include "agg_arit.h"
#include "portfolio.h"
#include "actualizador.h"

using namespace std;

ResultMHDouble AGG_Arit::optimize(ProblemDouble &problem, int maxevals) {
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
    vector<tSolution<double>> hijos(TAM_POBLACION);
    vector<double> fitness_hijos(TAM_POBLACION);
    vector<tSolution<double>> padres(TAM_POBLACION);
    vector<double> fitness_padres(TAM_POBLACION);
    vector<bool> indices_mutados(TAM_POBLACION);        // true si un indice se ha mutado

    Actualizador actualizador(NUM_CRUCES_ESPERADO, TAM_POBLACION);

    while(evaluaciones < maxevals){
        // Seleccion padres por torneo
        seleccionTorneo(poblacion, fitness_poblacion, padres, fitness_padres);
        cruceAritmetico(problem, padres, hijos, tam_sol);
        mutar(hijos, tam_sol, lo , hi, indices_mutados);
        int indice_mejor = 0;
        int indice_peor = 0;
        actualizador.ActualizarFitness(problem, fitness_padres, hijos, fitness_hijos, indice_mejor, indice_peor, indices_mutados, evaluaciones);       
        // Si el mejor hijo es peor que el mejor padre, entonces cambiamos el peor hijo por el mejor padre para no perderlo
        if(fitness_hijos[indice_mejor] < mejor_fitness_global){
            hijos[indice_peor] = mejor_global;
            fitness_hijos[indice_peor] = mejor_fitness_global;
            indice_mejor = indice_peor;     // El mejor es ahora el que era el peor
        }
        else{       // Tenemos un nuevo mejor global
            mejor_global = hijos[indice_mejor];
            mejor_fitness_global = fitness_hijos[indice_mejor];
        }
        
        // swap por eficiencia
        swap(poblacion, hijos);
        swap(fitness_poblacion, fitness_hijos);
    }

    
    return ResultMH(mejor_global, mejor_fitness_global, evaluaciones);
}

void AGG_Arit::cruceAritmetico(Problem<double>& problem, const vector<tSolution<double>>& padres, vector<tSolution<double>>& hijos, int tam_sol){
    for(int i = 0; i < TAM_POBLACION; i+=2){
        tSolution<double> hijo1 = padres[i];
        tSolution<double> hijo2 = padres[i+1];

        if(i < 2*NUM_CRUCES_ESPERADO){        // Cruzamos los NUM_CRUCES_ESPERADO PRIMEROS
            // Cruce Aritmético
            for (size_t j = 0; j < tam_sol; j++) {
                double r = Random::get<double>(0.0, 1.0);
                hijo1[j] = r * padres[i][j] + (1.0 - r) * padres[i+1][j];
                hijo2[j] = r * padres[i+1][j] + (1.0 - r) * padres[i][j];
            }
            
            problem.fix(hijo1);
            problem.fix(hijo2);
        }
        hijos[i] = hijo1;
        hijos[i+1] = hijo2;
    }
}

void AGG_Arit::mutar(vector<tSolution<double>>& hijos, int tam_sol, double lo, double hi, vector<bool>& indices_mutados){
    fill(indices_mutados.begin(), indices_mutados.end(), false);    // Reiniciamos el vector de indices mutados
    for(int i = 0; i < NUM_MUTACIONES_ESPERADO; i++){
        int indice_hijo = Random::get<int>(0, TAM_POBLACION - 1);
        int indice_empresa_1  = Random::get<int>(0, tam_sol - 1);
        int indice_empresa_2  = Random::get<int>(0, tam_sol - 1);
        if(indice_empresa_1 != indice_empresa_2){
            double valor_anterior_i = hijos[indice_hijo][indice_empresa_1];
            double valor_anterior_j = hijos[indice_hijo][indice_empresa_2];
            if(valor_anterior_i > lo && valor_anterior_j < hi){
                double tope_1 = 1.0 - (lo / valor_anterior_i);
                double tope_2 = (hi - valor_anterior_j) / valor_anterior_i;
                double tope_3 = RATIO;
                // Nos quedamos con el menor tope y ese es lo maximo que podemos quitarle a i para darselo a j
                double ratio = min({tope_1, tope_2, tope_3});
                if(ratio == 0){
                    continue;
                }
                hijos[indice_hijo][indice_empresa_1] = (1-ratio)*valor_anterior_i;
                hijos[indice_hijo][indice_empresa_2] = valor_anterior_j +  ratio*valor_anterior_i;
                indices_mutados[indice_hijo] = true;            // Se muta ese indice
            }
        }
    }
}

void AGG_Arit::seleccionTorneo(const vector<tSolution<double>>& poblacion, const vector<double>& fitness_pob, vector<tSolution<double>>& padres, vector<double>& fitness_padres){
    for (int i = 0; i < TAM_POBLACION; i++) {
        int indice_ganador = Random::get<int>(0, TAM_POBLACION-1);

        for (int j = 1; j < K_TORNEO; j++) {
            int indice_rival = Random::get<int>(0, TAM_POBLACION-1);
            if (fitness_pob[indice_rival] > fitness_pob[indice_ganador]) {
                indice_ganador = indice_rival;
            }
        }
        padres[i] = poblacion[indice_ganador];
        fitness_padres[i] = fitness_pob[indice_ganador];
    }
}
