#ifndef UDPNETOBJECTSEEKERWORKER_H
#define UDPNETOBJECTSEEKERWORKER_H

#include <vector>
#include <string>
#include <map>
//#include
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
using namespace boost::asio;

#include "udpnetobjectseeker.h"

struct PAddr
{
public:
    PAddr() { addr="127.0.0.1"; port=3128; }
    PAddr(std::string _addr, long _port) { addr=_addr; port=_port; }
    std::string addr;
    long port;
};

class UdpSender;
class UdpReceiver;
class LoaderFile;


class UdpNetObjectSeekerWorker
{
    friend class UdpSender;
    friend class UdpReceiver;
public:
    // is_multicast_or_filename = true:  [ string = multicast address ]
    // is_multicast_or_filename = false: [ string = filename with multicast address and addresses to send ]
    UdpNetObjectSeekerWorker(int myId_, int secondstoseek_, bool is_multicast_or_filename, std::string multicastaddress_or_filename);
    ~UdpNetObjectSeekerWorker();

    ipvector getIpvectorById(int id);
    void deleteIpFromAllIds(std::string ip);
    void deleteIpFromId(int id, std::string ip);

    void udpSeekPacketWasIncoming(char * data, int size, std::string ip);
    void udpShortPacketWasIncoming(char * data, int size, std::string ip);

    void SubscribeOnEvents(UdpNetObjectSeekersProcessor* pProcessor = NULL);
    void DescribeOnEvents(UdpNetObjectSeekersProcessor* pProcessor = NULL);

    void sendShortMessage(std::string data);

	void setParent(UdpNetObjectSeeker * _parent) { parent = _parent; }

private:
    LoaderFile * getLoaderFile() { return loaderfile; }

    void startTimer(int seconds);
    void handle_timeout(const boost::system::error_code& error);


    std::vector<UdpNetObjectSeekersProcessor*> subscribers;
    std::map<int, ipvector> idToIPmap;
    //std::multimap<int, std::string> idToIPmap;

    int secondstoseek;
    int myId;

	UdpNetObjectSeeker * parent;
    UdpSender   * udpSeekSender;
    UdpSender   * udpShortSender;
    std::vector<UdpReceiver*> udpSeekReceivers;
    std::vector<UdpReceiver*> udpShortReceivers;
    LoaderFile * loaderfile;
    boost::asio::io_service service_send;
    boost::asio::io_service service_recv;
    boost::asio::deadline_timer timer_;
    boost::thread * io_thread_send;
    boost::thread * io_thread_recv;
};

#endif // UDPNETOBJECTSEEKERWORKER_H
