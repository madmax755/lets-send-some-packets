#include "mpeg-ts-packet.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    // create UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    // setup destination address
    sockaddr_in dest_addr{};
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(5000);
    inet_pton(AF_INET, "127.0.0.1", &dest_addr.sin_addr);

    // create test packets
    uint16_t video_pid = 0x100;
    uint8_t continuity_counter = 0;

    // send packets indefinitely
    while (true) {
        try {
            // create dummy payload (replace with real video data later)
            std::string dummy_data = "Frame data " + std::to_string(continuity_counter);
            std::vector<uint8_t> payload(dummy_data.begin(), dummy_data.end());

            // create and pack MPEG-TS packet
            MPEGTSPacket packet(video_pid, payload, continuity_counter);
            std::vector<uint8_t> packed_data = packet.pack();

            // send packet
            ssize_t sent = sendto(sock, 
                                packed_data.data(), 
                                packed_data.size(), 
                                0,
                                reinterpret_cast<sockaddr*>(&dest_addr), 
                                sizeof(dest_addr));

            if (sent < 0) {
                std::cerr << "Failed to send packet\n";
                break;
            }

            std::cout << "Sent: packet with PID " << packet.pid << "\n";

            // update continuity counter
            continuity_counter = (continuity_counter + 1) % 16;

            // delay between packets
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            break;
        }
    }

    close(sock);
    return 0;
}