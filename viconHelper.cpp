#include "viconHelper.h"

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

void viconTranslation(std::map<std::string,std::vector<float>>  trackingData){
    
    #ifdef DGR_MASTER

    std::string name;

    while(MyClient.GetFrame().Result != Result::Success) 
    {
        sleep(1);
        std::cout << ".";
    }   
    std::cout<<std::flush;


    for(int i = 0; i < MyClient.GetSubjectCount(); i++)
    {
        std::string name = string(MyClient.GetSubjectName(i).SubjectName);
        Output_GetSegmentGlobalTranslation Output = MyClient.GetSegmentGlobalTranslation(name,name);
        //std::cout<<names[i]<<": "<<Output.Translation << std::endl;
        if(!Output.Occluded)
            { 
            //std::cout<<names[i]<<" NOT occluded!"<< std::endl;
            /*std::cout<<names[i]<<": ("<<Output.Translation[0]<<", "
                          <<Output.Translation[1]<<", "
                          <<Output.Translation[2]<<") " 
                          <<Output.Occluded << std::endl;*/
            try {
                    trackingData.at(name) = std::vector(Output.Translation[0],Output.Translation[1],Output.Translation[2]);     
                }
            catch (const std::out_of_range& oor) {
                trackingData.insert(std::pair<std::string,std::tuple<float,float,float>> (
                    name,
                    std::std::vector(Output.Translation[0],Output.Translation[1],Output.Translation[2]) ) 
                );
                std::cout << "added element " << name << std::endl;
            }
        }
        else
        { 
            //std::cout<<names[i]<<" IS occluded!"<< std::endl;
            temp[i] = p1->getPosition()[i];
        }
    }

    
    p1->setPosition(temp);

    #endif
    p1->updateBody();
    return 0;
}