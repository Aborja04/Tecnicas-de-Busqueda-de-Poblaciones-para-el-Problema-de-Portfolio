#pragma once
#include <mh.h>

using namespace std;

// Instanciamos la plantilla con el tipo que nos interese
using MHDouble = MH<double>;
using ProblemDouble = Problem<double>;
using ResultMHDouble = ResultMH<double>;

class FA : public MHDouble {
private:
    static constexpr double GAMMA = 2.0;  
    static constexpr double ALPHA = 0.005;  
    static constexpr double BETA_0 = 1;  
    static constexpr int N_LUCIERNAGAS = 15;

    int evaluaciones = 0;
    tSolution<double> mejor_global;
    double mejor_fitness_global = -9999999.0;

    double distanciaEuclideaCuadrado(const tSolution<double>& sol1, const tSolution<double>& sol2);
public:
    FA() : MH() {}
    virtual ~FA() {}
    // Implement the MH interface methods
    /**
     * Create random solutions looking moving to a better neighbour
     *
     * @param problem The problem to be optimized
     * @return A pair containing the best solution found and its fitness
     */
    virtual ResultMH<double> optimize(Problem<double> &problem, int maxevals);
};