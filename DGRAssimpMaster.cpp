#include <GL/glut.h>

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <math.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "ViconClient.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>

std::string hostname = "c07-0510-01.ad.mtu.edu";//"141.219.28.17:801";//was 141.219.28.107:801
ViconDataStreamSDK::CPP::Client MyClient;

using namespace ViconDataStreamSDK::CPP;
namespace
{
  std::string Adapt( const bool i_Value )
  {
    return i_Value ? "True" : "False";
  }

  std::string Adapt( const Direction::Enum i_Direction )
  {
    switch( i_Direction )
    {
      case Direction::Forward:
        return "Forward";
      case Direction::Backward:
        return "Backward";
      case Direction::Left:
        return "Left";
      case Direction::Right:
        return "Right";
      case Direction::Up:
        return "Up";
      case Direction::Down:
        return "Down";
      default:
        return "Unknown";
    }
  }
}

#define SEND_IP "255.255.255.255"  // broadcast address
#define BUFLEN 512
#define PORT 25884
/////////////////////////////////////////////////
/////// VICON ///////////////////////////////////
/////////////////////////////////////////////////
int viconInit()
{
    // Connect to a server
    std::cout << "Connecting to " << hostname.c_str() << " ..." << std::flush;
  int attemptConnectCount = 0;
  const int MAX_CONNECT_ATTEMPTS=2;
    while( !MyClient.IsConnected().Connected && attemptConnectCount < MAX_CONNECT_ATTEMPTS)
    {
    attemptConnectCount++;
    bool ok = false;
    ok =( MyClient.Connect( hostname ).Result == Result::Success );
    if(!ok)
      std::cout << "Warning - connect failed..." << std::endl;
    std::cout << ".";
    sleep(1);
    }
  if(attemptConnectCount == MAX_CONNECT_ATTEMPTS)
  {
    printf("Giving up making connection to Vicon system\n");
    return 1;
  }
    std::cout << std::endl;
/* TODO: bjnix at mtu dot edu | 11.13.2013
  add enumeration for input, so as to let user input what types of data to enable
*/
    // Enable some different data types
    MyClient.EnableSegmentData();
    //MyClient.EnableMarkerData();
    //MyClient.EnableUnlabeledMarkerData();
    //MyClient.EnableDeviceData();

    std::cout << "Segment Data Enabled: "          << Adapt( MyClient.IsSegmentDataEnabled().Enabled )         << std::endl;
    std::cout << "Marker Data Enabled: "           << Adapt( MyClient.IsMarkerDataEnabled().Enabled )          << std::endl;
    std::cout << "Unlabeled Marker Data Enabled: " << Adapt( MyClient.IsUnlabeledMarkerDataEnabled().Enabled ) << std::endl;
    std::cout << "Device Data Enabled: "           << Adapt( MyClient.IsDeviceDataEnabled().Enabled )          << std::endl;

    // Set the streaming mode
    //MyClient.SetStreamMode( ViconDataStreamSDK::CPP::StreamMode::ClientPull );
    // MyClient.SetStreamMode( ViconDataStreamSDK::CPP::StreamMode::ClientPullPreFetch );
    MyClient.SetStreamMode( ViconDataStreamSDK::CPP::StreamMode::ServerPush );

    // Set the global up axis
    MyClient.SetAxisMapping( Direction::Forward,
                             Direction::Up,
                             Direction::Right);
    //MyClient.SetAxisMapping( Direction::Forward, 
    //                         Direction::Left, 
    //                         Direction::Up ); // Z-up
    // MyClient.SetGlobalUpAxis( Direction::Forward, 
    //                           Direction::Up, 
    //                           Direction::Right ); // Y-up

    Output_GetAxisMapping _Output_GetAxisMapping = MyClient.GetAxisMapping();
    std::cout << "Axis Mapping: X-" << Adapt( _Output_GetAxisMapping.XAxis ) 
        << " Y-" << Adapt( _Output_GetAxisMapping.YAxis ) 
        << " Z-" << Adapt( _Output_GetAxisMapping.ZAxis ) << std::endl;

    // Discover the version number
    Output_GetVersion _Output_GetVersion = MyClient.GetVersion();
    std::cout << "Version: " << _Output_GetVersion.Major << "." 
        << _Output_GetVersion.Minor << "." 
        << _Output_GetVersion.Point << std::endl;
  return 0;
}
void viconExit()
{
    MyClient.DisableSegmentData();
//    MyClient.DisableMarkerData();
//    MyClient.DisableUnlabeledMarkerData();
//    MyClient.DisableDeviceData();

  // TODO: Disconnect seems to cause a hang. -Scott Kuhl
    // Disconnect and dispose
    int t = clock();
    std::cout << " Disconnecting..." << std::endl;
    MyClient.Disconnect();
    int dt = clock() - t;
    double secs = (double) (dt)/(double)CLOCKS_PER_SEC;
    std::cout << " Disconnect time = " << secs << " secs" << std::endl;
}

/////////////////////////////////////////////////
/////// END VICON ///////////////////////////////
/////////////////////////////////////////////////

// DGR variables
struct sockaddr_in si_other;
int slen, s;
char buf [BUFLEN];
int so_broadcast = 1;

using namespace std;

const GLdouble SCREEN_WIDTH = (1920*6)/8.0;  
const GLdouble SCREEN_HEIGHT = (1080.0*4)/8.0;
const float screenAspectRatio = SCREEN_WIDTH/SCREEN_HEIGHT;

float rotation = 0.0f;

void error(const char *msg) {
  perror(msg);
  exit(1);
}

void closeProgram() {
  exit(0);
}
float distanceTraveled(vector<float> start, vector<float> end){
  return sqrt(pow((end[0] - start[0]),2) +pow((end[1] - start[1]),2));
}

int main(int argc, char** argv) {

  slen=sizeof(si_other);
  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) error("ERROR socket");
  setsockopt(s, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));
  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);
  if (inet_aton(SEND_IP, &si_other.sin_addr) == 0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }

  

  FILE * pFile;
  pFile = fopen ("testResults.txt","a+");
  
  viconInit();
  atexit(viconExit);
  
for(int i = 0; i<5; i++){
    char * foo;
    std::cout << "Press Enter to Continue";
    std::cin.ignore();
    
    if (pFile!=NULL)
    {
      std::cout<<"putting!"<<std::endl;
      fputs ("fopen example\n",pFile);
    }
  }
std::cout<<"done!"<<std::endl;
fclose (pFile);

/*
  while (true) {
    //rotation += .0001;
    //printf("rot: %f\n",rotation);
    sprintf(buf, "%f", rotation);
    if (sendto(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other, slen) == -1) error("ERROR sendto()");
    usleep(4000);
  }
*/
  return 0;
}
