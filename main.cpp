/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: JCT
 *
 * Created on 06 March 2018, 11:41
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include "TCPClient.h"

using namespace std;

TCPClient tcp;

void sig_exit(int s)
{
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
    
    string status = tcp.receive();
    cout << "Got! " << status << endl;
    
    while(1)
    {
        srand(time(NULL));
        tcp.Send("GET TEMP\n");
        string rec = tcp.receive();
        if( rec != "" )
        {
                cout << "Server Response: " << rec << endl;
                std::stringstream ss(rec);
                string reply_command, reply_command2;
                float reply_temp;
                ss >> reply_command >> reply_command2 >> reply_temp;
                cout << "Parsed: " << reply_command << " " << reply_command2 << " Value: " << reply_temp << endl;
                if(reply_command == "REPLY" && reply_command2 == "TEMP") {
                    if(reply_temp < settemperature) {
                        tcp.Send("SET HEATER ON\n");
                    } else {
                        tcp.Send("SET HEATER OFF\n");
                    }
                }
                tcp.receive();
        }
        sleep(5);
    }
    return 0;
}

