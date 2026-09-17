#pragma once
#include <mh.h>

using namespace std;


class BusquedaLocalAuxiliar {
private:
    double lo;
    double hi;
    double ratio_max;
    int max_evals_BL;
    int max_evals_global;
public:    
    BusquedaLocalAuxiliar(double lo_1, double hi_1, double ratio_1, int max_evals_bl, int max_evals_globales){
        lo = lo_1;
        hi = hi_1;
        ratio_max = ratio_1;
        max_evals_BL = max_evals_bl;
        max_evals_global = max_evals_globales;
    }

    void aplicarBL(Problem<double>& problem, tSolution<double>& sol, double& fit, vector<pair<int,int>>& posiciones, int tam_pos, int& evaluaciones);
};

