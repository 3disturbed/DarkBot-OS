#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

using namespace boost::asio;
using websocketpp::connection_hdl;
typedef websocketpp::server<websocketpp::config::asio> server;

std::string serial_port_name = "/dev/ttyS1"; // Adjust to your serial port
unsigned int baud_rate = 115200;
io_service io;
serial_port serial(io, serial_port_name);

void on_message(server* s, connection_hdl hdl, server::message_ptr msg) {
    std::string payload = msg->get_payload();
    std::cout << "Received message: " << payload << std::endl;
    write(serial, buffer(payload + "\n"));
}

void read_from_serial(server* s, connection_hdl hdl) {
    boost::asio::streambuf buf;
    while (true) {
        boost::asio::read_until(serial, buf, "\n");
        std::istream is(&buf);
        std::string line;
        std::getline(is, line);
        std::cout << "From Teensy: " << line << std::endl;
        s->send(hdl, line, websocketpp::frame::opcode::text);
    }
}

int main() {
    server ws_server;

    try {
        serial.set_option(serial_port_base::baud_rate(baud_rate));

        ws_server.set_message_handler(bind(&on_message, &ws_server, ::_1, ::_2));
        ws_server.init_asio();
        ws_server.listen(9002);
        ws_server.start_accept();

        websocketpp::lib::thread asio_thread(&server::run, &ws_server);
        read_from_serial(&ws_server, websocketpp::lib::connection_hdl());
        asio_thread.join();
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
