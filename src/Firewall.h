#pragma once
#include <string>
#include <vector>
#include "Config.h"

class Firewall 
{
public:
    Firewall()=default;
    explicit Firewall(const std::vector<BlockRule>& blockrules);
    bool isBlocked(const std::string& ipaddr) const;
    void setRules(const std::vector<BlockRule>& blockrules);

private:
    std::vector<BlockRule> blockrules_;
};