#include "udpnetobjectseeker.h"
#include <unistd.h>

using namespace std;

int main()
{
    UdpNetObjectSeeker unos(1, 10, false, "ips_new1.txt");
    int i=0;
    while(i<200){
        if(i%10==0) unos.sendShortMessage("message data 1");
        sleep(1);
        i++;
    }
    return 0;
}

