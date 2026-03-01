#pragma once
#include <optional>
#include "Request.h"

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