#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <limits>

#ifdef _WIN32
    #include <windows.h>
#endif

using namespace std;

//configuración de colores (se activan si la consola soporta VT)
string C_MENU = "";
string C_RESET = "";
string C_OUT = "";

//Limpiar pantalla cross-platform
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

//intenta habilitar virtual terminal processing en Windows para que las secuencias ANSI funcionen
bool enableVirtualTerminalProcessing() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return false;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return false;

    //flag para habilitar secuencias ANSI
    //const DWORD ENABLE_VIRTUAL_TERMINAL_PROCESSING = 0x0004;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode)) return false;

    return true;
#else
    //en unix/posix suele funcionar
    return true;
#endif
}

//inicializa variables de color segun si VT fue activado
void initConsoleColors() {
    bool ok = enableVirtualTerminalProcessing();
    if (ok) {
        C_MENU = "\x1b[96m";   //color menu, cian
        C_RESET = "\x1b[0m";   //reseteo
        C_OUT = "\x1b[93m";    //color resultado, amariilo
    } else {
        C_MENU = "";
        C_RESET = "";
        C_OUT = "";
    }
}

//presiona enter para continua
void waitForEnter() {
    cout << "\nPresiona ENTER para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

//estructura estudiante

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

//arboles binarios
struct NodoArbol {
    Estudiante data;
    NodoArbol* izq;
    NodoArbol* der;
    NodoArbol(const Estudiante& e) : data(e), izq(nullptr), der(nullptr) {}
};

unordered_map<string, Estudiante> estudiantes;
NodoArbol* arbolAsistencia = nullptr;
NodoArbol* arbolPromedio = nullptr;

string ultimoCodigo = "2025481";

//formato para guardar
string estudianteToLine(const Estudiante& e) {
    string linea;
    linea += e.codigo + ",";
    linea += e.nombre + ",";
    linea += to_string(e.edad) + ",";
    linea += to_string(e.grado) + ",";
    linea += e.seccion + ",";

    ostringstream prom;
    prom << fixed << setprecision(1) << e.promedio;
    linea += prom.str() + ",";

    ostringstream asist;
    asist << fixed << setprecision(0) << e.asistencia;
    linea += asist.str() + ",";

    linea += (e.becado ? "SI" : "NO");
    return linea;
}

//mostrar estudiante
void mostrar(const Estudiante& e) {
    cout << C_OUT;
    cout << "---------------------------------\n";
    cout << "Codigo     : " << e.codigo << "\n";
    cout << "Nombre     : " << e.nombre << "\n";
    cout << "Edad       : " << e.edad << "\n";
    cout << "Grado      : " << e.grado << "\n";
    cout << "Seccion    : " << e.seccion << "\n";
    cout << "Promedio   : " << fixed << setprecision(1) << e.promedio << "\n";
    cout << "Asistencia : " << fixed << setprecision(0) << e.asistencia << "%\n";
    cout << "Becado     : " << (e.becado ? "SI" : "NO") << "\n";
    cout << C_RESET;
}

//inserción en árboles de estudiantes

NodoArbol* insertarAsistencia(NodoArbol* nodo, const Estudiante& e) {
    if (!nodo) return new NodoArbol(e);
    if (e.asistencia < nodo->data.asistencia)
        nodo->izq = insertarAsistencia(nodo->izq, e);
    else
        nodo->der = insertarAsistencia(nodo->der, e);
    return nodo;
}

NodoArbol* insertarPromedio(NodoArbol* nodo, const Estudiante& e) {
    if (!nodo) return new NodoArbol(e);
    if (e.promedio < nodo->data.promedio)
        nodo->izq = insertarPromedio(nodo->izq, e);
    else
        nodo->der = insertarPromedio(nodo->der, e);
    return nodo;
}

//Cargar datos arboles y hash
void cargarDatos() {
    ifstream archivo("estudiantes.txt");
    if (!archivo.is_open()) return;

    string linea;
    while (getline(archivo, linea)) {
        if (linea.empty() || linea[0] == '#') continue;

        stringstream ss(linea);
        Estudiante e;
        string becaStr;

        getline(ss, e.codigo, ',');
        getline(ss, e.nombre, ',');
        ss >> e.edad; ss.ignore();
        ss >> e.grado; ss.ignore();
        getline(ss, e.seccion, ',');
        ss >> e.promedio; ss.ignore();
        ss >> e.asistencia; ss.ignore();
        getline(ss, becaStr);

        e.becado = (becaStr == "SI" || becaStr == "Si" || becaStr == "si");

        estudiantes[e.codigo] = e;
        arbolAsistencia = insertarAsistencia(arbolAsistencia, e);
        arbolPromedio = insertarPromedio(arbolPromedio, e);
    }

    //actualizar ultimoCodigo matricula
    if (!estudiantes.empty()) {
        string maxCodigo = "0000000";
        for (auto& p : estudiantes)
            if (p.first > maxCodigo) maxCodigo = p.first;
        ultimoCodigo = maxCodigo;
    }
}

//busqueda por rango en árboles

void buscarPorRangoAsistencia(NodoArbol* nodo, double minA, double maxA, vector<Estudiante>& res) {
    if (!nodo) return;
    if (nodo->data.asistencia > minA)
        buscarPorRangoAsistencia(nodo->izq, minA, maxA, res);
    if (nodo->data.asistencia >= minA && nodo->data.asistencia <= maxA)
        res.push_back(nodo->data);
    if (nodo->data.asistencia < maxA)
        buscarPorRangoAsistencia(nodo->der, minA, maxA, res);
}

void buscarPorRangoPromedio(NodoArbol* nodo, double minP, double maxP, vector<Estudiante>& res) {
    if (!nodo) return;
    if (nodo->data.promedio > minP)
        buscarPorRangoPromedio(nodo->izq, minP, maxP, res);
    if (nodo->data.promedio >= minP && nodo->data.promedio <= maxP)
        res.push_back(nodo->data);
    if (nodo->data.promedio < maxP)
        buscarPorRangoPromedio(nodo->der, minP, maxP, res);
}

//Comparadores para merge sort

bool cmpCodigo(const Estudiante& a, const Estudiante& b) { return a.codigo < b.codigo; }
bool cmpEdad(const Estudiante& a, const Estudiante& b) { return a.edad < b.edad; }
bool cmpPromedio(const Estudiante& a, const Estudiante& b) { return a.promedio < b.promedio; }
bool cmpAsistencia(const Estudiante& a, const Estudiante& b) { return a.asistencia < b.asistencia; }
bool cmpNombre(const Estudiante& a, const Estudiante& b) { return a.nombre < b.nombre; }

//MergeSort

void merge(vector<Estudiante>& arr, int l, int m, int r, bool (*cmp)(const Estudiante&, const Estudiante&)) {
    int n1 = m - l + 1;
    int n2 = r - m;
    vector<Estudiante> L(n1), R(n2);

    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int i = 0; i < n2; i++) R[i] = arr[m + 1 + i];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2)
        arr[k++] = cmp(L[i], R[j]) ? L[i++] : R[j++];
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(vector<Estudiante>& arr, int l, int r, bool (*cmp)(const Estudiante&, const Estudiante&)) {
    if (l >= r) return;
    int m = (l + r) / 2;
    mergeSort(arr, l, m, cmp);
    mergeSort(arr, m + 1, r, cmp);
    merge(arr, l, m, r, cmp);
}

//ordenar y mostrar resultados
void ordenarResultados(vector<Estudiante>& resultados) {
    if (resultados.empty()) {
        cout << "No hay datos para ordenar.\n";
        return;
    }

    clearScreen();
    cout << C_MENU << "--- ORDENAR RESULTADOS ---\n" << C_RESET;
    cout << "[1] Edad\n[2] Promedio\n[3] Asistencia\n[4] Nombre\n> ";

    int op; cin >> op;

    bool (*cmp)(const Estudiante&, const Estudiante&);
    switch (op) {
        case 1: cmp = cmpEdad; break;
        case 2: cmp = cmpPromedio; break;
        case 3: cmp = cmpAsistencia; break;
        case 4: cmp = cmpNombre; break;
        default:
            cout << "Opción inválida.\n";
            waitForEnter();
            return;
    }

    mergeSort(resultados, 0, (int)resultados.size() - 1, cmp);

    clearScreen();
    cout << C_MENU << "--- RESULTADOS ORDENADOS ---\n" << C_RESET;
    for (auto& e : resultados) mostrar(e);

    //pausa
    waitForEnter();
}

//guardado a archivo

void saveStudentsToFile(const string& filename = "estudiantes.txt") {
    vector<Estudiante> lista;
    for (auto& p : estudiantes) lista.push_back(p.second);

    if (!lista.empty())
        mergeSort(lista, 0, (int)lista.size() - 1, cmpCodigo);

    ofstream out(filename, ios::trunc);
    out << "# Codigo,Nombre,Edad,Grado,Seccion,Promedio,Asistencia,Becado\n";
    for (auto& e : lista) out << estudianteToLine(e) << "\n";
}

void saveTreePreorderFull(NodoArbol* nodo, ofstream& out) {
    if (!nodo) return;
    out << estudianteToLine(nodo->data) << "\n";
    saveTreePreorderFull(nodo->izq, out);
    saveTreePreorderFull(nodo->der, out);
}

void saveTreeToFile(NodoArbol* root, const string& filename) {
    ofstream out(filename, ios::trunc);
    out << "# Arbol en preorden\n";
    saveTreePreorderFull(root, out);
}

void saveAllStructures() {
    saveStudentsToFile("estudiantes.txt");
    saveTreeToFile(arbolAsistencia, "arbol_asistencia.txt");
    saveTreeToFile(arbolPromedio, "arbol_promedio.txt");
}

//buscar por cod matricula
void buscarPorCodigo() {
    clearScreen();
    cout << C_MENU << "--- BUSCAR POR CÓDIGO ---\n" << C_RESET;

    string cod;
    cout << "Código: ";
    cin >> cod;

    clearScreen();
    if (estudiantes.count(cod))
        mostrar(estudiantes[cod]);
    else
        cout << "No existe el estudiante.\n";

    waitForEnter();
}

//busqueda personalizada, secuncial y con arreglo dinamico

void buscar() {
    string filtroNombre = "";
    string filtroSeccion = "";
    bool filtrarEdad = false, filtrarGrado = false, filtrarSeccionB = false, filtrarProm = false, filtrarAsist = false, filtrarBeca = false;

    int edadMin = 0, edadMax = 0;
    int gradoMin = 0, gradoMax = 0;
    double promMin = 0, promMax = 0;
    double asistMin = 0, asistMax = 0;
    bool becado = false;

    int op;
    do {
        clearScreen();
        cout << C_MENU << "--- BUSQUEDA PERSONALIZADA ---\n" << C_RESET;
        cout << "[1] Nombre          : " << (filtroNombre == "" ? "X" : filtroNombre) << "\n";
        cout << "[2] Edad (min-max)  : " << (filtrarEdad ? to_string(edadMin)+"-"+to_string(edadMax) : "X") << "\n";
        cout << "[3] Grado (min-max) : " << (filtrarGrado ? to_string(gradoMin)+"-"+to_string(gradoMax) : "X") << "\n";
        cout << "[4] Seccion         : " << (filtrarSeccionB ? filtroSeccion : "X") << "\n";
        cout << "[5] Promedio (min-max): " << (filtrarProm ? to_string(promMin)+"-"+to_string(promMax) : "X") << "\n";
        cout << "[6] Asistencia (min-max): " << (filtrarAsist ? to_string(asistMin)+"-"+to_string(asistMax) : "X") << "\n";
        cout << "[7] Becado (SI/NO)  : " << (filtrarBeca ? (becado ? "SI" : "NO") : "X") << "\n";
        cout << "[8] Mostrar resultados\n";
        cout << "[9] Volver\n> ";
        cin >> op;

        switch(op) {
            case 1:
                cin.ignore();
                cout << "Nombre a buscar: ";
                getline(cin, filtroNombre);
                break;
            case 2:
                cout << "Edad mínima: "; cin >> edadMin;
                cout << "Edad máxima: "; cin >> edadMax;
                filtrarEdad = true;
                break;
            case 3:
                cout << "Grado mínimo: "; cin >> gradoMin;
                cout << "Grado máximo: "; cin >> gradoMax;
                filtrarGrado = true;
                break;
            case 4:
                cin.ignore();
                cout << "Seccion: ";
                getline(cin, filtroSeccion);
                filtrarSeccionB = true;
                break;
            case 5:
                cout << "Promedio min: "; cin >> promMin;
                cout << "Promedio max: "; cin >> promMax;
                filtrarProm = true;
                break;
            case 6:
                cout << "Asistencia min: "; cin >> asistMin;
                cout << "Asistencia max: "; cin >> asistMax;
                filtrarAsist = true;
                break;
            case 7: {
                char b;
                cout << "Becado (S/N): ";
                cin >> b;
                filtrarBeca = true;
                becado = (b=='S'||b=='s');
                break;
            }
            case 8: {
                vector<Estudiante> resultados;
                for (auto& p : estudiantes) {
                    Estudiante e = p.second;

                    if (filtroNombre != "" && e.nombre.find(filtroNombre) == string::npos) continue;
                    if (filtrarEdad && !(e.edad >= edadMin && e.edad <= edadMax)) continue;
                    if (filtrarGrado && !(e.grado >= gradoMin && e.grado <= gradoMax)) continue;
                    if (filtrarSeccionB && e.seccion != filtroSeccion) continue;
                    if (filtrarProm && !(e.promedio >= promMin && e.promedio <= promMax)) continue;
                    if (filtrarAsist && !(e.asistencia >= asistMin && e.asistencia <= asistMax)) continue;
                    if (filtrarBeca && e.becado != becado) continue;

                    resultados.push_back(e);
                }

                clearScreen();
                if (resultados.empty()) {
                    cout << "No se encontraron resultados.\n";
                    waitForEnter();
                } else {
                    cout << C_MENU << "--- RESULTADOS ---\n" << C_RESET;

                    for (auto& e : resultados) mostrar(e);

                    cout << "\n¿Deseas ordenar los resultados? (1=SI / 0=NO): ";
                    int o; cin >> o;
                    if (o == 1) {
                        ordenarResultados(resultados);
                    } else {
                        //esperar a volver
                        waitForEnter();
                    }
                }
            }
            break;
        }

    } while (op != 9);
}


//menu de arboles
void menuBusquedaArbol() {
    int op;
    do {
        clearScreen();
        cout << C_MENU << "--- BUSQUEDA POR ARBOLES ---\n" << C_RESET;
        cout << "[1] Buscar por rango de asistencia\n";
        cout << "[2] Buscar por rango de promedio\n";
        cout << "[3] Volver\n> ";
        cin >> op;

        if (op == 1) {
            double minA, maxA;
            cout << "Asistencia mínima: "; cin >> minA;
            cout << "Asistencia máxima: "; cin >> maxA;

            vector<Estudiante> resultados;
            buscarPorRangoAsistencia(arbolAsistencia, minA, maxA, resultados);

            clearScreen();
            if (resultados.empty()) {
                cout << "Sin resultados.\n";
                waitForEnter();
            } else {
                for (auto& e : resultados) mostrar(e);
                cout << "\n¿Ordenar? (1=SI / 0=NO): ";
                int o; cin >> o;
                if (o==1) ordenarResultados(resultados);
                else waitForEnter();
            }
        }

        else if (op == 2) {
            double minP, maxP;
            cout << "Promedio mínimo: "; cin >> minP;
            cout << "Promedio máximo: "; cin >> maxP;

            vector<Estudiante> resultados;
            buscarPorRangoPromedio(arbolPromedio, minP, maxP, resultados);

            clearScreen();
            if (resultados.empty()) {
                cout << "Sin resultados.\n";
                waitForEnter();
            } else {
                for (auto& e : resultados) mostrar(e);
                cout << "\n¿Ordenar? (1=SI / 0=NO): ";
                int o; cin >> o;
                if (o==1) ordenarResultados(resultados);
                else waitForEnter();
            }
        }

    } while (op != 3);
}

//Generar el codigo que sigue
string generarCodigo() {
    string anio = ultimoCodigo.substr(0, 4);
    int num = stoi(ultimoCodigo.substr(4));
    num++;

    stringstream ss;
    ss << anio << setw(3) << setfill('0') << num;

    ultimoCodigo = ss.str();
    return ultimoCodigo;
}

//registrar estudiante, actualiza las estructuras arboles hash y eso
void registrarEstudiante() {
    clearScreen();
    cout << C_MENU << "--- REGISTRAR ESTUDIANTE ---\n" << C_RESET;

    Estudiante e;
    e.codigo = generarCodigo();
    cout << "Código generado: " << e.codigo << "\n";

    if (estudiantes.count(e.codigo)) {
        cout << "Error: El código ya existe.\n";
        waitForEnter();
        return;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Nombre: "; getline(cin, e.nombre);
    cout << "Edad: "; cin >> e.edad;
    cout << "Grado: "; cin >> e.grado;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Sección: "; getline(cin, e.seccion);
    cout << "Promedio: "; cin >> e.promedio;
    cout << "Asistencia (%): "; cin >> e.asistencia;

    char b;
    cout << "Becado (S/N): "; cin >> b;
    e.becado = (b=='S'||b=='s');

    estudiantes[e.codigo] = e;
    arbolAsistencia = insertarAsistencia(arbolAsistencia, e);
    arbolPromedio = insertarPromedio(arbolPromedio, e);

    saveAllStructures();

    cout << "Registrado correctamente.\n";
    waitForEnter();
}


//menu principal
void menuPrincipal() {
    int op;
    do {
        clearScreen();
        cout << C_MENU;
        cout << "--- MENU ---\n";
        cout << "[1] Buscar por código\n";
        cout << "[2] Búsqueda personalizada\n";
        cout << "[3] Búsqueda por árboles\n";
        cout << "[4] Registrar estudiante\n";
        cout << "[5] Salir\n> ";
        cout << C_RESET;

        cin >> op;

        switch(op) {
            case 1: buscarPorCodigo(); break;
            case 2: buscar(); break;
            case 3: menuBusquedaArbol(); break;
            case 4: registrarEstudiante(); break;
            default: break;
        }

    } while (op != 5);
}


int main() {
    //inicializar colores
    initConsoleColors();

    cargarDatos();
    menuPrincipal();
    return 0;
}
