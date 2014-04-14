//DGR_framework.cpp
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "DGR_framework.h"

int framesPassed = 0;

int slave_listen_port = SLAVE_LISTEN_PORT;

std::map<std::string,MapNodePtr *> InputMap;
int s;
int milliseconds;
struct timespec req;

struct sockaddr_in si_me, si_other;
socklen_t slen;
bool receivedPacket;
pthread_t senderThread, receiverThread;
char * packet_buffer;

void error(const char *msg) 
{
    perror(msg);
    exit(1);
}


void * sender(void * data) {

    int packet_length;
    unsigned char packet_counter;
    //current node properties
    char * node_buf;
    int node_length, node_counter;
    bool first_node, last_node;
    packet_buffer = new char[BUFLEN];

    while (true) 
    {
        
        //packet_buffer properties
        int packet_length = 0;
        unsigned char packet_counter = 0;
        //current node properties
        char * node_buf;
        int node_length = 0;
        int node_counter = 0;

        bool first_node = true;
        bool last_node = false;


        for(std::map<std::string,MapNodePtr *>::iterator it = InputMap.begin();it!= InputMap.end();++it)
        {

            node_length = 0;
            //if(std::next(it) == InputMap.end()){ last_node = true;}

            MapNodePtr * cur_node = it->second;
            std::string cur_name = cur_node->name;
            char* cur_data = cur_node->getDataString();
            int cur_data_length = cur_node->dataLength;

            node_length = cur_name.length() + sizeof(char) + cur_data_length;

            // if(first_node || last_node) node_length += sizeof(packet_counter);
            node_buf = new char[node_length];

            //prepare node buffer
            memcpy(node_buf, cur_name.c_str(), cur_name.length());
            node_buf[cur_name.length()] = '\0';
            memcpy(node_buf + cur_name.length() + 1, cur_data, cur_data_length);

            //message buffer full. send and start a new one.
            if( (packet_length + node_length) > BUFLEN)
            {
                if (sendto(s, packet_buffer, packet_length, 0, (struct sockaddr*)&si_other,slen) == -1) 
                    error ("ERROR sendto()");
                packet_counter ++;
                packet_length = 0;
                first_node = true;
                it--;

            }
            else
            { // add node to packet buffer
                memcpy(packet_buffer + packet_length, node_buf, node_length);
                packet_length += node_length;
                first_node = false;
                node_counter ++;
            }

        }
        if (sendto(s, packet_buffer, packet_length, 0, (struct sockaddr*)&si_other,slen) == -1) 
            error ("ERROR sendto()");

        usleep(32000);
    }
}

// The SLAVES receive state data from teh RELAY via UDP packets and parse the data
    
void * receiver(void * data){

    packet_buffer = new char[BUFLEN];

    MapNodePtr * cur_node;
    std::string node_name;
    int node_data_length, packet_cursor;


    while (true){
        if (recvfrom(s, packet_buffer, BUFLEN, 0, (struct sockaddr *)&si_other, &slen) == -1) 
            error("ERROR recvfrom()");
        receivedPacket = true;
    	framesPassed = 0;
        packet_cursor = 0;

        while( (packet_buffer[packet_cursor] > 31) && (packet_cursor < BUFLEN) )
        {

                        //extract name
            node_name = "";
            for(int i = packet_cursor; i < BUFLEN; i++) 
            {
                if(packet_buffer[i]) 
                { 
                    node_name.push_back(packet_buffer[i]); 
                    packet_cursor++; 
                }
                else 
                { 
                    packet_cursor++; 
                    break; 
                }
            }
                        //get current node and data length
            cur_node = InputMap.at(node_name);
            node_data_length = cur_node->dataLength;

            char * node_data = new char[node_data_length];
            node_data = &packet_buffer[packet_cursor];
                        //set data
            cur_node->setData(node_data);
            packet_cursor += node_data_length;
                        //print data        
        }
    }
}


//constructors

DGR_framework::DGR_framework(char* r_IP){
   	
	char *RELAY_IP = NULL;
	int so_broadcast = 1;
    RELAY_IP=r_IP;
    InpMap = &InputMap;

    // socket to send data to relay
    slen = sizeof(si_other);
    so_broadcast = 1;

    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) 
        error("ERROR socket");
    printf("--%d--\n",s);
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(RELAY_LISTEN_PORT);

    if (inet_aton(RELAY_IP, &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    if (pthread_create(&senderThread, NULL, &sender, NULL) != 0) 
    {
        perror("Can't start thread, terminating\n");
        exit(1);
    }


}
DGR_framework::DGR_framework(int s_listen_port){
    slave_listen_port = s_listen_port;
    slaveInit();

}

DGR_framework::DGR_framework(){
    slave_listen_port = SLAVE_LISTEN_PORT;
    slaveInit();
}



DGR_framework::~DGR_framework(){
    close(s);
}

void DGR_framework::slaveInit(){
    receivedPacket = false;
    InpMap = &InputMap;
    recvPack = &receivedPacket;

      // Socket to read data from relay

    slen=sizeof(si_other);
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) 
        error("ERROR socket");
    printf("--%i--%d--\n",slave_listen_port,s);
    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(slave_listen_port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1) 
        error("ERROR bind");

      // listen for updates
    
    if (pthread_create(&receiverThread, NULL, &receiver, NULL) != 0) 
    {
        perror("Can't start thread, terminating");
        exit(1);
    }
}
// void DGR_framework::exitCallback(void) 
// {
// 	printf("closing socket\n");
// 	    close(s);
// 	printf("socket closed\n");
	
// }


