# Estructura del proyecto

A continuación se muestra la estructura general del proyecto junto con una breve descripción de cada carpeta y archivo:

|-- build
|-- CMakeLists.txt
|-- common
|-- configuraciones
|-- datos_portfolio_2526
|-- ejecutar_todo.sh
|-- inc
|-- LICENSE
|-- main.cpp
|-- README.md
|-- resultados
|-- src


## Descripción de carpetas y archivos

- **build**: Contiene los archivos generados automáticamente durante la compilación del proyecto (CMake y Make), incluyendo ejecutables y ficheros intermedios.

- **CMakeLists.txt**: Archivo de configuración de CMake utilizado para generar el sistema de compilación.

- **common**: Incluye clases y utilidades generales proporcionadas (como estructuras base de problema, solución, etc.) que son utilizadas por los distintos algoritmos.

- **configuraciones**: Contiene los archivos de configuración de cada instancia del problema (IBEX 35, S&P 100, S&P 500), donde se definen parámetros como rutas, fechas y límites.

- **datos_portfolio_2526**: Contiene los archivos CSV con los datos históricos de las distintas bolsas utilizados en los experimentos.

- **ejecutar_todo.sh**: Script que automatiza la ejecución del programa con distintas configuraciones y parámetros.

- **inc**: Contiene los archivos de cabecera (`.h`) donde se definen las clases principales del proyecto.

- **src**: Contiene los archivos fuente (`.cpp`) donde se implementan las clases y algoritmos.

- **resultados**:Contiene los resultados en archivos csv y el script de python para hacer las gráficas

- **main.cpp**: Archivo principal del programa, encargado de lanzar la ejecución de los algoritmos.

- **LICENSE**: Archivo que contiene la licencia del proyecto.

- **README.md**: Documento descriptivo del proyecto.
