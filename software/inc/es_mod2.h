#pragma once
#include <mh.h>

using namespace std;

// Instanciamos la plantilla con el tipo que nos interese
using MHDouble = MH<double>;
using ProblemDouble = Problem<double>;
using ResultMHDouble = ResultMH<double>;

class ES_MOD2 : public MHDouble {
private:
    vector<pair<int, int>> posiciones;      // Posiciones para ES
    static constexpr double RATIO = 0.40;  

    static constexpr double MU = 0.2;
    static constexpr double PHI = 0.3; 
    static constexpr double T_FINAL = 1e-3;
    static constexpr int CONSTANTE_VECINOS = 10;
    static constexpr int CONSTANTE_EXITOS = 1;

    int evaluaciones = 0;
    tSolution<double> mejor_global;
    double mejor_fitness_global = -9999999.0;

    void inicializarPosiciones(int tam_sol);
public:
    ES_MOD2() : MH() {}
    virtual ~ES_MOD2() {}
    // Implement the MH interface methods
    /**
     * Create random solutions looking moving to a better neighbour
     *
     * @param problem The problem to be optimized
     * @return A pair containing the best solution found and its fitness
     */
    virtual ResultMH<double> optimize(Problem<double> &problem, int maxevals);
};