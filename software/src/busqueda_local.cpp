#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility> // Para std::pair

#include "busqueda_local.h"
#include "portfolio.h"

using namespace std;



/**
 * Búsqueda Local: Implementación del método optimize
 */
ResultMHDouble BusquedaLocal::optimize(ProblemDouble &problem, int maxevals) {
    
    // Usamos una solucion aleatoria como base
    tSolution<double> sol = problem.createSolution();

    // Generamos el vector de parejas de naturales desde (0, 0) hasta (sol.size(), sol.size())
    size_t tam_sol = problem.getSolutionSize();
    vector<pair<int, int>> posiciones;
    int tam_pos = tam_sol*tam_sol;
    // Reserva de memoria para ser mas rapido
    posiciones.reserve(tam_pos);

    // Inicializamos el vector con los pares (0,0), (0,1)... hasta (n-1, n-1)
    for (size_t i = 0; i < tam_sol; ++i) {
        for (size_t j = 0; j < tam_sol; ++j) {
            posiciones.push_back(make_pair(i, j));
        }
    }
    // Mezclamos el vector usando la librería Random proporcionada
    Random::shuffle(posiciones);
    int pos = 0;
    double fit = problem.fitness(sol);
    int evals_realizadas = 0;
    pair<double, double> rango = problem.getSolutionDomainRange();
    double lo = rango.first;
    double hi = rango.second;
    double ratio_max=0.4;
    while(pos < tam_pos and evals_realizadas < maxevals){
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
                    evals_realizadas++;
                    if(new_fit > fit){
                        Random::shuffle(posiciones);
                        pos = 0;
                        fit = new_fit;
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
    return ResultMH(sol, fit, evals_realizadas);
}