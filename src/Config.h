#pragma once
#include <string>
#include <vector>

struct BlockRule 
{
    std::string prefix; 
};

struct Config 
{
    int initial_servers=10;
    int simulation_cycles=10000;
    int initial_queue_multiplier=100; 
    int processing_time_min=5;
    int processing_time_max=40;
    double request_arrival_probability=0.30; 
    int requests_per_arrival_min=1;
    int requests_per_arrival_max=2;
    int queue_lower_threshold=20; 
    int queue_upper_threshold=80; 
    int scale_cooldown_cycles=25;
    bool enable_color_output=true;
    int log_interval=100; 
    std::string log_dir="logs";
    std::string log_file="logs/run_log.txt";
    std::vector<BlockRule> blocked_prefixes;
};

class ConfigLoader 
{
public:
    static bool loadFromFile(const std::string& path, Config& out);
};