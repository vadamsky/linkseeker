#include "udpnetobjectseeker.h"
#include "udpnetobjectseekerworker.h"

#include "udpsender.h"
#include "udpreceiver.h"
#include "loaderfile.h"

UdpNetObjectSeeker::UdpNetObjectSeeker(int myId_, int secondstoseek_, bool is_multicast_or_filename, std::string multicastaddress_or_filename)
{
	try{
		unosw = new UdpNetObjectSeekerWorker(myId_, secondstoseek_, is_multicast_or_filename, multicastaddress_or_filename);
	}
	catch(...)
	{
        std::cout << "UdpNetObjectSeeker: UdpNetObjectSeekerWorker constructor error :\n";
        std::cout << "    myId_="<<myId_<<" secondstoseek_="<<secondstoseek_<<" is_multicast_or_filename="<<is_multicast_or_filename;
        std::cout << " multicastaddress_or_filename="<<multicastaddress_or_filename<<"\n";
        unosw = 0;
	}
}

UdpNetObjectSeeker::~UdpNetObjectSeeker()
{
    try
    {
        if(unosw) delete unosw;
    }
    catch(...)
    {
        std::cout << "UdpNetObjectSeeker: UdpNetObjectSeekerWorker destructor error \n";
    }
}

void UdpNetObjectSeeker::SubscribeOnEvents(UdpNetObjectSeekersProcessor* pProcessor)
{
    if(unosw) unosw->SubscribeOnEvents(pProcessor);
}

void UdpNetObjectSeeker::DescribeOnEvents(UdpNetObjectSeekersProcessor* pProcessor)
{
    if(unosw) unosw->DescribeOnEvents(pProcessor);
}


ipvector UdpNetObjectSeeker::getIpvectorById(int id)
{
    ipvector vec;
    if(unosw) vec = unosw->getIpvectorById(id);
    return vec;
}

void UdpNetObjectSeeker::deleteIpFromAllIds(std::string ip)
{
    if(unosw) unosw->deleteIpFromAllIds(ip);
}

void UdpNetObjectSeeker::deleteIpFromId(int id, std::string ip)
{
    if(unosw) unosw->deleteIpFromId(id, ip);
}

void UdpNetObjectSeeker::sendShortMessage(std::string data)
{
    if(unosw) unosw->sendShortMessage(data);
}
