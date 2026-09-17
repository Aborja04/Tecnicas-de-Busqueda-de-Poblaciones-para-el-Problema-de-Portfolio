#pragma once
#include <mh.h>

using namespace std;

// Instanciamos la plantilla con el tipo que nos interese
using MHDouble = MH<double>;
using ProblemDouble = Problem<double>;
using ResultMHDouble = ResultMH<double>;

class AGE_BLX_ALPHA : public MHDouble {
private:
    static constexpr int TAM_POBLACION = 100;
    static constexpr int K_TORNEO = 3;
    static constexpr double PROB_MUTACION = 0.1;
    static constexpr double RATIO = 0.15;  
    static constexpr double ALPHA = 0.3;  
    int evaluaciones = 0;
    tSolution<double> mejor_global;
    double mejor_fitness_global = -9999999.0;

    
    // Realiza el torneo y devuelve una posicion de la poblacion
    int seleccionTorneo(const vector<tSolution<double>>& poblacion, const vector<double>& fitness_pob);

    // Aplica el cruce blx a los padres
    void cruceBLX(Problem<double>& problem, const tSolution<double>& padre1, const tSolution<double>& padre2, int tam_sol, tSolution<double>& hijo1, tSolution<double>& hijo2);

    void mutar(Problem<double>& problem, tSolution<double>& hijo, int tam_sol, double lo, double hi);
    void seleccionaDosPeores(const vector<double>& fitness_pob, int &indice_peor_1, int &indice_peor_2);
    void seleccionaMejor(const vector<double>& fitness_pob, int &indice_mejor);
public:
    AGE_BLX_ALPHA() : MH() {}
    virtual ~AGE_BLX_ALPHA() {}
    // Implement the MH interface methods
    /**
     * Create random solutions looking moving to a better neighbour
     *
     * @param problem The problem to be optimized
     * @return A pair containing the best solution found and its fitness
     */
    virtual ResultMH<double> optimize(Problem<double> &problem, int maxevals);
};