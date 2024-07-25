#include <iostream>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <thread>
#include <unistd.h>
#include "json.hpp"  // json handling
#include "mqtt/client.h"  // Paho MQTT
#include <iomanip>

#define QOS 1
#define BROKER_ADDRESS "tcp://maqiatto.com:1883"
#define SERIAL_PORT "/dev/ttyUSB0"
#define BAUD_RATE 115200

int main(int argc, char* argv[]) {
    // Configuração da porta serial
    serial::Serial my_serial(SERIAL_PORT, BAUD_RATE, serial::Timeout::simpleTimeout(1000));

    std::string clientId = "lucasl050503@gmail.com";
    mqtt::client client(BROKER_ADDRESS, clientId);

    // Connect to the MQTT broker.
    mqtt::connect_options connOpts;
    connOpts.set_keep_alive_interval(20);
    connOpts.set_clean_session(true);

    try {
        client.connect(connOpts);
    } catch (const mqtt::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    std::clog << "Connected to the broker" << std::endl;

    while (true) {
        // Get the current time in ISO 8601 format.
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&now_c);
        std::stringstream ss;
        ss << std::put_time(now_tm, "%FT%TZ");
        std::string timestamp = ss.str();

        // Pega os valores dos sensores e publica em cada tópico.
        if (my_serial.available()) {
            std::string line = my_serial.readline();
            std::cout << "Dados recebidos: " << line << std::endl;

            // Identificar se a mensagem é de temperatura ou umidade
            if (line.find("Temperatura: ") != std::string::npos) {
                // Construct the JSON message.
                nlohmann::json j;
                j["timestamp"] = timestamp;
                j["value"] = line;

                // Publish the JSON message to the appropriate topic.
                std::string topic = clientId + "/UPI1/temperature";
                mqtt::message msg(topic, j.dump(), QOS, false);
                std::clog << "Message published - topic: " << topic << " - message: " << line << std::endl;
                client.publish(msg);

            } else if (line.find("Umidade: ") != std::string::npos) {
                // Construct the JSON message.
                nlohmann::json j;
                j["timestamp"] = timestamp;
                j["value"] = line;

                // Publish the JSON message to the appropriate topic.
                std::string topic = clientId + "/UPI1/moisture";
                mqtt::message msg(topic, j.dump(), QOS, false);
                std::clog << "Message published - topic: " << topic << " - message: " << line << std::endl;
                client.publish(msg);
            }  
        }            

        // Sleep for some time.
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return EXIT_SUCCESS;
}
