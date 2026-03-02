/**
 * @file Firewall.h
 * @brief IP prefix based blocking mechanism (simple firewall) for requests.
 */
#pragma once
#include <string>
#include <vector>
#include "Config.h"
/**
 * @class Firewall
 * @brief Rejects requests whose source IP matches a blocked prefix.
 *
 * The firewall uses a simple prefix match:
 * if ip starts with rule.prefix then blocked.
 */
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