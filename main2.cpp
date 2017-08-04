#include "udpnetobjectseeker.h"
#include <unistd.h>

using namespace std;
SECONDS_BETWEEN_SENDS = 10;
SECONDS_OF_WORK = 200;
MESSAGE = "message data 2";

// Creates UdpNetObjectSeeker object that works 200 seconds and 
// sends each 10 seconds message to all devices, that hear it
int main()
{
    int myId = 2, 
    int secondsBeetweenSeeks = 10;
    bool isMulticastOrFilename = false;
    std::string filename = "ips_new2.txt";
    UdpNetObjectSeeker unos(myId, secondsBeetweenSeeks, isMulticastOrFilename, filename);
    int i = 0;
    while(i < SECONDS_OF_WORK){
        if(i % SECONDS_BETWEEN_SENDS == 0) unos.sendShortMessage(MESSAGE);
        sleep(1);
        i++;
    }
    return 0;
}
