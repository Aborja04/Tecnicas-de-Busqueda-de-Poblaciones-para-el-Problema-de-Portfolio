#pragma once
#include <mh.h>

using namespace std;

// Instanciamos la plantilla con el tipo que nos interese
using MHDouble = MH<double>;
using ProblemDouble = Problem<double>;
using ResultMHDouble = ResultMH<double>;

class BMB : public MHDouble {
private:
    vector<pair<int, int>> posiciones;      // Posiciones para BL
    static constexpr int N_SOLUCIONES_INICIALES = 5;
    static constexpr int MAX_EVALS_BL = 1999;    
    static constexpr double RATIO = 0.40;  
    int evaluaciones = 0;
    

    void inicializarPosiciones(int tam_sol);
    int seleccionaMejor(const vector<double>& fitness_sol);
public:
    BMB() : MH() {}
    virtual ~BMB() {}
    // Implement the MH interface methods
    /**
     * Create random solutions looking moving to a better neighbour
     *
     * @param problem The problem to be optimized
     * @return A pair containing the best solution found and its fitness
     */
    virtual ResultMH<double> optimize(Problem<double> &problem, int maxevals);
};