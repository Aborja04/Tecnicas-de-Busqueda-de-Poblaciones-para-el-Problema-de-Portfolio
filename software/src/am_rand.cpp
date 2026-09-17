#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility> // Para std::pair
#include <numeric>

#include "am_rand.h"
#include "portfolio.h"
#include "busqueda_local_auxiliar.h"
#include "actualizador.h"

using namespace std;

ResultMHDouble AM_RAND::optimize(ProblemDouble &problem, int maxevals) {
    this->evaluaciones = 0;
    this->mejor_fitness_global = -numeric_limits<double>::infinity(); 
    this->mejor_global.clear();

    vector<tSolution<double>> poblacion(TAM_POBLACION);
    vector<double> fitness_poblacion(TAM_POBLACION);

    // Inicializacion poblacion
    for(int i = 0; i < TAM_POBLACION; i++){
        poblacion[i] = problem.createSolution();
        fitness_poblacion[i] = problem.fitness(poblacion[i]);
        evaluaciones++;
        if(fitness_poblacion[i] > mejor_fitness_global){
            mejor_global = poblacion[i];
            mejor_fitness_global = fitness_poblacion[i];
        }
    }
    
    int tam_sol = problem.getSolutionSize();
    auto rango  = problem.getSolutionDomainRange();
    double lo = rango.first;
    double hi = rango.second;
    vector<tSolution<double>> hijos(TAM_POBLACION);
    vector<double> fitness_hijos(TAM_POBLACION);
    vector<tSolution<double>> padres(TAM_POBLACION);
    vector<double> fitness_padres(TAM_POBLACION);
    vector<bool> indices_mutados(TAM_POBLACION);        // true si un indice se ha mutado
    int contador_generaciones = 0;
    // Inicializamos el vector de posiciones para solo tener que hacerlo una vez
    inicializarPosiciones(tam_sol);
    int tam_pos = tam_sol*tam_sol;

    // Instancia de clase Busqueda Local
    BusquedaLocalAuxiliar bl(lo, hi, RATIO, MAX_EVALS_BL, maxevals);

    Actualizador actualizador(NUM_CRUCES_ESPERADO, TAM_POBLACION);

    vector<int> indices_aleatorios(N_ALEATORIOS);
    inicializarPosicionesAleatorio(TAM_POBLACION);
    while(evaluaciones < maxevals){

        if(contador_generaciones < 10){
            contador_generaciones++;
            // Seleccion padres por torneo
            seleccionTorneo(poblacion, fitness_poblacion, padres, fitness_padres);
            cruceBLX(problem, padres, hijos, tam_sol);
            mutar(hijos, tam_sol, lo , hi, indices_mutados);
            int indice_mejor = 0;
            int indice_peor = 0;
            actualizador.ActualizarFitness(problem, fitness_padres, hijos, fitness_hijos, indice_mejor, indice_peor, indices_mutados, evaluaciones);
            // Mantenemos el mejor de la poblacion anterior si no hemos mejorado
            if(fitness_hijos[indice_mejor] < mejor_fitness_global){
                hijos[indice_peor] = mejor_global;
                fitness_hijos[indice_peor] = mejor_fitness_global;
                indice_mejor = indice_peor;     // El mejor es ahora el que era el peor
            }
            else{       // Tenemos un nuevo mejor global
                mejor_global = hijos[indice_mejor];
                mejor_fitness_global = fitness_hijos[indice_mejor];
            }
            // swap por eficiencia
            swap(poblacion, hijos);
            swap(fitness_poblacion, fitness_hijos);  
        }
        else{
            contador_generaciones = 0;
            seleccionaNAleatorios(poblacion, fitness_poblacion, indices_aleatorios);
            for(int t= 0; t < N_ALEATORIOS && evaluaciones < maxevals; t++){
                bl.aplicarBL(problem, poblacion[indices_aleatorios[t]], fitness_poblacion[indices_aleatorios[t]], posiciones, tam_pos, evaluaciones);
            }

            int indice_mejor = seleccionaMejor(fitness_poblacion);
            
            // Actualizar el mejor global
            if(fitness_poblacion[indice_mejor] > mejor_fitness_global){
                mejor_global = poblacion[indice_mejor];
                mejor_fitness_global = fitness_poblacion[indice_mejor];
            }

        }
    }
    return ResultMH(mejor_global, mejor_fitness_global, evaluaciones);
}


void AM_RAND::cruceBLX(Problem<double>& problem, const vector<tSolution<double>>& padres, vector<tSolution<double>>& hijos, int tam_sol){

    for(int i = 0; i < TAM_POBLACION; i+=2){
        tSolution<double> hijo1 = padres[i];
        tSolution<double> hijo2 = padres[i+1];
        //BLX-ALPHA
        bool todos_cero_hijo_1 = true;
        bool todos_cero_hijo_2 = true;
        if(i < 2*NUM_CRUCES_ESPERADO){
            for (size_t j = 0; j < tam_sol; j++) {
                double cmin = min(padres[i][j], padres[i+1][j]);
                double cmax = max(padres[i][j], padres[i+1][j]);
                double I    = cmax - cmin;

                double lo_blx = cmin - I * ALPHA;
                double hi_blx = cmax + I * ALPHA;

                hijo1[j] = max(Random::get<double>(lo_blx, hi_blx), 0.0);
                // Hay que comprobar que todos los hijos no sean 0.0 (luego al hacer fix dividiria por 0.0)
                if(hijo1[j] != 0.0){todos_cero_hijo_1 = false;}
                hijo2[j] = max(Random::get<double>(lo_blx, hi_blx), 0.0);
                if(hijo2[j] != 0.0){todos_cero_hijo_2 = false;}
            }
            // Nos quedamos con los padres si todos los valores de los hijos eran 0 (es decir el aleatorio era negativo siempre)
            if(todos_cero_hijo_1){
                hijo1 = padres[i];
            }
            if(todos_cero_hijo_2){
                hijo2 = padres[i+1];
            }
            // Reparamos los hijos para que cumplan las restricciones
            problem.fix(hijo1);
            problem.fix(hijo2);
        }
        hijos[i] = hijo1;
        hijos[i+1] = hijo2;
    }
}

void AM_RAND::mutar(vector<tSolution<double>>& hijos, int tam_sol, double lo, double hi, vector<bool>& indices_mutados){
    fill(indices_mutados.begin(), indices_mutados.end(), false);    // Reiniciamos el vector de indices mutados
    for(int i = 0; i < NUM_MUTACIONES_ESPERADO; i++){
        int indice_hijo = Random::get<int>(0, TAM_POBLACION - 1);
        int indice_empresa_1  = Random::get<int>(0, tam_sol - 1);
        int indice_empresa_2  = Random::get<int>(0, tam_sol - 1);
        if(indice_empresa_1 != indice_empresa_2){
            double valor_anterior_i = hijos[indice_hijo][indice_empresa_1];
            double valor_anterior_j = hijos[indice_hijo][indice_empresa_2];
            if(valor_anterior_i > lo && valor_anterior_j < hi){
                double tope_1 = 1.0 - (lo / valor_anterior_i);
                double tope_2 = (hi - valor_anterior_j) / valor_anterior_i;
                double tope_3 = RATIO;
                // Nos quedamos con el menor tope y ese es lo maximo que podemos quitarle a i para darselo a j
                double ratio = min({tope_1, tope_2, tope_3});
                if(ratio == 0){
                    continue;
                }
                hijos[indice_hijo][indice_empresa_1] = (1-ratio)*valor_anterior_i;
                hijos[indice_hijo][indice_empresa_2] = valor_anterior_j +  ratio*valor_anterior_i;
                indices_mutados[indice_hijo] = true;            // Se muta ese indice
            }
        }
    }
}

void AM_RAND::seleccionTorneo(const vector<tSolution<double>>& poblacion, const vector<double>& fitness_pob, vector<tSolution<double>>& padres, vector<double>& fitness_padres){
    for (int i = 0; i < TAM_POBLACION; i++) {
        int indice_ganador = Random::get<int>(0, TAM_POBLACION-1);

        for (int j = 1; j < K_TORNEO; j++) {
            int indice_rival = Random::get<int>(0, TAM_POBLACION-1);
            if (fitness_pob[indice_rival] > fitness_pob[indice_ganador]) {
                indice_ganador = indice_rival;
            }
        }
        padres[i] = poblacion[indice_ganador];
        fitness_padres[i] = fitness_pob[indice_ganador];
    }
}

void AM_RAND::inicializarPosiciones(int tam_sol){
    posiciones.clear();
    posiciones.reserve(tam_sol * tam_sol);
    for (int i = 0; i < tam_sol; ++i) {
        for (int j = 0; j < tam_sol; ++j) {
            posiciones.push_back({i, j});
        }
    }
}

void AM_RAND::inicializarPosicionesAleatorio(int tam){
    posiciones_aleatorias.clear();
    posiciones_aleatorias.reserve(tam);
    for (int i = 0; i < tam; ++i) {
        posiciones_aleatorias.push_back(i);
    }
}

int AM_RAND::seleccionaMejor(const vector<double>& fitness_pob){
    double mejor =  fitness_pob[0];
    int pos_mejor = 0;
    for(int i = 1; i < TAM_POBLACION; i++){
        if(fitness_pob[i] > mejor){
            pos_mejor = i;
            mejor = fitness_pob[i];
        }
    }
    return pos_mejor;
}

void AM_RAND::seleccionaNAleatorios(const vector<tSolution<double>>& poblacion, const vector<double>& fitness_pob, vector<int>& indices_aleatorios){
    // Barajamos el vector de posiciones aleatorias (va de 0 hasta TAM_POB-1)
    Random::shuffle(posiciones_aleatorias);
    indices_aleatorios.resize(N_ALEATORIOS);
    for(int i = 0; i < N_ALEATORIOS; i++){
        indices_aleatorios[i] = posiciones_aleatorias[i];  // guardamos la posicion original
    }
}

