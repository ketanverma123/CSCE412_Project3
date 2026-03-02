/**
 * @file Config.cpp
 * @brief Implements configuration parsing utilities and ConfigLoader.
 */
#include "Config.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

static inline std::string trim(const std::string& str)
{
    size_t begin=0,end=str.size();
    while
    (
        begin<end&&std::isspace(static_cast<unsigned char>(str[begin]))
    )
    begin++;
    while
    (
        end>begin&&std::isspace(static_cast<unsigned char>(str[end-1]))
    )
    end--;
    return str.substr(begin,end-begin);
}

static inline std::string toLower(std::string str)
{
    std::transform(str.begin(),str.end(),str.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});
    return str;
}

static inline bool parseBool(const std::string& val,bool fallback)
{
    std::string normal=toLower(trim(val));
    if
    (
        normal=="true"||normal=="1"||normal=="yes"||normal=="on"
    ) return true;
    if
    (
        normal=="false"||normal=="0"||normal=="no"||normal=="off"
    ) return false;
    return fallback;
}

static inline std::vector<std::string> splitCSV(const std::string& str)
{
    std::vector<std::string> out;
    std::stringstream csvstr(str);
    std::string item;
    while(std::getline(csvstr,item,','))
    {
        item=trim(item);
        if(!item.empty()) out.push_back(item);
    }
    return out;
}

bool ConfigLoader::loadFromFile(const std::string& path,Config& config)
{
    std::ifstream in(path);
    if(!in.is_open()) return false;
    std::string line;
    while(std::getline(in,line))
    {
        line=trim(line);
        if(line.empty()||line[0]=='#') continue;
        auto pos=line.find('=');
        if(pos==std::string::npos) continue;
        std::string key=trim(line.substr(0,pos));
        std::string val=trim(line.substr(pos+1));
        std::string lowkey=toLower(key);
        try{
            if(lowkey=="initial_servers") config.initial_servers=std::stoi(val);
            else if(lowkey=="simulation_cycles") config.simulation_cycles=std::stoi(val);
            else if(lowkey=="initial_queue_multiplier") config.initial_queue_multiplier=std::stoi(val);
            else if(lowkey=="processing_time_min") config.processing_time_min=std::stoi(val);
            else if(lowkey=="processing_time_max") config.processing_time_max=std::stoi(val);
            else if(lowkey=="request_arrival_probability") config.request_arrival_probability=std::stod(val);
            else if(lowkey=="requests_per_arrival_min") config.requests_per_arrival_min=std::stoi(val);
            else if(lowkey=="requests_per_arrival_max") config.requests_per_arrival_max=std::stoi(val);
            else if(lowkey=="queue_lower_threshold") config.queue_lower_threshold=std::stoi(val);
            else if(lowkey=="queue_upper_threshold") config.queue_upper_threshold=std::stoi(val);
            else if(lowkey=="scale_cooldown_cycles") config.scale_cooldown_cycles=std::stoi(val);
            else if(lowkey=="enable_color_output") config.enable_color_output=parseBool(val,config.enable_color_output);
            else if(lowkey=="log_interval") config.log_interval=std::stoi(val);
            else if(lowkey=="log_dir") config.log_dir=val;
            else if(lowkey=="log_file") config.log_file=val;
            else if(lowkey=="blocked_ranges"||lowkey=="blocked_prefixes")
            {
                config.blocked_prefixes.clear();
                for(const auto& pref:splitCSV(val)) config.blocked_prefixes.push_back({pref});
            }
        }catch(...){
        }
    }
    if(config.processing_time_min<1) config.processing_time_min=1;
    if(config.processing_time_max<config.processing_time_min) config.processing_time_max=config.processing_time_min;
    if(config.requests_per_arrival_min<0) config.requests_per_arrival_min=0;
    if(config.requests_per_arrival_max<config.requests_per_arrival_min) config.requests_per_arrival_max=config.requests_per_arrival_min;
    if(config.initial_servers<1) config.initial_servers=1;
    if(config.simulation_cycles<1) config.simulation_cycles=1;
    if(config.scale_cooldown_cycles<0) config.scale_cooldown_cycles=0;
    if(config.queue_lower_threshold<1) config.queue_lower_threshold=1;
    if(config.queue_upper_threshold<config.queue_lower_threshold) config.queue_upper_threshold=config.queue_lower_threshold;
    return true;
}