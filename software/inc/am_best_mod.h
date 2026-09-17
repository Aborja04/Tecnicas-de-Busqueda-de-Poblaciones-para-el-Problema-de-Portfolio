#pragma once
#include <mh.h>

using namespace std;

// Instanciamos la plantilla con el tipo que nos interese
using MHDouble = MH<double>;
using ProblemDouble = Problem<double>;
using ResultMHDouble = ResultMH<double>;

class AM_BEST_MOD : public MHDouble {
private:
    vector<pair<int, int>> posiciones;      // Posiciones para BL
    static constexpr int TAM_POBLACION = 100;
    static constexpr int MAX_EVALS_BL = 500;
    static constexpr int N_MEJORES = 0.1*TAM_POBLACION;
    static constexpr int K_TORNEO = 3;
    static constexpr double PROB_MUTACION = 0.1;
    static constexpr double PROB_CRUCE = 0.8;
    static constexpr double RATIO = 0.15;  
    static constexpr double ALPHA = 0.3;  
    static constexpr int NUM_CRUCES_ESPERADO = static_cast<int>(PROB_CRUCE * TAM_POBLACION / 2);
    static constexpr int NUM_MUTACIONES_ESPERADO = static_cast<int>(PROB_MUTACION * TAM_POBLACION);
    int evaluaciones = 0;
    tSolution<double> mejor_global;
    double mejor_fitness_global = -9999999.0;

    
    // Realiza el torneo binario para rellenar el vector de padres
    void seleccionTorneo(const vector<tSolution<double>>& poblacion, const vector<double>& fitness_pob, vector<tSolution<double>>& padres, vector<double>& fitness_padres);
    // Aplica el cruce blx
    void cruceBLX(Problem<double>& problem, const vector<tSolution<double>>& padres, vector<tSolution<double>>& hijos, int tam_sol);
    void mutar( vector<tSolution<double>>& hijos, int tam_sol, double lo, double hi, vector<bool>& indices_mutados);
    void inicializarPosiciones(int tam_sol);
    int seleccionaMejor(const vector<double>& fitness_pob);
    // Devuelve los indices de los N mejores
    void seleccionaNMejores(const vector<tSolution<double>>& poblacion, const vector<double>& fitness_pob, vector<int>& indices_mejores);
public:
    AM_BEST_MOD() : MH() {}
    virtual ~AM_BEST_MOD() {}
    // Implement the MH interface methods
    /**
     * Create random solutions looking moving to a better neighbour
     *
     * @param problem The problem to be optimized
     * @return A pair containing the best solution found and its fitness
     */
    virtual ResultMH<double> optimize(Problem<double> &problem, int maxevals);
};