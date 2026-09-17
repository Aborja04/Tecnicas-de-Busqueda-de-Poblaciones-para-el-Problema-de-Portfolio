#pragma once
#include <mh.h>

using namespace std;

// Instanciamos la plantilla con el tipo que nos interese
using MHDouble = MH<double>;
using ProblemDouble = Problem<double>;
using ResultMHDouble = ResultMH<double>;

class ILS_ES : public MHDouble {
private:
    vector<pair<int, int>> posiciones;      // Posiciones para ES
    vector<int> indices_empresas;      // Posiciones para las N empresas

    static constexpr int N_VUELTAS = 5;
    static constexpr int MAX_EVALS_ES = 1999;    
    static constexpr double RATIO = 0.40;  

    static constexpr double MU = 0.2;
    static constexpr double PHI = 0.3; 
    static constexpr double T_FINAL = 1e-3;
    static constexpr int CONSTANTE_VECINOS = 10;
    static constexpr int CONSTANTE_EXITOS = 1;
    static constexpr double PORCENTAJE_MUTACION = 0.2;        


    int evaluaciones = 0;
    tSolution<double> mejor_global;
    double mejor_fitness_global = -9999999.0;
    void mutar(tSolution<double>& solucion, int numero_empresas_barajar);
    void inicializarIndicesEmpresas(int tam);
    void inicializarPosiciones(int tam_sol);
public:
    ILS_ES() : MH() {}
    virtual ~ILS_ES() {}
    // Implement the MH interface methods
    /**
     * Create random solutions looking moving to a better neighbour
     *
     * @param problem The problem to be optimized
     * @return A pair containing the best solution found and its fitness
     */
    virtual ResultMH<double> optimize(Problem<double> &problem, int maxevals);
};