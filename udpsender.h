#ifndef UDPSENDER_H
#define UDPSENDER_H

#include <fstream>
#include <functional>
#include <numeric>
#include <list>
#include <vector>
#include <string>


#include <boost/asio.hpp>
using namespace boost::asio;

#include "udpnetobjectseeker.h"
#include "loaderfile.h"


class UdpSender
{
public:
  UdpSender(boost::asio::io_service * io_service, UdpNetObjectSeekerWorker * unos_, std::vector<PAddr> * _seekToSend)
      : endpoint_(boost::asio::ip::address_v4::from_string( "127.0.0.1" ), 3228),
      socket_(*io_service, endpoint_.protocol()),
      io_service_(io_service)
  {
      seekToSend = _seekToSend;
      unos = unos_;
      socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
  }

  void sendData(std::string data)
  {
      for(std::size_t i=0; i<seekToSend->size(); i++){
          boost::asio::ip::udp::endpoint ep( boost::asio::ip::address_v4::from_string( seekToSend->at(i).addr ), seekToSend->at(i).port );
          try{
              socket_.send_to( boost::asio::buffer(data.c_str(), data.size()), ep );
          }
          catch(...){
              std::cout << "ERROR in UdpSender::sendData by addr=" << seekToSend->at(i).addr << std::endl;
          }
      }
  }

private:
  UdpNetObjectSeekerWorker * unos;
  std::vector<PAddr> * seekToSend;
  boost::asio::ip::udp::endpoint endpoint_;
  boost::asio::ip::udp::socket socket_;
  boost::asio::io_service * io_service_;
};



#endif // UDPSENDER_H
