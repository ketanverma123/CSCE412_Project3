/**
 * @file main.cpp
 * @brief Driver program for Project 3 Load Balancer simulation.
 */
#include <iostream>
#include <string>
#include "Config.h"
#include "LoadBalancer.h"

static int readIntOrDefault(const std::string& prompt, int def) 
{
    std::cout << prompt << " (default " << def << "): ";
    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) return def;
    try 
    { 
        return std::stoi(line); 
    } catch (...) { return def; }
}
/**
 * @brief Program entry point.
 *
 * Reads user inputs and/or loads config, then runs the simulation and produces logs.
 * @return Exit code.
 */
int main() 
{
    Config cfg;
    ConfigLoader::loadFromFile("config.txt", cfg);
    std::cout << "=== Project 3: Load Balancer Simulation ===\n";
    std::cout << "Press Enter to accept defaults.\n\n";
    cfg.initial_servers = readIntOrDefault("Enter starting number of servers", cfg.initial_servers);
    cfg.simulation_cycles = readIntOrDefault("Enter number of clock cycles to run", cfg.simulation_cycles);
    cfg.log_dir = "logs";
    cfg.log_file = cfg.log_dir + std::string("/run_") + std::to_string(cfg.initial_servers) + "servers_" + std::to_string(cfg.simulation_cycles) + "cycles.txt";
    LoadBalancer lb(cfg);
    lb.run();
    std::cout << "\nLog written to: " << cfg.log_file << "\n";
    return 0;
}