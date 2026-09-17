#include "portfolio.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cmath>

// Implementación del constructor (necesario poner Problem<double>() porque mi clase hereda de Problem)
Portfolio::Portfolio(string ruta, double lowest, double highest, string fecha_ini, string fecha_final) : Problem<double>() {
    pos_inicio = 0;
    pos_fin = 0;
    fecha_inicio = fecha_ini;
    fecha_fin = fecha_final;
    T = 0;
    leerArchivo(ruta, retorno);
    lo = lowest;
    hi = highest;
    // Número de FILAS (cuántos vectores hay dentro del vector principal)
    num_fil = retorno.size();

    // Número de COLUMNAS (cuántos elementos hay dentro del primer vector)
    // Importante: Solo funciona si la matriz no está vacía
    num_col = 0;
    if (num_fil > 0) {
        num_col = retorno[0].size();
    }
    
    covarianzas.assign(num_col, vector<double>(num_col, 0.0));
    calcularMatrizCovarianzas();

    suma_log.assign(num_col, 0.0);
    calcularSumaLog();
}

void Portfolio::leerArchivo(string ruta, vector<vector<double>>& matriz_a_rellenar){
    ifstream archivo(ruta);
    string linea;
    // Si el no se puede abrir salimos
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir " << ruta << endl;
        return;
    }

    // 1. Saltamos los nombres de las empresas
    getline(archivo, linea);
    bool encontrada_fecha_inicio = false;
    bool encontrada_fecha_fin = false;
    // 2. Bucle de lectura de filas
    while (getline(archivo, linea) and !encontrada_fecha_fin) {
        vector<double> fila_numerica;
        stringstream ss(linea); //Metemos la fila en ss que tiene memoria y puntero
        string casilla;

        // 3. Guardamos la fecha que es la primera columna para usarla
        getline(ss, casilla, ',');
        if(!encontrada_fecha_inicio and casilla != fecha_inicio){
            pos_inicio++;
        }
        else{
            if(casilla == fecha_inicio){
                encontrada_fecha_inicio = true;
                pos_fin = pos_inicio;
            }
            if(casilla == fecha_fin){
                encontrada_fecha_fin = true;
            }else{
                pos_fin++;
            }
            // 4. Leemos cada casilla de la matriz
            while (getline(ss, casilla, ',')) {
                if (casilla.empty() || casilla == "\r") {   // Si está vacia significa que hay dos comas seguidas y que el valor es 0
                    fila_numerica.push_back(0.0);
                } else {
                    fila_numerica.push_back(stod(casilla));
                }
            }

            if (!fila_numerica.empty()) {                       // Para no meter un vector vacío
                matriz_a_rellenar.push_back(fila_numerica);

            }
        }
    }
    archivo.close();
    T = pos_fin - pos_inicio + 1;
}

// Devuelve el tamaño de la solucion
size_t Portfolio::getSolutionSize() {
    return num_col; 
}

std::pair<double, double> Portfolio::getSolutionDomainRange() {
    return {lo, hi}; 
}

// Calcula lo buena que es la solución (devolvemos 0.0 por ahora)
tFitness Portfolio::fitness(const tSolution<double> &solution) {
    return (beneficio(solution) - lambda*riesgo(solution)); 
}

double Portfolio::riesgo(const tSolution<double> &solution){
    size_t tam_sol = getSolutionSize();
    double riesgo_total = 0.0;
    // Implementación del doble sumatorio: Σ_i Σ_j (wi * Σij * wj)
    for (int i = 0; i < tam_sol; ++i) {
        // Optimización: si wi es 0, no hace falta entrar en el segundo bucle
        if (solution[i] > 0.0) { 
            for (int j = 0; j < tam_sol; ++j) {
                if (solution[j] > 0.0) {
                    riesgo_total += solution[i] * covarianzas[i][j] * solution[j];
                }
            }
        }
    }
    return sqrt(riesgo_total);
}

double Portfolio::beneficio(const tSolution<double> &solution){
    size_t tam_sol = getSolutionSize();
    //BENEFICIO
    double beneficio = 0.0;
    for(size_t i = 0; i < tam_sol; i++){
        double peso_i = solution[i];
        if(peso_i != 0.0){
            beneficio+=peso_i*suma_log[i];
        }
    }
    return beneficio;
}

tSolution<double> Portfolio::createSolution() {
    //Primero tomamos un numero de ceros entre 1 y N-1
    int num_ceros = Random::get<int>(1, num_col-1);

    tSolution<double> solution(num_col);
    for (int i = 0; i < num_col; i++) {
        if(i < (num_col - num_ceros)){             // Los n primeros seran aleatorios
            solution[i] = Random::get<double>(lo, hi);
        }
        else{                           // Rellenamos los num_ceros finales
            solution[i] = 0.0;
        }
    }
    fix(solution);
    Random::shuffle(solution.begin(), solution.end());
    return solution;
}

// Comprueba si la solución se pasa del presupuesto (por defecto, decimos que sí es válida)
bool Portfolio::isValid(const tSolution<double> &solution) {
    bool suma_es_1 = false;
    bool entre_rangos_o_cero = true;
    size_t i = 0;
    double suma = 0.0;
    size_t tam_sol = getSolutionSize();
    while(entre_rangos_o_cero and i < tam_sol){
        double valor = solution[i];
        entre_rangos_o_cero = (valor == 0.0) || ((valor >= lo) && (valor <= hi));
        suma+=valor;
        i++;
    }
    if(!entre_rangos_o_cero){ return false;}
    // Si están entre los rangos o son 0 comprobamos la suma
    suma_es_1 = igualConTolerancia(suma, 1.0);
    return suma_es_1 && entre_rangos_o_cero; 
}


void Portfolio::fix(tSolution<double> &solution) {
    double suma_pesos = calculaSuma(solution);
    normalizar(solution, suma_pesos);
    size_t tam_sol = getSolutionSize();

    // 1. EL BARRIDO: in-place, sin mover elementos
    vector<int> posiciones_validas;   // indices con valor entre lo y hi
    vector<int> posiciones_ceros;     // indices con valor 0

    for(size_t i = 0; i < tam_sol; i++){
        if(solution[i] != 0.0 && solution[i] >= lo){
            solution[i] = std::min(solution[i], hi);  // limitamos a hi en su sitio
            posiciones_validas.push_back(i);
        } else {
            solution[i] = 0.0;  // ponemos a 0 en su sitio
            posiciones_ceros.push_back(i);
        }
    }
    Random::shuffle(posiciones_validas);
    Random::shuffle(posiciones_ceros);
    int num_no_ceros = posiciones_validas.size();
    int num_ceros    = posiciones_ceros.size();
    double suma_acumulada = 1.0 - calculaSuma(solution);

    int indice   = 0;  // índice dentro de posiciones_validas
    int indice_2 = Random::get<int>(0, tam_sol - 1);

    while(suma_acumulada > tolerancia){
        if(num_ceros > 0){  // quedan posiciones vacías donde meter valores nuevos
            if(suma_acumulada >= lo){
                double num_random = Random::get<double>(lo, min(hi, suma_acumulada));
                int pos = posiciones_ceros.back();
                posiciones_ceros.pop_back();
                solution[pos] = num_random;
                posiciones_validas.push_back(pos);
                suma_acumulada -= num_random;
                num_no_ceros++;
                num_ceros--;
            } else {
                // suma_acumulada < lo: robamos de un válido para poder crear uno nuevo
                if(num_no_ceros == 0) break;
                int pos_valido = posiciones_validas[indice % num_no_ceros];
                double num_random = Random::get<double>(0.0, solution[pos_valido] - lo);
                solution[pos_valido] -= num_random;
                if(suma_acumulada + num_random >= lo){
                    int pos = posiciones_ceros.back();
                    posiciones_ceros.pop_back();
                    solution[pos] = suma_acumulada + num_random;
                    posiciones_validas.push_back(pos);
                    suma_acumulada = 0.0;
                    num_no_ceros++;
                    num_ceros--;
                } else {
                    indice++;
                    suma_acumulada += num_random;
                }
            }
        } else {  // todos los pesos son distintos de 0, repartimos entre ellos
            if(solution[indice_2] + suma_acumulada <= hi){
                solution[indice_2] += suma_acumulada;
                suma_acumulada = 0.0;
            } else {
                double num_random = Random::get<double>(0.0, hi - solution[indice_2]);
                solution[indice_2] += num_random;
                suma_acumulada -= num_random;
                indice_2 = (indice_2 + 1) % tam_sol;
            }
        }
    }
}


double Portfolio::calculaSuma(const tSolution<double> &solution){
    double suma = 0.0;
    size_t tam_sol = getSolutionSize();
    for(size_t i=0; i < tam_sol; i++){
        suma+=solution[i];
    }
    return suma;
}

void Portfolio::normalizar(tSolution<double> &solution, double suma){
    size_t tam_sol = getSolutionSize();
    for(size_t i=0; i < tam_sol; i++){
        solution[i]=solution[i]/suma;
    }
}

bool Portfolio::igualConTolerancia(double num1, double num_2){
    return (abs(num1 - num_2) <= tolerancia);
}


void Portfolio::calcularMatrizCovarianzas() {
    int tam_sol = getSolutionSize();
    // 1. Calcular las medias de cada activo (µ_i)
    vector<double> medias(tam_sol, 0.0);
    for (int j = 0; j < tam_sol; ++j) {
        for (int i = 0; i < T; ++i) {
            medias[j] += retorno[i][j];
        }
        medias[j] /= (T*1.0);
    }

    // 2. Rellenar la matriz de covarianza
    for (int i = 0; i < tam_sol; ++i) {
        for (int j = i; j < tam_sol; ++j) { // Empezamos en j = i porque es simétrica
            double suma = 0.0;
            for (int t = 0; t < T; ++t) {
                suma += (retorno[t][i] - medias[i]) * (retorno[t][j] - medias[j]);
            }
            double resultado = suma / (T*1.0);
            covarianzas[i][j] = resultado;
            covarianzas[j][i] = resultado; // Aprovechamos la simetría
        }
    }
}

size_t Portfolio::getNumCol(){
    return num_col;
}
size_t Portfolio::getNumFil(){
        return num_fil;
}

double Portfolio::getCovarianza(int i, int j){
    return covarianzas[i][j];
}

int Portfolio::getPeriodo(){
    return T;
}

double Portfolio::getLambda(){
    return lambda;
}

double Portfolio::getHi(){
    return hi;
}

double Portfolio::getSumaLog(int i){
    return suma_log[i];
}

void Portfolio::calcularSumaLog(){
    int tam_sol = getSolutionSize();
    for(size_t i = 0; i < tam_sol; i++){
        double suma_logaritmos = 0.0;
        for(int j = 0; j < T; j++){
            suma_logaritmos+=log(1 + retorno[j][i]);
        }
        suma_log[i] = suma_logaritmos;
    }
}