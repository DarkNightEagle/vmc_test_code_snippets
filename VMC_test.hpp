#ifndef VMC_TEST_HPP
#define VMC_TEST_HPP
#include <boost/asio.hpp>

#include <vector>
#include <string>
#include <cstdint>
#include <iostream>

// Функция для добавления строки с выравниванием OSC (4 байта)
void inline push_osc_string(std::vector<uint8_t>& buffer, const std::string& str) {
    for (char c : str) buffer.push_back(c);
    buffer.push_back('\0');
    while (buffer.size() % 4 != 0) buffer.push_back('\0');
}

// Функция добавления int32 в сетевом порядке байтов
void inline push_osc_int(std::vector<uint8_t>& buffer, int32_t val) {
    uint32_t net_val = htonl(val); // Перевод в Big-endian
    uint8_t bytes[4];
    std::memcpy(bytes, &net_val, 4);
    for (int i = 0; i < 4; i++) buffer.push_back(bytes[i]);
}

std::vector<uint8_t> inline generate_inochi_key_packet(std::string key_name, int32_t is_pressed) {
    std::vector<uint8_t> packet;

    // 1. Адрес. В VMC для клавиш часто используется этот путь:
    push_osc_string(packet, "/VMC/Ext/Key");

    // 2. Типы: 's' (string - имя клавиши), 'i' (int - нажата 1 или 0)
    push_osc_string(packet, ",si");

    // 3. Данные
    push_osc_string(packet, key_name); // Например, "W" или "F16"
    push_osc_int(packet, is_pressed);   // 1 если нажата, 0 если отпущена

    return packet;
}

class testVMC
{
public:
    testVMC()
    {

    }
    ~testVMC()
    {

    }

    void run()
    {
        testTimer.expires_after(std::chrono::milliseconds{100});
        testTimer.async_wait([this](const boost::system::error_code& ec) {
            timerCallback(ec);
            });
        boost::asio::io_context::work worker{vmcContext};
        vmcContext.run();
    }
    void timerCallback(const boost::system::error_code& ec)
    {
        try
        {
            std::vector<uint8_t> generatedPacket = generate_inochi_key_packet("Key_W", 1);
            udpSock.send_to(boost::asio::buffer(generatedPacket), remoteendpoint);
            std::cout << "packet sended" << std::endl;
        }
        catch (const std::exception& e)
        {

        }
        testTimer.expires_after(std::chrono::milliseconds{400});
        testTimer.async_wait([this](const boost::system::error_code& ec) {
            timerCallback(ec);
            });
    }
private:
    boost::asio::io_context vmcContext;
    boost::asio::ip::udp::socket udpSock{vmcContext, boost::asio::ip::udp::endpoint{boost::asio::ip::udp::v4(), 7999}};
    boost::asio::ip::udp::endpoint remoteendpoint{boost::asio::ip::make_address("127.0.0.1"), 8000};
    boost::asio::steady_timer testTimer{vmcContext};
    

};
#endif // !VMC_TEST_HPP
