#ifndef UDPNETOBJECTSEEKER_H
#define UDPNETOBJECTSEEKER_H

#include <vector>
#include <string>
#include <map>


class UdpNetObjectSeekerWorker;

class UdpNetObjectSeekersProcessor
{
public:
    virtual void OnSeekMessage(std::string ip, int id) = 0;
    virtual void OnShortMessage(int id, std::string ip, std::string message) = 0;
    virtual void OnShortMessage(std::string ip, std::string message) = 0;
};

typedef std::vector<std::string> ipvector;

class UdpNetObjectSeeker
{
public:
    UdpNetObjectSeeker(int myId_, int secondstoseek_, bool is_multicast_or_filename, std::string multicastaddress_or_filename);
    ~UdpNetObjectSeeker();

    ipvector getIpvectorById(int id);
    void deleteIpFromAllIds(std::string ip);
    void deleteIpFromId(int id, std::string ip);

    void SubscribeOnEvents(UdpNetObjectSeekersProcessor* pProcessor = NULL);
    void DescribeOnEvents(UdpNetObjectSeekersProcessor* pProcessor = NULL);

    void sendShortMessage(std::string data);

private:
    UdpNetObjectSeekerWorker   * unosw;
};

#endif // UDPNETOBJECTSEEKER_H
