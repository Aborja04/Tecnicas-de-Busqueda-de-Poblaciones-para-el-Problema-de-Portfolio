#pragma once
#include <mh.h>

using namespace std;

// Instanciamos la plantilla con el tipo que nos interese
using MHDouble = MH<double>;
using ProblemDouble = Problem<double>;
using ResultMHDouble = ResultMH<double>;

class AGG_BLX_ALPHA : public MHDouble {
private:
    static constexpr int TAM_POBLACION = 100;
    static constexpr int K_TORNEO = 3;
    static constexpr double PROB_MUTACION = 0.1;
    static constexpr double PROB_CRUCE = 0.8;
    static constexpr double RATIO = 0.15;
    static constexpr double ALPHA = 0.3;   
    static constexpr int NUM_CRUCES_ESPERADO = static_cast<int>(PROB_CRUCE * TAM_POBLACION / 2);
    static constexpr int NUM_MUTACIONES_ESPERADO = static_cast<int>(PROB_MUTACION * TAM_POBLACION);
    int evaluaciones = 0;
    tSolution<double> mejor_global;
    double mejor_fitness_global = -numeric_limits<double>::infinity();

    
    // Realiza el torneo binario para rellenar el vector de padres
    void seleccionTorneo(const vector<tSolution<double>>& poblacion, const vector<double>& fitness_pob, vector<tSolution<double>>& padres, vector<double>& fitness_padres);

    // Aplica el cruce blx a los padres
    void cruceBLX(Problem<double>& problem, const vector<tSolution<double>>& padres, vector<tSolution<double>>& hijos, int tam_sol);
    
    void mutar( vector<tSolution<double>>& hijos, int tam_sol, double lo, double hi, vector<bool>& indices_mutados);
        
public:
    AGG_BLX_ALPHA() : MH() {}
    virtual ~AGG_BLX_ALPHA() {}
    // Implement the MH interface methods
    /**
     * Create random solutions looking moving to a better neighbour
     *
     * @param problem The problem to be optimized
     * @return A pair containing the best solution found and its fitness
     */
    virtual ResultMH<double> optimize(Problem<double> &problem, int maxevals);
};