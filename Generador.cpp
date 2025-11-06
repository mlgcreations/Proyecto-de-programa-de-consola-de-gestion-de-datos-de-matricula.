#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// Listas de nombres y apellidos comunes en Perú
vector<string> nombres = {
    "Juan", "María", "Carlos", "Ana", "Luis", "Rosa", "Pedro", "Carmen", 
    "José", "Isabel", "Miguel", "Teresa", "Diego", "Lucía", "Jorge", 
    "Patricia", "Ricardo", "Elena", "Fernando", "Sofía", "Alejandro", 
    "Daniela", "Roberto", "Valeria", "Francisco", "Gabriela", "Antonio",
    "Camila", "Manuel", "Andrea", "Ángel", "Victoria", "Pablo", "Natalia",
    "Raúl", "Paula", "Sergio", "Adriana", "Javier", "Melissa"
};

vector<string> apellidos = {
    "García", "Rodríguez", "Fernández", "López", "Martínez", "Sánchez",
    "Pérez", "Gómez", "Ramírez", "Torres", "Flores", "Vásquez", "Castro",
    "Chávez", "Mendoza", "Quispe", "Huamán", "Ccoyllo", "Mamani", "Yupanqui",
    "Condori", "Apaza", "Puma", "Rojas", "Villanueva", "Gutiérrez", "Díaz",
    "Cruz", "Morales", "Reyes", "Jiménez", "Herrera", "Silva", "Medina",
    "Vargas", "Ortiz", "Romero", "Alvarez", "Castillo", "Ríos"
};

// Función para generar un nombre completo aleatorio
string generarNombreCompleto() {
    string nombre = nombres[rand() % nombres.size()];
    string apellido1 = apellidos[rand() % apellidos.size()];
    string apellido2 = apellidos[rand() % apellidos.size()];
    return nombre + " " + apellido1 + " " + apellido2;
}

// Función para generar edad según el grado
int generarEdad(int grado) {
    // Primaria: grados 1-6, edades típicas 6-11
    return 5 + grado + (rand() % 2); // Puede tener +0 o +1 año de variación
}

// Función para generar sección aleatoria
char generarSeccion() {
    char secciones[] = {'A', 'B', 'C', 'D'};
    return secciones[rand() % 4];
}

// Función para generar promedio aleatorio (10-20 en escala vigesimal)
double generarPromedio() {
    return 10.0 + (rand() % 101) / 10.0; // Entre 10.0 y 20.0
}

// Función para generar porcentaje de asistencia
int generarAsistencia() {
    return 70 + (rand() % 31); // Entre 70% y 100%
}

int main() {
    srand(time(0)); // Inicializar semilla aleatoria
    
    ofstream archivo("estudiantes.txt");
    
    if (!archivo.is_open()) {
        cerr << "Error al crear el archivo estudiantes.txt" << endl;
        return 1;
    }
    
    // Escribir encabezado del archivo
    archivo << "# Base de Datos de Estudiantes - Colegio Estatal Santa Barbara" << endl;
    archivo << "# Formato: Codigo|Nombre|Edad|Grado|Seccion|Promedio|Asistencia(%)|Becado" << endl;
    archivo << "# =============================================================================" << endl;
    archivo << endl;
    
    int totalEstudiantes = 120; // Generar 120 estudiantes (20 por grado)
    int codigoBase = 2025001;
    
    cout << "Generando datos de " << totalEstudiantes << " estudiantes..." << endl;
    
    for (int i = 0; i < totalEstudiantes; i++) {
        int grado = (i / 20) + 1; // Distribuir 20 estudiantes por grado (1-6)
        
        // Generar datos del estudiante
        int codigo = codigoBase + i;
        string nombre = generarNombreCompleto();
        int edad = generarEdad(grado);
        char seccion = generarSeccion();
        double promedio = generarPromedio();
        int asistencia = generarAsistencia();
        bool becado = (promedio >= 16.0 && rand() % 100 < 30); // 30% probabilidad si tiene buen promedio
        
        // Escribir en formato delimitado por |
        archivo << codigo << "|"
                << nombre << "|"
                << edad << "|"
                << grado << "|"
                << seccion << "|"
                << promedio << "|"
                << asistencia << "|"
                << (becado ? "SI" : "NO") << endl;
    }
    
    archivo.close();
    
    cout << "✓ Archivo 'estudiantes.txt' generado exitosamente!" << endl;
    cout << "✓ Total de estudiantes: " << totalEstudiantes << endl;
    cout << "✓ Distribución: 20 estudiantes por grado (1° a 6° de primaria)" << endl;
    cout << "✓ Secciones: A, B, C, D" << endl;
    cout << endl;
    cout << "Formato del archivo:" << endl;
    cout << "Matricula|Nombre|Edad|Grado|Seccion|Promedio|Asistencia(%)|Becado" << endl;
    cout << endl;
    cout << "El archivo está listo para ser usado por el programa de consola." << endl;
    
    return 0;
}