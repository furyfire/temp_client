/* 
 * File:   main.cpp
 * Author: JCT
 * Created on 06 March 2018, 11:41
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <signal.h>
#include "TCPClient.h"
#include "jsonrpc.h"
using namespace std;

TCPClient tcp;

void sig_exit(int s)
{
    tcp.Send("HEAT OFF\n");
    tcp.exit();
    exit(0);
}

int main(int argc, char** argv) {
    cout << "Temperature client for the BBB" << endl;
    
    if(argc != 4) {
        cout << "Please input a hostname, port and requested temperature" << endl;
        return EXIT_FAILURE;
    }
    
    char *servername = argv[1];
    int port = atof(argv[2]);
    float settemperature = atof(argv[3]);
    
    cout << "Set temperature: " << settemperature << endl;
    
    if(!tcp.setup(servername,port)) {
        cout << "Connecting failed" << endl;
        return EXIT_FAILURE;
    }
    signal(SIGINT, sig_exit);
    string status = tcp.receive();
    cout << "Got! " << status << endl;
    
    while(1)
    {
        //Get temperature frequently.
        tcp.Send("GET TEMP\n");
        
        //Receive the string containing the temperature
        string rec = tcp.receive();
        if( rec != "" )
        {
                cout << "Server Response: " << rec;
                std::stringstream ss(rec);
                string reply_command, reply_command2;
                float reply_temp;
                
                //Save each element of the incomming string in a seperate
                //string. Seperated by spaces.
                ss >> reply_command >> reply_command2 >> reply_temp;
                cout << "Parsed: " << reply_command << " " << reply_command2 
                     << " Value: " << reply_temp << endl;
                
                //If the client received a temperature
                if(reply_command == "REPLY" && reply_command2 == "TEMP:") {
                    
                    //turn heat on, when temperature is lower than setpoint
                    if(reply_temp < settemperature) {
                        tcp.Send("HEAT ON\n");
                        cout << "Sent: HEAT ON" << endl;
                    } else {
                        tcp.Send("HEAT OFF\n");
                        cout << "Sent: HEAT OFF" << endl;
                    }
                }
        }
        sleep(2);
    }
    return 0;
}

