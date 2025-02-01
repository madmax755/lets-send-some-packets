#ifndef MPEG_TS_PACKET_HPP
#define MPEG_TS_PACKET_HPP

#include <cstdint>
#include <vector>

class MPEGTSPacket {
public:
    static const uint8_t SYNC_BYTE;
    static const uint8_t HEADER_SIZE;
    static const uint8_t PACKET_SIZE;

    uint16_t pid;
    std::vector<uint8_t> data;
    uint8_t continuity_counter;

    uint8_t transport_error_indicator;
    uint8_t payload_unit_start_indicator;
    uint8_t transport_priority;
    uint8_t adaptation_field_control;

    MPEGTSPacket(uint16_t pid, std::vector<uint8_t> data, uint8_t continuity_counter = 0);
    std::vector<uint8_t> pack();
    static MPEGTSPacket unpack(const std::vector<uint8_t>& packet);

};

#endif // MPEG_TS_PACKET_HPP
