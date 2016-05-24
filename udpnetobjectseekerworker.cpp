#include "udpnetobjectseekerworker.h"

#include "udpsender.h"
#include "udpreceiver.h"
#include "loaderfile.h"

UdpNetObjectSeekerWorker::UdpNetObjectSeekerWorker(int myId_, int secondstoseek_, bool is_multicast_or_filename, std::string multicastaddress_or_filename) :
    timer_(service_send, boost::posix_time::seconds(secondstoseek_))
{
    udpSeekSender = 0;
    udpShortSender = 0;
    myId = myId_;
    secondstoseek = secondstoseek_;

    try{
        loaderfile = is_multicast_or_filename ? 0 : new LoaderFile(multicastaddress_or_filename);
        //std::string multicastaddress = is_multicast_or_filename ? multicastaddress_or_filename : loaderfile->getMulticastAddress();
    }
    catch(...)
    {
        std::cout << "UdpNetObjectSeekerWorker: LoaderFile constructor error :\n";
    }

    try{
        if(is_multicast_or_filename){
            udpSeekSender = new UdpSender(&service_send, this, &loaderfile->multicastSeekToSend);
            udpShortSender = new UdpSender(&service_send, this, &loaderfile->multicastShortToSend);
        }
        else{
            udpSeekSender = new UdpSender(&service_send, this, &loaderfile->seekToSend);
            udpShortSender = new UdpSender(&service_send, this, &loaderfile->shortToSend);
        }
    }
    catch(...)
    {
        std::cout << "UdpNetObjectSeekerWorker: one of UdpSender's constructor error :\n";
    }

    try{
        //for(std::size_t i=0; i<loaderfile->multicastSeekToSend.size(); i++){
        UdpReceiver * udpSeekReceiver = new UdpReceiver(&service_recv, this, true, loaderfile->multicastaddress, loaderfile->receive_seekport);
        udpSeekReceivers.push_back(udpSeekReceiver);
        //}
        //for(std::size_t i=0; i<loaderfile->multicastShortToSend.size(); i++){
        UdpReceiver * udpShortReceiver = new UdpReceiver(&service_recv, this, false, loaderfile->multicastaddress, loaderfile->receive_shortport);
        udpShortReceivers.push_back(udpShortReceiver);
        //}
    }
    catch(...)
    {
        std::cout << "UdpNetObjectSeekerWorker: one of UdpReceiver's' constructor error :\n";
    }

    try{
        startTimer(secondstoseek);
    }
    catch(...)
    {
        std::cout << "UdpNetObjectSeekerWorker: startTimer error :\n";
    }

    try{
        io_thread_send = new boost::thread(boost::bind(&boost::asio::io_service::run, &service_send)); // run(service_)
        io_thread_recv = new boost::thread(boost::bind(&boost::asio::io_service::run, &service_recv));
//    service_.run();
    }
    catch(...)
    {
        std::cout << "UdpNetObjectSeekerWorker: io_thread constructor error :\n";
    }
}

UdpNetObjectSeekerWorker::~UdpNetObjectSeekerWorker()
{
    service_send.stop();
    service_recv.stop();
    io_thread_send->join();
    io_thread_recv->join();
    try
    {
        if(udpSeekSender) delete udpSeekSender;
        if(udpShortSender) delete udpShortSender;
        for(std::size_t i=0; i<udpSeekReceivers.size(); i++)
            delete udpSeekReceivers.at(i);
        for(std::size_t i=0; i<udpShortReceivers.size(); i++)
            delete udpShortReceivers.at(i);
        if(loaderfile) delete loaderfile;
        //delete service_;
        delete io_thread_send;
        delete io_thread_recv;
    }
    catch(...)
    {

    }
}

void UdpNetObjectSeekerWorker::SubscribeOnEvents(UdpNetObjectSeekersProcessor* pProcessor)
{
    if(pProcessor == NULL) return;
    for(std::size_t i=0; i<subscribers.size(); i++)
        if(subscribers.at(i)==pProcessor) return;
    subscribers.push_back(pProcessor);
}

void UdpNetObjectSeekerWorker::DescribeOnEvents(UdpNetObjectSeekersProcessor* pProcessor)
{
    if(pProcessor == NULL) return;
    for(std::vector<UdpNetObjectSeekersProcessor*>::iterator it=subscribers.begin(); it!=subscribers.end(); it++)
        if(*it==pProcessor){
            subscribers.erase(it);
            return;
        }
}

void UdpNetObjectSeekerWorker::startTimer(int seconds)
{
    timer_.expires_from_now(boost::posix_time::seconds(seconds));
    timer_.async_wait(
        boost::bind(&UdpNetObjectSeekerWorker::handle_timeout, this,
          boost::asio::placeholders::error));
}

void UdpNetObjectSeekerWorker::handle_timeout(const boost::system::error_code& error)
{
  if (!error)
  {
      char data[20]; sprintf(data, "%d", myId);
      udpSeekSender->sendData(data);
      startTimer(secondstoseek);
  }
}

void UdpNetObjectSeekerWorker::udpSeekPacketWasIncoming(char * data, int size, std::string ip)
{
    size = size;
    int id = atoi(data);

    std::string message;
    for(int i=0; i<size; i++)
         message.push_back(data[i]);
    std::cout << "Seek message was incoming. message=" << message << std::endl;

    UdpNetObjectSeekersProcessor * pProcessor;
    for(std::vector<UdpNetObjectSeekersProcessor*>::iterator it=subscribers.begin(); it!=subscribers.end(); it++){
        pProcessor = *it;
        pProcessor->OnSeekMessage(ip, id);
    }

    std::map<int, ipvector>::iterator it = idToIPmap.find(id);
    ipvector vec;
    if(it==idToIPmap.end()){
        vec.push_back(ip);
        idToIPmap[id] = vec;
    }
    else{
        vec = idToIPmap[id];
        for(std::size_t i=0; i<vec.size(); i++)
            if(vec[i]==ip)
                return;
        vec.push_back(ip);
        idToIPmap[id] = vec;
    }
}

void UdpNetObjectSeekerWorker::udpShortPacketWasIncoming(char * data, int size, std::string ip)
{
    std::string message;
    char temp[2];
    for(int i=0; i<size; i++)
    {
        if(i<2)
            temp[i] = data[i];
        else if(i < 11)
            continue;
        else
            message.push_back(data[i]);
    }
    std::cout << "Short message was incoming. message=" << message << std::endl;

    short int id;
    memcpy(&id, temp, 2);
    UdpNetObjectSeekersProcessor * pProcessor;
    for(std::vector<UdpNetObjectSeekersProcessor*>::iterator it=subscribers.begin(); it!=subscribers.end(); it++){
        pProcessor = *it;
        pProcessor->OnShortMessage(ip, message);
        pProcessor->OnShortMessage(id, ip, message);
    }
}

ipvector UdpNetObjectSeekerWorker::getIpvectorById(int id)
{
    std::map<int, ipvector>::iterator it = idToIPmap.find(id);
    ipvector vec;
    if(it!=idToIPmap.end())
        vec = idToIPmap[id];
    return vec;
}

void UdpNetObjectSeekerWorker::deleteIpFromAllIds(std::string ip)
{
    for(std::map<int, ipvector>::iterator it=idToIPmap.begin(); it!=idToIPmap.end(); it++)
        deleteIpFromId(it->first ,ip);
}

void UdpNetObjectSeekerWorker::deleteIpFromId(int id, std::string ip)
{
    std::map<int, ipvector>::iterator it = idToIPmap.find(id);
    ipvector vec;
    if(it==idToIPmap.end())
        return;
    vec = idToIPmap[id];
    for(ipvector::iterator it=vec.begin(); it!=vec.end(); it++)
        if(*it==ip){
            vec.erase(it);
            idToIPmap[id] = vec;
            return;
        }
}

void UdpNetObjectSeekerWorker::sendShortMessage(std::string data)
{
    short int id = myId;
    char temp[2];
    memcpy(temp, &id, 2);
    std::string str;
    str.push_back(temp[0]);
    str.push_back(temp[1]);
    udpShortSender->sendData(str + data);
}
