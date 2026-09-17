#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility> // Para std::pair

#include "greedy.h"
#include "portfolio.h"

using namespace std;

// Función auxiliar siguiendo el estilo del Greedy
template <class T> 
void print_vector(string name, const vector<T> &sol) {
  cout << name << ": ";
  for (auto elem : sol) {
    cout << elem << ", ";
  }
  cout << endl;
}

/**
 * Búsqueda Local: Implementación del método optimize
 */
ResultMHDouble Greedy::optimize(ProblemDouble &problem, int maxevals) {
    
    Portfolio realproblem = dynamic_cast<Portfolio &>(problem);
    size_t tam_sol = problem.getSolutionSize();
    vector<double> heuristicas;
    // Reserva de memoria para ser mas rapido
    heuristicas.reserve(tam_sol);

    int T = realproblem.getPeriodo();
    for(size_t i = 0; i < tam_sol; i++){
        // Aqui calcular la heuristica
        double suma_covarianzas = 0.0;
        for(int t = 0; t < tam_sol; t++){
            suma_covarianzas+=realproblem.getCovarianza(t, i);
        }
        heuristicas.push_back((realproblem.getSumaLog(i) - realproblem.getLambda()*sqrt(suma_covarianzas/ (tam_sol*1.0))));
    }

    vector<int> indices;
    indices.reserve(tam_sol);
    for(int i = 0; i < tam_sol; i++){
        indices.push_back(i);
    }
    // Usamos una función "Lambda" (el [&]) para comparar los valores reales de mayor a menor (>).
    sort(indices.begin(), indices.end(), [&](int a, int b) {
        return heuristicas[a] > heuristicas[b]; 
    });

    tSolution<double> sol(tam_sol);
    double a_repartir = 1.0;
    double hi = realproblem.getHi();
    for(int i = 0; i < tam_sol; i++){
        if(a_repartir > 0.0){
            double asignacion = min(hi, a_repartir);
            sol[indices[i]] = asignacion;
            a_repartir-=asignacion;
        }else{
            sol[indices[i]] = 0.0;
        }
    }
    return ResultMH(sol, problem.fitness(sol), 1);
}