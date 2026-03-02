/**
 * @file WebServer.h
 * @brief Simulated web server that processes requests over time.
 */
#pragma once
#include <optional>
#include "Request.h"
/**
 * @class WebServer
 * @brief Represents a single server that can process one request at a time.
 *
 * A WebServer maintains an internal countdown . Each tick decrements
 * the timer until the assigned request completes.
 */
class WebServer 
{
public:
    WebServer();
    bool isBusy() const;
    bool canBeRemovedSafely() const; 
    void assign(const LoadRequest& r);
    void tick(); 
    bool justFinished() const; 
    std::optional<LoadRequest> current() const;
    int remainingTime() const;

private:
    bool is_busy_;
    int remaining_time_;
    bool completed_this_cycle_;
    std::optional<LoadRequest> current_request_;
};