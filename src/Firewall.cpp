#include "Firewall.h"

Firewall::Firewall(const std::vector<BlockRule>& blockrules):blockrules_(blockrules) {}

void Firewall::setRules(const std::vector<BlockRule>& blockrules) 
{
    blockrules_=blockrules;
}

bool Firewall::isBlocked(const std::string& ipaddr) const 
{
    for (const auto& rule:blockrules_) 
    {
        if (!rule.prefix.empty() && ipaddr.rfind(rule.prefix,0)==0) 
        { 
            return true;
        }
    }
    return false;
}