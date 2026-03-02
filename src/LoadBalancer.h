/**
 * @file LoadBalancer.h
 * @brief Core simulation of a load balancer with dynamic server scaling.
 */
#pragma once
#include <queue>
#include <random>
#include <vector>
#include "Config.h"
#include "Firewall.h"
#include "Logger.h"
#include "Request.h"
#include "WebServer.h"
/**
 * @class LoadBalancer
 * @brief Simulates a company load balancer handling web requests.
 *
 * Responsibilities:
 * - Maintains a request queue
 * - Assigns requests to idle servers
 * - Generates random arrivals
 * - Dynamically scales server pool based on queue thresholds
 * - Applies firewall filtering on request source IP
 * - Logs events and prints a final summary for rubric requirements
 */
class LoadBalancer
{
public:
    explicit LoadBalancer(const Config& cfg);
    void run();

private:
    Config config_;
    Firewall firewall_;
    Logger logger_;
    std::queue<LoadRequest> request_queue_;
    std::vector<WebServer> server_pool_;

    std::mt19937 rng_;
    std::uniform_int_distribution<int> byte_dist_;
    std::uniform_int_distribution<int> service_time_dist_;
    std::uniform_real_distribution<double> prob_dist_;
    std::uniform_int_distribution<int> batch_size_dist_;
    std::uniform_int_distribution<int> type_dist_;

    int scale_wait_=0;
    int cycle_=0;

    long long initial_generated_count_=0;
    long long runtime_generated_count_=0;
    long long completed_count_=0;
    long long firewall_rejected_count_=0;

    long long scale_up_count_=0;
    long long scale_down_count_=0;

    int min_active_servers_=0;
    int max_active_servers_=0;

    long long peak_queue_size_=0;
    long long queue_size_sum_=0;

    void ensureLogDir();
    void writeHeader();
    void writeFinalSummary(int starting_queue);

    std::string randomIP();
    char randomJobType();
    int randomTaskTime();
    LoadRequest makeRandomRequest(int created_time);

    void seedInitialQueue(int count);
    void maybeGenerateArrivals();
    void assignToIdleServers();
    void tickServers();
    void maybeScaleServers();

    int activeServerCount() const;
    int busyServerCount() const;
    int idleServerCount() const;

    bool tryRemoveOneServer();
    void addOneServer();
};