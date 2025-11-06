#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <algorithm>

using namespace std;

struct Estudiante {
    string codigo;
    string nombre;
    int edad;
    int grado;
    string seccion;
    double promedio;
    double asistencia;
    bool becado;
};

unordered_map<string, Estudiante> estudiantes;

// Función para cargar archivo
void cargarDatos() {
    ifstream archivo("estudiantes.txt");
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo estudiantes.txt\n";
        return;
    }

    string linea;
    while (getline(archivo, linea)) {
        if (linea.size() == 0 || linea[0] == '#') continue;

        stringstream ss(linea);
        Estudiante e;
        string becaStr;

        getline(ss, e.codigo, '|');
        getline(ss, e.nombre, '|');
        ss >> e.edad;
        ss.ignore();
        ss >> e.grado;
        ss.ignore();
        getline(ss, e.seccion, '|');
        ss >> e.promedio;
        ss.ignore();
        ss >> e.asistencia;
        ss.ignore();
        getline(ss, becaStr);

        e.becado = (becaStr == "SI" || becaStr == "Si" || becaStr == "si");

        estudiantes[e.codigo] = e;
    }
}

// Mostrar datos
void mostrar(const Estudiante& e) {
    cout << "---------------------------------\n";
    cout << "Codigo     : " << e.codigo << "\n";
    cout << "Nombre     : " << e.nombre << "\n";
    cout << "Edad       : " << e.edad << "\n";
    cout << "Grado      : " << e.grado << "\n";
    cout << "Seccion    : " << e.seccion << "\n";
    cout << "Promedio   : " << e.promedio << "\n";
    cout << "Asistencia : " << e.asistencia << "%\n";
    cout << "Becado     : " << (e.becado ? "SI" : "NO") << "\n";
}

// Función de búsqueda personalizada
void buscar() {
    string nombre = "";
    bool filtrarEdad = false, filtrarGrado = false, filtrarSeccion = false, filtrarProm = false, filtrarAsist = false, filtrarBeca = false;
    int edadMin = 0, edadMax = 0;
    int gradoMin = 0, gradoMax = 0;
    double promMin = 0, promMax = 0;
    double asistMin = 0, asistMax = 0;
    bool becado = false;

    int op;
    do {
        cout << "\n--- BUSQUEDA PERSONALIZADA ---\n";
        cout << "[1] Nombre          : " << (nombre == "" ? "X" : nombre) << "\n";
        cout << "[2] Edad (min-max)  : " << (filtrarEdad ? to_string(edadMin) + "-" + to_string(edadMax) : "X") << "\n";
        cout << "[3] Grado (min-max) : " << (filtrarGrado ? to_string(gradoMin) + "-" + to_string(gradoMax) : "X") << "\n";
        cout << "[4] Seccion         : " << (filtrarSeccion ? "✔" : "X") << "\n";
        cout << "[5] Promedio (min-max): " << (filtrarProm ? to_string(promMin) + "-" + to_string(promMax) : "X") << "\n";
        cout << "[6] Asistencia (min-max): " << (filtrarAsist ? to_string(asistMin) + "-" + to_string(asistMax) : "X") << "\n";
        cout << "[7] Becado (SI/NO)  : " << (filtrarBeca ? (becado ? "SI" : "NO") : "X") << "\n";
        cout << "[8] Mostrar resultados\n";
        cout << "[9] Volver\n";
        cout << "> ";
        cin >> op;

        switch(op) {
            case 1: cin.ignore(); cout << "Nombre a buscar: "; getline(cin, nombre); break;
            case 2: cout << "Edad mínima: "; cin >> edadMin; cout << "Edad máxima: "; cin >> edadMax; filtrarEdad=true; break;
            case 3: cout << "Grado mínimo: "; cin >> gradoMin; cout << "Grado máximo: "; cin >> gradoMax; filtrarGrado=true; break;
            case 4: cin.ignore(); cout << "Seccion: "; getline(cin, nombre); filtrarSeccion=true; break;
            case 5: cout << "Promedio min: "; cin >> promMin; cout << "Promedio max: "; cin >> promMax; filtrarProm=true; break;
            case 6: cout << "Asistencia min: "; cin >> asistMin; cout << "Asistencia max: "; cin >> asistMax; filtrarAsist=true; break;
            case 7: char b; cout << "Becado (S/N): "; cin >> b; filtrarBeca=true; becado = (b=='S'||b=='s'); break;
            case 8:
                for (auto &p : estudiantes) {
                    Estudiante e = p.second;
                    if (nombre != "" && e.nombre.find(nombre) == string::npos) continue;
                    if (filtrarEdad && !(e.edad >= edadMin && e.edad <= edadMax)) continue;
                    if (filtrarGrado && !(e.grado >= gradoMin && e.grado <= gradoMax)) continue;
                    if (filtrarSeccion && !(e.seccion == nombre)) continue;
                    if (filtrarProm && !(e.promedio >= promMin && e.promedio <= promMax)) continue;
                    if (filtrarAsist && !(e.asistencia >= asistMin && e.asistencia <= asistMax)) continue;
                    if (filtrarBeca && !(e.becado == becado)) continue;
                    mostrar(e);
                }
                break;
        }
    } while (op != 9);
}

// Buscar por código directamente
void buscarPorCodigo() {
    string cod;
    cout << "Codigo: ";
    cin >> cod;
    if (estudiantes.count(cod))
        mostrar(estudiantes[cod]);
    else
        cout << "No existe el estudiante.\n";
}

int main() {
    cargarDatos();
    int op;
    do {
        cout << "\n--- MENU ---\n";
        cout << "[1] Buscar por codigo\n";
        cout << "[2] Busqueda personalizada\n";
        cout << "[3] Salir\n> ";
        cin >> op;

        if (op == 1) buscarPorCodigo();
        else if (op == 2) buscar();
    } while (op != 3);

    return 0;
}
