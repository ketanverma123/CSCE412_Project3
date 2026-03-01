#include "WebServer.h"

WebServer::WebServer()
    : is_busy_(false), remaining_time_(0), completed_this_cycle_(false), current_request_(std::nullopt) {}

bool WebServer::isBusy() const { return is_busy_; }

bool WebServer::canBeRemovedSafely() const 
{
    return !is_busy_;
}

void WebServer::assign(const LoadRequest& r) 
{
    current_request_=r;
    is_busy_=true;
    remaining_time_=r.service_time;
    completed_this_cycle_=false;
}

void WebServer::tick() 
{
    completed_this_cycle_=false;
    if (!is_busy_) return;
    if (remaining_time_>0) remaining_time_--;
    if (remaining_time_<=0) 
    {
        is_busy_=false;
        remaining_time_=0;
        completed_this_cycle_=true;
    }
}

bool WebServer::justFinished() const 
{
    return completed_this_cycle_;
}

std::optional<LoadRequest> WebServer::current() const 
{ 
    return current_request_; 
}

int WebServer::remainingTime() const 
{ 
    return remaining_time_; 
}