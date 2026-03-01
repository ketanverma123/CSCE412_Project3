#pragma once
#include <string>

struct LoadRequest 
{
    std::string source_ip;       
    std::string destination_ip;      
    int service_time;     
    char request_type;           
    int arrival_time;       
};