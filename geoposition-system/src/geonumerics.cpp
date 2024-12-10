#include "geonumerics.h"

using json = nlohmann::json; // JSON library
using namespace alexTMqtt; // Broker MQTT

const string& TEST_BROKER_URI = "mqtt://mosquitto:1883";
const string& TEST_PERSIST_DIR = "./.work/persist";

const string& GNSS_BINARY_FILE = "gonumerics_gnss.bin";

void geo_system_process_ins() {
    // Crea un cliente MQTT para la adquisición de datos INS
    AdquisitionMqttClient adq_client = AdquisitionMqttClient("INS", TEST_BROKER_URI, TEST_PERSIST_DIR);
    
    // Configura las opciones de conexión del cliente
    ClientConnectOptions connOptsServer;
    connOptsServer.cleanStart = true;
    adq_client.connect(connOptsServer); // Conecta al broker MQTT

    bool is_finished = false; // Bandera para indicar cuando finalizar el proceso

    // Suscribe a los datos de adquisición INS y define el callback
    adq_client.subscribeToAdquisitionINSData([&](const string& message) {
        std::cout << "geonumerics Adq INS: " << message << std::endl;

        // Verifica si el mensaje indica que debe finalizar
        if (message == "FINISH")
            is_finished = true;
    });

    // Mantiene el proceso activo hasta que se reciba un mensaje de finalización
    while (!is_finished)
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Espera un segundo antes de verificar de nuevo

    adq_client.disconnect(); // Desconecta el cliente MQTT al finalizar
}

// Convierte una cadena hexadecimal en datos binarios
std::string hexStringToBinary(const std::string& hex) {
    std::string binaryData;
    
    // Verifica si la longitud de la cadena hexadecimal es par
    if (hex.length() % 2 != 0) {
        std::cerr << "La cadena hexadecimal debe tener una longitud par." << std::endl;
        return binaryData; // Retorna una cadena vacía si la longitud no es válida
    }

    // Recorre la cadena hexadecimal en pasos de dos caracteres
    for (size_t i = 0; i < hex.length(); i += 2) {
        // Toma un par de caracteres como un byte
        std::string byteString = hex.substr(i, 2);
        
        // Convierte el par de caracteres hexadecimales a un byte (unsigned char)
        unsigned char byte = static_cast<unsigned char>(std::stoi(byteString, nullptr, 16));
        
        // Añade el byte a la cadena de datos binarios
        binaryData += byte;
    }
    return binaryData; // Retorna los datos binarios
}

// Convierte una cadena de texto en su representación hexadecimal
std::string stringToHex(const std::string& input) {
    std::stringstream hexStream;
    
    // Recorre cada carácter en la cadena de entrada
    for (unsigned char c : input) {
        // Convierte cada carácter a su valor hexadecimal y añade al flujo
        hexStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    return hexStream.str(); // Retorna la representación hexadecimal de la cadena
}

void geo_system_process_gnss() {
    // Crea un cliente MQTT para la adquisición de datos GNSS
    AdquisitionMqttClient adq_client = AdquisitionMqttClient("GNSS", TEST_BROKER_URI, TEST_PERSIST_DIR);
    
    // Configura las opciones de conexión del cliente
    ClientConnectOptions connOptsServer;
    connOptsServer.cleanStart = true;
    adq_client.connect(connOptsServer); // Conecta al broker MQTT

    // Abre un archivo binario para escribir los datos GNSS
    FILE *gnss_binary_file_ptr = fopen(GNSS_BINARY_FILE.c_str(), "wb");
    if (gnss_binary_file_ptr == nullptr) {
        std::cerr << "Error al abrir el archivo binario gnss" << std::endl;
        return; // Sale de la función si no se puede abrir el archivo
    }

    bool is_finished = false; // Bandera para indicar cuando finalizar el proceso

    // Suscribe a los datos de adquisición GNSS y define el callback
    adq_client.subscribeToAdquisitionGNSSData([&](const string& message) {
        std::cout << "geonumerics Adq GNSS: " << message << std::endl;

        try {
            // Intenta parsear el mensaje JSON recibido
            json j = json::parse(message.c_str());
            std::string body = j["Message"];
            std::string bin  = hexStringToBinary(body); // Convierte el mensaje hexadecimal a binario
            fwrite(bin.c_str(), sizeof(char), bin.size(), gnss_binary_file_ptr); // Escribe los datos binarios en el archivo
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl; // Muestra cualquier error de parsing
        }

        // Verifica si el mensaje indica que debe finalizar
        if (message == "FINISH")
            is_finished = true;
    });

    // Mantiene el proceso activo hasta que se reciba un mensaje de finalización
    while (!is_finished)
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Espera 1 segundo antes de verificar nuevamente
    
    adq_client.disconnect(); // Desconecta del broker MQTT
    fclose(gnss_binary_file_ptr); // Cierra el archivo binario
}

void geo_system_write_nexa(){
	AdquisitionMqttClient adq_client = AdquisitionMqttClient("NEXA", TEST_BROKER_URI, TEST_PERSIST_DIR);
	ClientConnectOptions connOptsServer;
	connOptsServer.cleanStart = true;
    adq_client.connect(connOptsServer);

	while (42){
		auto t = std::time(nullptr);
    	auto tm = *std::localtime(&t);
    	std::ostringstream oss;
    	oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    	std::string current_date_time = oss.str();

		adq_client.publishGeopositionData("NEXA" + current_date_time);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	adq_client.disconnect();

}

int main() {
    // Crea un hilo para ejecutar la función geo_system_process_ins
    std::thread geo_system_process_ins_thread(geo_system_process_ins);

    // Crea otro hilo para ejecutar la función geo_system_process_gnss
    std::thread geo_system_process_gnss_thread(geo_system_process_gnss);

    // Crea un hilo para ejecutar la función geo_system_write_nexa
    std::thread geo_system_write_nexa_thread(geo_system_write_nexa);


    // Verifica si el hilo geo_system_process_ins_thread se puede unir y lo une
    if (geo_system_process_ins_thread.joinable()) {
        geo_system_process_ins_thread.join(); // Espera a que el hilo termine su ejecución
    }

    // Verifica si el hilo geo_system_process_gnss_thread se puede unir y lo une
    if (geo_system_process_gnss_thread.joinable()) {
        geo_system_process_gnss_thread.join(); // Espera a que el hilo termine su ejecución
    }
    
    // Verifica si el hilo geo_system_write_nexa_thread se puede unir y lo une
    if (geo_system_write_nexa_thread.joinable()) {
		geo_system_write_nexa_thread.join();
	}
}

