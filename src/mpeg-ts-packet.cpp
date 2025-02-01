#include <iostream>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <cassert>
#include "mpeg-ts-packet.hpp"
#include <stdexcept>

// define static constants
const uint8_t MPEGTSPacket::SYNC_BYTE = 0x47;   // used for alignment - decoder looks for sequence of 0x47 every 188 bytes
const uint8_t MPEGTSPacket::HEADER_SIZE = 4;    // in bytes
const uint8_t MPEGTSPacket::PACKET_SIZE = 188;  // MPEG-TS standard packet size

MPEGTSPacket::MPEGTSPacket(uint16_t pid, std::vector<uint8_t> data, uint8_t continuity_counter) {
    if (pid < 0 || pid > 8191) {
        throw std::invalid_argument("PID must be between 0 and 8191 (13 bits)");
    }
    if (continuity_counter < 0 || continuity_counter > 15) {
        throw std::invalid_argument("Continuity counter must be between 0 and 15 (4 bits)");
    }
    if (data.size() > PACKET_SIZE-HEADER_SIZE) {
        throw std::invalid_argument("Data size must be 184 bytes");
    }

    // pad data to 184 bytes
    data.resize(PACKET_SIZE-HEADER_SIZE, 0);

    this->pid = pid;
    this->data = data;
    this->continuity_counter = continuity_counter;
    
    // set default values for new packet
    // todo allow user to set these values
    this->transport_error_indicator = 0;
    this->payload_unit_start_indicator = 1;
    this->transport_priority = 0;
    this->adaptation_field_control = 1;  // adaptation field is not present by default
}

std::vector<uint8_t> MPEGTSPacket::pack() {
    assert(data.size() == PACKET_SIZE-HEADER_SIZE);
    assert(pid >= 0 && pid <= 8191);
    assert(continuity_counter >= 0 && continuity_counter <= 15);

    std::vector<uint8_t> packet(PACKET_SIZE);

    // sync byte
    packet[0] = SYNC_BYTE;

    uint8_t pid_start = static_cast<uint8_t>(pid >> 8);  // get first 5 bits of pid
    uint8_t pid_end = static_cast<uint8_t>(pid & 0xFF);  // get last 8 bits of pid

    packet[1] = transport_error_indicator << 7 | payload_unit_start_indicator << 6 | transport_priority << 5 | pid_start;
    packet[2] = pid_end;
    packet[3] = adaptation_field_control << 4 | continuity_counter;

    // copy data into packet
    std::copy(data.begin(), data.end(), packet.begin() + HEADER_SIZE);

    return packet;
}

MPEGTSPacket MPEGTSPacket::unpack(const std::vector<uint8_t>& packet) {
    assert(packet.size() == PACKET_SIZE);
    assert(packet[0] == SYNC_BYTE);

    MPEGTSPacket unpacked_packet(0, std::vector<uint8_t>(PACKET_SIZE-HEADER_SIZE, 0));

    if (packet[0] != SYNC_BYTE) {
        throw std::invalid_argument("Packet does not start with sync byte");
    }

    // unpack header
    unpacked_packet.transport_error_indicator = (packet[1] & 0b10000000 ) >> 7;
    unpacked_packet.payload_unit_start_indicator = (packet[1] & 0b01000000) >> 6;
    unpacked_packet.transport_priority = (packet[1] & 0b00100000) >> 5;
    unpacked_packet.pid = (packet[1] & 0b00011111) << 8 | packet[2];
    unpacked_packet.adaptation_field_control = (packet[3] & 0b11110000) >> 4;
    unpacked_packet.continuity_counter = packet[3] & 0b00001111;

    // unpack data
    unpacked_packet.data = std::vector<uint8_t>(packet.begin() + HEADER_SIZE, packet.end());

    return unpacked_packet;
}