#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility> // Para std::pair
#include <numeric>

#include "es_mod.h"
#include "portfolio.h"
#include "busqueda_local_auxiliar.h"

using namespace std;

ResultMHDouble ES_MOD::optimize(ProblemDouble &problem, int maxevals) {
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
    // Inicializacion
    tSolution<double> solucion = problem.createSolution();
    double fitness = problem.fitness(solucion);
    mejor_global = solucion;
    mejor_fitness_global = fitness;
    evaluaciones++;
    double T_inicial = MU*abs(fitness)/(-log(PHI));

    // La temperatura incial debe ser mayor a la final
    while(T_inicial <= T_FINAL){
        solucion = problem.createSolution();
        fitness = problem.fitness(solucion);
        mejor_global = solucion;
        mejor_fitness_global = fitness;
        evaluaciones++;
        T_inicial = MU*abs(fitness)/(-log(PHI));

    }

    int max_vecinos = CONSTANTE_VECINOS * tam_sol;
    int max_exitos = CONSTANTE_EXITOS * tam_sol;
    int M = maxevals/max_vecinos;   // Numero de enfriamientos
    bool hay_exito = true;

    double beta = (T_inicial - T_FINAL)/(M * T_inicial * T_FINAL * 1.0);
    double T = T_inicial;
    // Terminamos cuando llegamos al maximo de evaluaciones o el numero de exitos es 0 en la iteracion anterior
    // Condicion de parada
    while(evaluaciones < maxevals && hay_exito){

        int n_vecinos = 0;
        int n_exitos = 0;
        Random::shuffle(posiciones);
        int pos = 0;
        // Condicion enfriamiento
        while(n_vecinos < max_vecinos && n_exitos < max_exitos && evaluaciones < maxevals && pos < tam_pos){
            // Generamos el vecino
            int pos_i = posiciones[pos].first;
            int pos_j = posiciones[pos].second;
            if(pos_i != pos_j){
                double valor_anterior_i = solucion[pos_i];
                double valor_anterior_j = solucion[pos_j];
                if(valor_anterior_i > lo && valor_anterior_j < hi){
                    n_vecinos++;     
                    double tope_1 = 1.0 - (lo / valor_anterior_i);
                    double tope_2 = (hi - valor_anterior_j) / valor_anterior_i;
                    double tope_3 = RATIO;
                    // Nos quedamos con el menor tope y ese es lo maximo que podemos quitarle a i para darselo a j
                    double ratio = min({tope_1, tope_2, tope_3});
                    if(ratio == 0){
                        pos++;
                        continue;
                    }
                    solucion[pos_i] = (1-ratio)*valor_anterior_i;
                    solucion[pos_j] = valor_anterior_j +  ratio*valor_anterior_i;
                    if(problem.isValid(solucion)){   // hay que hacerlo porque por redondeo a veces se quedan < lo
                        double new_fit = problem.fitness(solucion);
                        evaluaciones++;
                        double incremento_fitness = fitness - new_fit;
                        double u = Random::get<double>(0.0, 1.0);
                        if(incremento_fitness < 0 || ( u <= exp(-incremento_fitness/(T*1.0)))){
                            fitness = new_fit;
                            // La solucion se mantiene como esta porque ya se han hecho los cambios, no hay que revertir nada
                            n_exitos++;
                            Random::shuffle(posiciones);
                            pos = 0;
                            if(fitness > mejor_fitness_global){
                                mejor_global = solucion;
                                mejor_fitness_global = fitness;
                            }
                        }
                        else{   //Recuperamos la solucion como era porque el cambio o no es valido o no mejora
                            solucion[pos_i] = valor_anterior_i;
                            solucion[pos_j] = valor_anterior_j;
                            pos++;
                            
                        }
                    }
                    else{
                        solucion[pos_i] = valor_anterior_i;
                        solucion[pos_j] = valor_anterior_j;
                        pos++;
                    }
                }
                else{
                    pos++;
                }
            }
            else{
                pos++;
            }
        }
        T = T / (1.0 + beta * T);
        hay_exito = n_exitos > 0;
        if(T < T_FINAL){
            // Perturbamos el mejor global encontrado
            solucion = mejor_global;
            mutar(solucion, numero_empresas_barajar);  // perturbación
            fitness = problem.fitness(solucion);
            evaluaciones++;

            // Recalculamos temperatura inicial desde la nueva solución
            T = MU * abs(fitness) / (-log(PHI));
            // Recalculamos beta con el presupuesto restante
            int M_restante = max(1, (maxevals - evaluaciones) / max_vecinos);
            beta = (T - T_FINAL) / (M_restante * T * T_FINAL);
        }
    }
    return ResultMH(mejor_global, mejor_fitness_global, evaluaciones);
}

void ES_MOD::inicializarPosiciones(int tam_sol){
    posiciones.clear();
    posiciones.reserve(tam_sol * tam_sol);
    for (int i = 0; i < tam_sol; ++i) {
        for (int j = 0; j < tam_sol; ++j) {
            posiciones.push_back({i, j});
        }
    }
}

void ES_MOD::mutar(tSolution<double>& solucion, int numero_empresas_barajar){
    Random::shuffle(indices_empresas);

    for(int i = 0; i < numero_empresas_barajar; i+=2){
        double sol_aux = solucion[indices_empresas[i]];
        solucion[indices_empresas[i]] = solucion[indices_empresas[i+1]];
        solucion[indices_empresas[i+1]] = sol_aux;
    }
}

void ES_MOD::inicializarIndicesEmpresas(int tam){
    indices_empresas.clear();
    indices_empresas.reserve(tam);
    for (int i = 0; i < tam; ++i) {
        indices_empresas.push_back(i);
    }
}