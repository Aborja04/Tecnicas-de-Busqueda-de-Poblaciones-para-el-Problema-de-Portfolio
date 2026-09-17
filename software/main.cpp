#include <iostream>
#include <fstream>
#include <problem.h>
#include <random.hpp>
#include <string>
#include <iomanip> 
#include <util.h>

#include "portfolio.h"

#include "randomsearch.h"
#include "busqueda_local.h"
#include "greedy.h"
#include "agg_arit.h"
#include "agg_blx_alpha.h"
#include "age_arit.h"
#include "age_blx_alpha.h"
#include "am_all.h"
#include "am_best.h"
#include "am_rand.h"
#include "bmb.h"
#include "ils.h"
#include "es.h"
#include "ils_es.h"



#include "am_best_mod.h"
#include "age_blx_alpha_mod.h"
#include "ils_mod.h"
#include "es_mod.h"
#include "es_mod2.h"


#include "fa.h"

using namespace std;
using namespace std::chrono;

// --- CONSTANTES GLOBALES ---
const double LO_IBEX = 0.005;
const double HI_IBEX = 0.08;
const double LO_SYP100 = 0.005;
const double HI_SYP100 = 0.05;
const double LO_SYP500 = 0.005;
const double HI_SYP500 = 0.02;
struct ResultadoAlg {
    string nombre;
    double media_fitness_hist = 0.0;
    double media_fitness_test = 0.0;
    double media_ben_test     = 0.0;
    double media_eval         = 0.0;
    double media_tiempo       = 0.0;
};

ResultadoAlg ejecutarAlgoritmo(const string& nombre, MHDouble* alg,
                                Portfolio& historico, Portfolio& test,
                                int evaluaciones, int repeticiones, int semilla,
                                ofstream& out1, ofstream& out2) {
    ResultadoAlg res;
    res.nombre = nombre;

    ProblemDouble* problem_hist = dynamic_cast<ProblemDouble*>(&historico);
    ProblemDouble* problem_test = dynamic_cast<ProblemDouble*>(&test);

    for (int rep = 0; rep < repeticiones; rep++) {
        Random::seed(semilla + rep);
        if(rep % 10 == 0){
            cout << "  -> [" << nombre << "] Repeticion " << rep + 1 
             << " (Semilla: " << semilla + rep << ")" << endl;
        }

        auto t1 = high_resolution_clock::now();
        ResultMHDouble resultado = alg->optimize(*problem_hist, evaluaciones);
        auto t2 = high_resolution_clock::now();
        duration<double> tiempo = t2 - t1;

        double fitness_test = problem_test->fitness(resultado.solution);

        res.media_fitness_hist += resultado.fitness;
        res.media_fitness_test += fitness_test;
        res.media_ben_test     += test.beneficio(resultado.solution);
        res.media_eval         += resultado.evaluations;
        res.media_tiempo       += tiempo.count();

        out1 << nombre << "," << resultado.fitness << "\n";
        out2 << nombre << "," << fitness_test       << "\n";
    }

    res.media_fitness_hist /= repeticiones;
    res.media_fitness_test /= repeticiones;
    res.media_ben_test     /= repeticiones;
    res.media_eval         /= repeticiones;
    res.media_tiempo       /= repeticiones;

    return res;
}

void imprimirTabla(const vector<ResultadoAlg>& resultados) {
    const int w_algo = 12, w_fit1 = 14, w_fit2 = 12;
    const int w_ben  = 12, w_eval = 16, w_time = 14;
    int ancho_total  = w_algo + w_fit1 + w_fit2 + w_ben + w_eval + w_time;

    cout << fixed << setprecision(4);
    cout << setfill('-') << setw(ancho_total) << "-" << setfill(' ') << endl;
    cout << left  << setw(w_algo) << "Algoritmo"
         << right << setw(w_fit1) << "2015-2024"
         << right << setw(w_fit2 + w_ben/2 + 2) << "2025" << setw(w_ben/2 - 2) << ""
         << right << setw(w_eval) << "Evaluaciones"
         << right << setw(w_time) << "Tiempo" << endl;
    cout << left  << setw(w_algo) << ""
         << right << setw(w_fit1) << "Fitness"
         << right << setw(w_fit2) << "Fitness"
         << right << setw(w_ben)  << "Beneficio"
         << right << setw(w_eval) << ""
         << right << setw(w_time) << "(segundos)" << endl;
    cout << setfill('-') << setw(ancho_total) << "-" << setfill(' ') << endl;

    for (const auto& r : resultados) {
        cout << left  << setw(w_algo) << ("  " + r.nombre)
             << right << setw(w_fit1) << r.media_fitness_hist
             << right << setw(w_fit2) << r.media_fitness_test
             << right << setw(w_ben)  << r.media_ben_test
             << right << setw(w_eval) << r.media_eval
             << right << setw(w_time) << r.media_tiempo << endl;
    }
    cout << setfill('-') << setw(ancho_total) << "-" << setfill(' ') << endl;
}

int main(int argc, char *argv[]) {
 
    if (argc != 33) {
        cerr << "Error: El programa esperaba 33 argumentos, pero recibio " << argc - 1 << endl;
        return 1;
    }

    // 2. Las repeticiones siempre son el penúltimo elemento (ahora el 25)
    int repeticiones = stoi(argv[25]);
    int semilla = stoi(argv[26]);


    // 3. Bucle Externo: Recorremos las 3 bolsas
    for (int b = 0; b < 3; b++) {
        
        // El salto ahora es de 8 en 8
        // Bolsa 0 empieza en argv[1], Bolsa 1 en argv[9], Bolsa 2 en argv[17]
        int inicio = 1 + (b * 8);

        // Extraemos los 8 datos
        string ruta_archivo  = argv[inicio];
        int evaluaciones     = stoi(argv[inicio + 1]);
        string hist_inicio   = argv[inicio + 2];
        string hist_final    = argv[inicio + 3];
        string test_inicio   = argv[inicio + 4];
        string test_final    = argv[inicio + 5];
        double lo            = stod(argv[inicio + 6]); // stod para leer doubles
        double hi            = stod(argv[inicio + 7]); // 
        string archivo_csv   = argv[27+b];
        string archivo_csv_test   = argv[27+b+3];

        Portfolio historico = Portfolio(ruta_archivo, lo, hi, hist_inicio, hist_final);
        Portfolio test = Portfolio(ruta_archivo, lo, hi, test_inicio, test_final);
        


        ofstream out1(archivo_csv);
        std::ofstream out2(archivo_csv_test);

        if (!out1.is_open() || !out2.is_open()) {
            std::cerr << "Error al abrir los archivos\n";
            return 1;
        }

        out1 << "alg,fitness\n";
        out2 << "alg,fitness\n";

        RandomSearch<double> rs = RandomSearch<double>();
        BusquedaLocal bl;

       // Algoritmos geneticos
        AGG_Arit      agg_arit;
        AGG_BLX_ALPHA agg_blx;
        AGE_Arit      age_arit;
        AGE_BLX_ALPHA age_blx;

        // Algoritmos memeticos
        AM_ALL am_all;
        AM_BEST am_best;
        AM_RAND am_rand;

        // Algoritmos Extra
        AM_BEST_MOD am_best_mod;
        AGE_BLX_ALPHA_MOD age_blx_mod;
        ILS_MOD ils_mod;
        ES_MOD es_mod;
        ES_MOD2 es_mod2;

        // Algoritmos P3
        BMB bmb;
        ILS ils;
        ES es;
        ILS_ES ils_es;
         
        // P4
        FA fa;

        vector<pair<string, MHDouble*>> algoritmos = {
            // P1
            /*{"AM-BEST-MOD", &am_best_mod}
            {"AGE-BLX-MOD",  &age_blx_mod},*/
            {"RS", &rs},
            {"BL", &bl},
            
            // P2
            /*{"AGG-ARIT", &agg_arit},
            {"AGG-BLX",  &agg_blx},
            {"AGE-ARIT", &age_arit},
            {"AGE-BLX",  &age_blx},
            {"AM-ALL", &am_all},
            {"AM-RAND", &am_rand},*/
            {"AM-BEST", &am_best},
            
            // P3
            {"BMB",  &bmb},
            {"ILS",  &ils},
            {"ES",  &es},
            {"ILS-ES",  &ils_es},

            // P4 / EXTRA
            //{"FA",  &fa}
            //{"ILS-MOD",  &ils_mod},
            //{"ES-MOD",  &es_mod},
            //{"ES-MOD2",  &es_mod2},
        };
        Greedy gr;
        auto t_gr1 = high_resolution_clock::now();
        ResultMHDouble resultado_gr = gr.optimize(historico, 1);
        auto t_gr2 = high_resolution_clock::now();

        ResultadoAlg res_gr;
        res_gr.nombre           = "Greedy";
        res_gr.media_fitness_hist = resultado_gr.fitness;
        res_gr.media_fitness_test = test.fitness(resultado_gr.solution);
        res_gr.media_ben_test     = test.beneficio(resultado_gr.solution);
        res_gr.media_eval         = resultado_gr.evaluations;
        res_gr.media_tiempo       = duration<double>(t_gr2 - t_gr1).count();

        out1 << "GREEDY," << resultado_gr.fitness          << "\n";
        out2 << "GREEDY," << res_gr.media_fitness_test      << "\n";
        vector<ResultadoAlg> resultados = {res_gr};  // Greedy primero
        
        for (auto& [nombre, alg] : algoritmos) {
            resultados.push_back(
                ejecutarAlgoritmo(nombre, alg, historico, test,
                                evaluaciones, repeticiones, semilla, out1, out2)
            );
        }

        imprimirTabla(resultados);
    }
    return 0;

}
