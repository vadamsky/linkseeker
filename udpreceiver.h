#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include <fstream>
#include <functional>
#include <numeric>
#include <list>
#include <vector>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "udpnetobjectseeker.h"

using namespace boost::asio;

class UdpReceiver
{
public:
    //  handle_receive_from
    UdpReceiver(boost::asio::io_service *ioService_, UdpNetObjectSeekerWorker * unos_, bool _seek_short, std::string multicastaddr, int port_) :ioService(ioService_), socket_(*ioService_) {
        seek_short = _seek_short;
        unos = unos_;
        port = port_;
        listen_address = boost::asio::ip::address_v4::from_string("0.0.0.0");
        multicast_address = boost::asio::ip::address_v4::from_string(multicastaddr);

        list();
    }


    void list()
    {
        boost::asio::ip::udp::endpoint listen_endpoint(listen_address, port);
        try
        {
            socket_.open(listen_endpoint.protocol());
            socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
            socket_.bind(listen_endpoint);
        }
        catch(std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }

        socket_.async_receive_from(
            boost::asio::buffer(data_, max_length), endpoint_,
            boost::bind(&UdpReceiver::handle_receive_from, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
    }


    void handle_receive_from(const boost::system::error_code& error,
          size_t bytes_recvd)
      {
        if (!error)
        {
            if(seek_short)
                unos->udpSeekPacketWasIncoming(data_, bytes_recvd, endpoint_.address().to_string());
            if(!seek_short)
                unos->udpShortPacketWasIncoming(data_, bytes_recvd, endpoint_.address().to_string());

            socket_.cancel();
            socket_.close();
            list();
        }
        else
            std::cout << "ERROR in UdpReceiver::handle_receive_from: " << error.message() << std::endl;
      }


private:
    bool seek_short;
    UdpNetObjectSeekerWorker * unos;
    boost::asio::io_service *ioService;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint endpoint_;
    boost::asio::ip::address_v4 listen_address;
    boost::asio::ip::address_v4 multicast_address;
    int port;
    enum { max_length = 1024 };
    char data_[max_length];
};




#endif // UDPRECEIVER_H
