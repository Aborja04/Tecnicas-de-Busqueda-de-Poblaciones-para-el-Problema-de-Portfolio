#pragma once
#include "problem.h"
#include <vector>
#include <string>

using namespace std;

class Portfolio : public Problem<double> {
private:
    static constexpr double tolerancia = 1e-8;  // para las comparaciones de la suma de pesos con 1
    static constexpr double lambda = 500.0;
    vector<vector<double>> retorno;
    vector<vector<double>> covarianzas;
    vector<double> suma_log; 
    // Rango de valores permitido
    double lo;
    double hi;
    size_t num_fil;
    size_t num_col;
    string fecha_inicio, fecha_fin;
    int T;
    int pos_inicio;
    int pos_fin;
public:
    /**
     * @brief Constructor de la clase Portfolio
     * @param ruta     Archivo CSV con los porcentajes de retorno por dia y empresa
     * @param lowest   limite inferior
     * @param highest   limite superior
     * @param fecha_ini     fecha inicial archivo
     * @param fecha_final   fecha final archivo
     */
    Portfolio(string ruta, double lowest, double highest, string fecha_ini, string fecha_final); 
    virtual ~Portfolio() {} // Destructor virtual vacío en el .h
    
    /**
     * @brief Calculates the size of the solution
     * @return Size of solution
     */
    virtual size_t getSolutionSize() override;

    /**
     * @brief Returns domain bounds (min, max) for solution components
     * @return {min, max}
     */
    virtual std::pair<double, double> getSolutionDomainRange() override;


    tFitness fitness(const tSolution<double> &solution) override;
    double beneficio(const tSolution<double> &solution);
    double riesgo(const tSolution<double> &solution);

    tSolution<double> createSolution() override;

    /**
     * @brief Checks if a solution meets all problem constraints
     * @param solution 
     * @return True if all is correct, False otherwise
     */
    virtual bool isValid(const tSolution<double> &solution) override;

    /**
     * @brief Modifies an invalid solution to meet constraints
     * @param solution 
     */
    virtual void fix(tSolution<double> &solution) override;

    /**
     * @brief Reads the csv file with the return values of the market
     * @param ruta Path of the file
     * @param matriz_a_rellenar Matrix that contains (i, j) where i (rows) = day and j (columns) = company
     *                          There is no name of the company and no date in the matrix
     */
    void leerArchivo(string ruta, vector<vector<double>>& matriz_a_rellenar);
    double calculaSuma(const tSolution<double> &solution);
    void normalizar(tSolution<double> &solution, double suma);
    bool igualConTolerancia(double num1, double num_2);
    void calcularMatrizCovarianzas();
    void calcularSumaLog();
    size_t getNumCol();
    size_t getNumFil();
    int getPeriodo();
    double getLambda();
    double getHi();
    double getCovarianza(int i, int j);
    double getSumaLog(int i);

};