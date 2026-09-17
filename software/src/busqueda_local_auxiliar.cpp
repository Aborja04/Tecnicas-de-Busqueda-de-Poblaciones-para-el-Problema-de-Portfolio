#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility> // Para std::pair

#include "busqueda_local_auxiliar.h"
#include "portfolio.h"

using namespace std;

void BusquedaLocalAuxiliar::aplicarBL(Problem<double>& problem, tSolution<double>& sol, double& fit, vector<pair<int,int>>& posiciones, int tam_pos, int& evaluaciones){
    Random::shuffle(posiciones);
    int pos = 0;
    int maximo_evaluaciones_BL = min(evaluaciones + max_evals_BL, max_evals_global);
    while(pos < tam_pos and evaluaciones < maximo_evaluaciones_BL){
            int pos_i = posiciones[pos].first;
            int pos_j = posiciones[pos].second;
            if(pos_i != pos_j){
                double valor_anterior_i = sol[pos_i];
                double valor_anterior_j = sol[pos_j];
                if(valor_anterior_i > lo && valor_anterior_j < hi){
                    double tope_1 = 1.0 - (lo / valor_anterior_i);
                    double tope_2 = (hi - valor_anterior_j) / valor_anterior_i;
                    double tope_3 = ratio_max;
                    // Nos quedamos con el menor tope y ese es lo maximo que podemos quitarle a i para darselo a j
                    double ratio = min({tope_1, tope_2, tope_3});
                    if(ratio == 0){
                        pos++;
                        continue;
                    }
                    sol[pos_i] = (1-ratio)*valor_anterior_i;
                    sol[pos_j] = valor_anterior_j +  ratio*valor_anterior_i;
                    if(problem.isValid(sol)){   // hay que hacerlo porque por redondeo a veces se quedan < lo
                        double new_fit = problem.fitness(sol);
                        evaluaciones++;
                        if(new_fit > fit){
                            fit = new_fit;
                            Random::shuffle(posiciones);
                            pos = 0;
                        }
                        else{   //Recuperamos la solucion como era porque el cambio o no es valido o no mejora
                            sol[pos_i] = valor_anterior_i;
                            sol[pos_j] = valor_anterior_j;
                            pos++;
                            
                        }
                    }
                    else{
                        sol[pos_i] = valor_anterior_i;
                        sol[pos_j] = valor_anterior_j;
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
}