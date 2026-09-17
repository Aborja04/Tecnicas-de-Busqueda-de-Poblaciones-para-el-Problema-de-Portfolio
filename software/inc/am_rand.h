#pragma once
#include <mh.h>

using namespace std;

// Instanciamos la plantilla con el tipo que nos interese
using MHDouble = MH<double>;
using ProblemDouble = Problem<double>;
using ResultMHDouble = ResultMH<double>;

class AM_RAND : public MHDouble {
private:
    vector<pair<int, int>> posiciones;      // Posiciones para BL
    vector<int> posiciones_aleatorias;      // Posiciones para seleccionar los N aleatorios
    static constexpr int TAM_POBLACION = 100;
    static constexpr int MAX_EVALS_BL = 100;
    static constexpr double P_LS = 0.1;
    static constexpr int N_ALEATORIOS = P_LS*TAM_POBLACION;
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
    void actualizaFitness(Problem<double>& problem, const vector<double>& fitness_padres, const vector<tSolution<double>>& hijos, vector<double>& fitness_hijos, int &indice_mejor_fitness, int &indice_peor_fitness, int &indice_mejor_padre, vector<bool>& indices_mutados);
    void inicializarPosiciones(int tam_sol);
    void inicializarPosicionesAleatorio(int tam_sol);

    int seleccionaMejor(const vector<double>& fitness_pob);
    // Devuelve los indices de N aleatorios
    void seleccionaNAleatorios(const vector<tSolution<double>>& poblacion, const vector<double>& fitness_pob, vector<int>& indices_aleatorios);
public:
    AM_RAND() : MH() {}
    virtual ~AM_RAND() {}
    // Implement the MH interface methods
    /**
     * Create random solutions looking moving to a better neighbour
     *
     * @param problem The problem to be optimized
     * @return A pair containing the best solution found and its fitness
     */
    virtual ResultMH<double> optimize(Problem<double> &problem, int maxevals);
};