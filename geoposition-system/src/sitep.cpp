#include "sitep.h"

using namespace alexTMqtt;

// Constante que representa el desfase actual entre UTC y GPS en segundos
const int GPS_UTC_OFFSET = 18; // Este valor puede cambiar con nuevos segundos intercalares
const int BUFFER_SIZE = 65536;


const string& TEST_BROKER_URI = "mqtt://mosquitto:1883";
const string& TEST_PERSIST_DIR = "./.work/persist";


// void executePythonScript_ins(const std::string& ins_filename) {

//     std::array<char, BUFFER_SIZE> buffer;
//     while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
//         std::cout << "sitep INS data: " << buffer.data();
//         geo_client.publishAdquisitionINSData(buffer.data());  
//     }
// 	geo_client.publishAdquisitionINSData("FINISH");
//     geo_client.disconnect();
// }


void geo_system_read_ins(const std::string& ins_filename) {
    std::ifstream file(ins_filename); // Abre el archivo para lectura
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << ins_filename << std::endl;
        return;
    }

	GeopositionMqttClient geo_client = GeopositionMqttClient("INS", TEST_BROKER_URI, TEST_PERSIST_DIR);
	ClientConnectOptions connOptsServer;
	connOptsServer.cleanStart = true;
    geo_client.connect(connOptsServer);

    std::string line;
    while (std::getline(file, line)) { // Lee el archivo línea por línea
        std::cout << "sitep INS data: " << line << std::endl;
        geo_client.publishAdquisitionINSData(line.c_str()); // Publica cada línea leída
    }

    geo_client.publishAdquisitionINSData("FINISH"); // Indica que se ha terminado de procesar
    geo_client.disconnect(); // Desconecta el cliente
}

// Función para leer datos GNSS desde un archivo y publicarlos
void geo_system_read_gnss(const std::string& gns_filename) {
    // Abre el archivo para lectura
    std::ifstream file(gns_filename);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << gns_filename << std::endl;
        return; // Sale de la función si no se puede abrir el archivo
    }

    // Crea un cliente MQTT para la publicación de datos GNSS
    GeopositionMqttClient geo_client = GeopositionMqttClient("GNSS", TEST_BROKER_URI, TEST_PERSIST_DIR);
    
    // Configura las opciones de conexión del cliente
    ClientConnectOptions connOptsServer;
    connOptsServer.cleanStart = true;
    geo_client.connect(connOptsServer); // Conecta al broker MQTT

    std::string line;
    // Lee el archivo línea por línea
    while (std::getline(file, line)) {
        std::cout << "sitep GNSS data: " << line << std::endl;
        geo_client.publishAdquisitionGNSSData(line.c_str()); // Publica cada línea leída
    }

    geo_client.publishAdquisitionGNSSData("FINISH"); // Indica que se ha terminado de procesar
    geo_client.disconnect(); // Desconecta el cliente MQTT
}



int main(int argc, char* argv[]) {
    // Nombres de archivo predeterminados
    std::string ins_filename = "sitep_ins.json";
    std::string gnss_filename = "sitep_gnss.json";

    // Verifica el número de argumentos
    if (argc != 1 && argc != 3) {
        std::cerr << "Debe pasar 0 o 2 argumentos: [ins_filename gnss_filename]" << std::endl;
        return 1; // Finaliza el programa con un código de error
    }

    // Asigna los argumentos a los nombres de archivo si se proporcionan
    if (argc == 3) {
        ins_filename = argv[1];
        gnss_filename = argv[2];
    }

    // Crea hilos para leer los sistemas INS y GNSS, pasando los nombres de archivo
    std::thread geo_system_read_ins_thread(geo_system_read_ins, ins_filename);
    std::thread geo_system_read_gnss_thread(geo_system_read_gnss, gnss_filename);

    // Une los hilos antes de finalizar el programa
    if (geo_system_read_ins_thread.joinable()) {
        geo_system_read_ins_thread.join();
    }
    if (geo_system_read_gnss_thread.joinable()) {
        geo_system_read_gnss_thread.join();
    }

    return 0;
}