#include "sitep.h"

using json = nlohmann::json; // JSON library
using namespace alexTMqtt;

// Constante que representa el desfase actual entre UTC y GPS en segundos
const int GPS_UTC_OFFSET = 18; // Este valor puede cambiar con nuevos segundos intercalares
const int BUFFER_SIZE = 65536;

// uncomment to use from Docker
// const string& TEST_BROKER_URI = "mqtt://mosquitto:1883";
// const string& TEST_PERSIST_DIR = "./.work/persist";

// comment to use from Docker
const string& TEST_BROKER_URI = "mqtt://localhost:1884";
const string& TEST_PERSIST_DIR = "/tmp/persist/";


std::chrono::system_clock::time_point parse_time(const std::string& ts) {
    std::istringstream ss(ts);
    std::tm dt = {};
    char dot;
    double seconds;

    // Parsear hasta los segundos enteros
    ss >> std::get_time(&dt, "%Y-%m-%dT%H:%M:");

    // Leer la parte de los segundos y decimales
    if (ss >> seconds) {
        // Convertir a time_t
        std::time_t time = std::mktime(&dt);

        // Crear un time_point y añadir los segundos decimales
        return std::chrono::system_clock::from_time_t(time) + std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::duration<double>(seconds));
    } else {
        throw std::runtime_error("Formato de tiempo no válido");
    }
}


// Función para leer datos INS desde un archivo y publicarlos
void geo_system_read_ins(const std::string& ins_filename) {
    std::ifstream file(ins_filename); // Abre el archivo para lectura
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << ins_filename << std::endl;
        return;
    }

	GeopositionMqttClient geo_client = GeopositionMqttClient("geo_INS", TEST_BROKER_URI, TEST_PERSIST_DIR);
	ClientConnectOptions connOptsServer;
	connOptsServer.cleanStart = true;
    geo_client.connect(connOptsServer);

    bool is_first = true;
    auto start_time = std::chrono::system_clock::now();
    auto first_time = std::chrono::system_clock::now();
       
    std::string line;
    while (std::getline(file, line)) { // Lee el archivo línea por línea
//
        // Parsear JSON
        auto json_object = json::parse(line);
        std::string ts = json_object.at("ts");
        auto message_time = parse_time(ts);
        
        if (is_first) {
            first_time = message_time;
            is_first = false;
        }

        while (true) {
            auto current_time = std::chrono::system_clock::now();

            if (current_time -  start_time >= message_time - first_time) {
                // std::cerr << "break: " << std::endl;
                break;
            }
            // std::cerr << "sleep: " << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    //    std::cout << "sitep INS data: " << line << std::endl;
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
    GeopositionMqttClient geo_client = GeopositionMqttClient("geo_GNSS", TEST_BROKER_URI, TEST_PERSIST_DIR);
    
    // Configura las opciones de conexión del cliente
    ClientConnectOptions connOptsServer;
    connOptsServer.cleanStart = true;
    geo_client.connect(connOptsServer); // Conecta al broker MQTT

    bool is_first = true;
    auto start_time = std::chrono::system_clock::now();
    auto first_time = std::chrono::system_clock::now();

    std::string line;
    // Lee el archivo línea por línea
    while (std::getline(file, line)) {

        // Parsear JSON
        auto json_object = json::parse(line);
        std::string ts = json_object.at("ts");
        auto message_time = parse_time(ts);
        
        if (is_first) {
            first_time = message_time;
            is_first = false;
        }

        while (true) {
            auto current_time = std::chrono::system_clock::now();

            if (current_time -  start_time >= message_time - first_time) {
                // std::cerr << "break: " << std::endl;
                break;
            }
            // std::cerr << "sleep: " << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

    //    std::cout << "sitep GNSS data: " << line << std::endl;
        geo_client.publishAdquisitionGNSSData(line.c_str()); // Publica cada línea leída
    }

    geo_client.publishAdquisitionGNSSData("FINISH"); // Indica que se ha terminado de procesar
    geo_client.disconnect(); // Desconecta el cliente MQTT
}


void geo_system_read_nexa(){

   	GeopositionMqttClient geo_client = GeopositionMqttClient("geo_NEXA", TEST_BROKER_URI, TEST_PERSIST_DIR);
	ClientConnectOptions connOptsServer;
	connOptsServer.cleanStart = true;
    geo_client.connect(connOptsServer);

    bool is_finished = false;

	geo_client.subscribeToGeopositionData([&](const string& message) {
		std::cout << "sitep read NEXA :" << message << std::endl;
		{
			if (message == "FINISH") {
				is_finished = true;
			}
		}
		});

	while (!is_finished) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	geo_client.disconnect();

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
    std::thread geo_system_read_nexa_thread(geo_system_read_nexa);

    // Une los hilos antes de finalizar el programa
    if (geo_system_read_ins_thread.joinable()) {
        geo_system_read_ins_thread.join();
    }
    if (geo_system_read_gnss_thread.joinable()) {
        geo_system_read_gnss_thread.join();
    }
    if (geo_system_read_nexa_thread.joinable()) {
		geo_system_read_nexa_thread.join();
    }
    return 0;
}