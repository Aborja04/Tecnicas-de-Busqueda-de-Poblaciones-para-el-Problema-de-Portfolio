# Practicas_MH

## Introducción
> *El problema del portfolio consiste en encontrar la asignación óptima de capital entre un conjunto de N activos
financieros disponibles, de forma que se maximice el beneficio y se minimice el riesgo.
Cada solución del problema se representa mediante un vector de pesos w = (w1 , w2 , . . . , wN ), donde wi indica
la proporción de capital invertida en el activo i. Deberán de cumplir ciertas restricciones que ahora veremos.
El beneficio asociado a una solución se calcula a partir de los retornos históricos de los activos, mientras que el
riesgo se modela mediante la matriz de covarianzas, que recoge la dependencia entre los distintos activos. De esta
forma, el problema plantea un compromiso entre maximizar la rentabilidad esperada y minimizar la variabilidad
de dicha rentabilidad.
Este problema lo vamos a abordar mediante el uso de técnicas metaheurísticas como los algoritmos greedy o
la búsqueda local.."*

---

## Guía de Uso

### Compilación
Nos situamos en la carpeta `software` y hacemos lo siguiente para limpiar el entorno y compilar:

```bash
cd build
rm -rf *
cmake ..
make
cd ..
```

### Ejecución
Ahora, después de hacer la compilación anterior, nos situamos en la raíz de la carpeta `software` y ejecutamos el script indicando la semilla:

```bash
./ejecutar_todo.sh <semilla_inicial>
```


### Gráficas
Para visualizar los datos, nos situamos de nuevo en la carpeta software y ejecutamos el script de Python:

```bash
cd resultados
python3 graficas.py
```
