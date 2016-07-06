#include <ctype.h>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <vector>
#include <memory>
#include <iostream>
#include <cstdio>
#include <getopt.h>
#include <unistd.h>
#include <mosquittopp.h>
#include <fstream>
#include "jsoncpp/json/json.h"
#include <wbmqtt/utils.h>
#include <wbmqtt/mqtt_wrapper.h>
#include "config_handler.h"
#include "Exception.h"
#include "exec.h"

using namespace std;

void LoadConfig(const string& file_name, THandlerConfig& config)
{
        ifstream config_file (file_name);
        if (config_file.fail())
                return; // just use defaults

        Json::Value root;
        Json::Reader reader;
        bool parsedSuccess = reader.parse(config_file, root, false);

        // Report failures and their locations in the document.
        if(not parsedSuccess)
                throw Exception("Failed to parse config JSON: " + reader.getFormatedErrorMessages());
        if (!root.isObject())
                throw Exception("Bad config file (the root is not an object)");
        if (root.isMember("rack_name"))
                config.rackName = root["rack_name"].asString();
        if (root.isMember("debug"))
                config.debug = root["debug"].asBool();
        if (root.isMember("timeout"))
                config.timeout = root["timeout"].asInt();

        const auto& nodes = root["channels"];

        for (unsigned int index = 0; index < nodes.size(); index++){
                const auto& item = nodes[index];

        
        TIpmiNode node;
      
        if (item.isMember("id")) {
                        node.id = item["id"].asString();
                }
        else
                throw Exception("Missing id for a node");

        if (item.isMember("addess")) {
                        node.address = item["addess"].asString();
                }
        else
                throw Exception("Missing IP address for a node");
        
        if (item.isMember("port")) {
                        node.port = item["port"].asString();
                }
        else
                throw Exception("Missing TCP port for a node");
        
        if (item.isMember("login")) {
                        node.login = item["login"].asString();
                }
        else
                throw Exception("Missing login for a node");
        
        if (item.isMember("password")) {
                        node.password = item["password"].asString();
                }
        else
                throw Exception("Missing password for a node");

        config.IpmiNodes.push_back(node);
        }
        
}


int main(int argc, char *argv[])
{
    TMQTTClient::TConfig mqtt_config;
    mqtt_config.Host = "localhost";
    mqtt_config.Port = 1883;
    string config_fname;
    bool debug = false;

    int c;
    //~ int digit_optind = 0;
    //~ int aopt = 0, bopt = 0;
    //~ char *copt = 0, *dopt = 0;
    while ( (c = getopt(argc, argv, "dsc:h:p:")) != -1) {
        //~ int this_option_optind = optind ? optind : 1;
        switch (c) {
        case 'd':
            debug = true;
            break;
        case 'c':
            config_fname = optarg;
            break;
        case 'p':
//            mqtt_config.Port = stoi(optarg);
            break;
        case 'h':
//            mqtt_config.Host = optarg;
            break;
        case '?':
            break;
        default:
            printf ("?? getopt returned character code 0%o ??\n", c);
        }
    }
     THandlerConfig config;
     if (!config_fname.empty())
                        LoadConfig(config_fname, config);

    
//    cout << config.rackName;

     mosqpp::lib_init();
     if (mqtt_config.Id.empty()) mqtt_config.Id = "wb-ipmi";
     PMQTTClient mqtt_client(new TMQTTClient(mqtt_config));
     mqtt_client->Connect();
     mqtt_client->StartLoop();

if(debug) cout <<"MQTT Connect..." <<endl;

     for(auto const& value: config.IpmiNodes) {

         string command = "/usr/sbin/ipmi-sensors -D LAN2_0 -h "+value.address+" -u "+value.login+" -p '"+value.password+"' -l USER -W discretereading --no-header-output --quiet-cache --sdr-cache-recreate --comma-separated-output --entity-sensor-names 2>&1";
         string queue = "/devices/"+value.id+"/controls";
         string out = exec(command.c_str());
         std::stringstream ss;
         std::string to;
         if (out.size() >0 )
         {
             ss  << out;

             while(std::getline(ss,to,'\n')){
                 if(isdigit(to.at(0)))
                 {
                     vector<string> val  = split(to,',');
                     std::replace(val[1].begin(), val[1].end(),' ','_');
                     std::replace(val[1].begin(), val[1].end(),'.','_');
                     if(isdigit(val[3].at(0)))
                     { if(debug) {
                             cout << value.id <<':'<<val[1] <<':'<< "range" <<'\n';
                         }
                         mqtt_client->Publish(NULL,queue+"/"+val[1]+"/meta/type","range",0,true);
                     }
                 }

             }
         }
     }


while(1)
{




for(auto const& value: config.IpmiNodes) {

    string command = "/usr/sbin/ipmi-sensors -D LAN2_0 -h "+value.address+" -u "+value.login+" -p '"+value.password+"' -l USER -W discretereading --no-header-output --quiet-cache --sdr-cache-recreate --comma-separated-output --entity-sensor-names 2>&1";
    string queue = "/devices/"+value.id+"/controls";
    string out = exec(command.c_str());
    std::stringstream ss;
    std::string to;
    if (out.size() >0 )
    {
        ss  << out;

        while(std::getline(ss,to,'\n')){
            if(isdigit(to.at(0)))
            {
                vector<string> val  = split(to,',');
                std::replace(val[1].begin(), val[1].end(),' ','_');
                std::replace(val[1].begin(), val[1].end(),'.','_');
                if(isdigit(val[3].at(0)))
                { if(debug) {
                        cout << value.id <<':'<<val[1] <<':'<<val[3]<<'\n';
                    }
                    mqtt_client->Publish(NULL,queue+"/"+val[1],val[3],0,false);
                }
            }

        }
    }
}

if(debug) cout <<"Timeout "<<config.timeout << "s..." <<endl;
sleep(config.timeout);
}

mqtt_client->StopLoop();
mosqpp::lib_cleanup();

    //for (;;)

return 1;

}
