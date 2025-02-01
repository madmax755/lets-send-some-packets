#include "mpeg-ts-packet.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

int main() {
    // create UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    // setup address to listen on
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5000);
    addr.sin_addr.s_addr = INADDR_ANY;

    // bind socket
    if (bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "Failed to bind socket\n";
        close(sock);
        return 1;
    }

    std::cout << "Listening for MPEG-TS packets...\n";

    std::vector<uint8_t> buffer(MPEGTSPacket::PACKET_SIZE);

    while (true) {
        try {
            // receive packet
            sockaddr_in sender_addr{};
            socklen_t sender_addr_len = sizeof(sender_addr);
            
            ssize_t received = recvfrom(sock, 
                                      buffer.data(), 
                                      buffer.size(), 
                                      0,
                                      reinterpret_cast<sockaddr*>(&sender_addr), 
                                      &sender_addr_len);

            if (received < 0) {
                std::cerr << "Failed to receive packet\n";
                break;
            }

            // parse packet
            MPEGTSPacket packet = MPEGTSPacket::unpack(buffer);

            // Instead of directly printing the CC value, format it as a number
            std::cout << "Received: PID=" << packet.pid 
            << ", CC=" << static_cast<int>(packet.continuity_counter)  // Cast to int for proper display
            << ", Data: " << packet.data.size() << " bytes\n";

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }

    close(sock);
    return 0;
}