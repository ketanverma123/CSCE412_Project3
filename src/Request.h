/**
 * @file Request.h
 * @brief Defines the request structure used in the load balancer queue.
 */
#pragma once
#include <string>
/**
 * @struct LoadRequest
 * @brief Represents a single web request in the simulation.
 *
 * Requests are generated randomly and stored in the load balancer queue until assigned to a server.
 */
struct LoadRequest 
{
    std::string source_ip;       
    std::string destination_ip;      
    int service_time;     
    char request_type;           
    int arrival_time;       
};