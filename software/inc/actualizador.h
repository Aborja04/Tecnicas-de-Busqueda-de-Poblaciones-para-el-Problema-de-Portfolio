#pragma once
#include <mh.h>

using namespace std;


class Actualizador {
private:
    int num_cruces_esperado;
    int tam_poblacion;
public:    
    Actualizador(int cruces, int tam){
        num_cruces_esperado = cruces;
        tam_poblacion = tam;
    }

    void ActualizarFitness(Problem<double>& problem, const vector<double>& fitness_padres, const vector<tSolution<double>>& hijos, vector<double>& fitness_hijos, int &indice_mejor_fitness, int &indice_peor_fitness, vector<bool>& indices_mutados, int &evaluaciones);
};

