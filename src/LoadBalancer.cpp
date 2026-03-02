/**
 * @file LoadBalancer.cpp
 * @brief Implements the LoadBalancer simulation and logging behavior.
 */
#include "LoadBalancer.h"
#include <filesystem>
#include <iomanip>
#include <sstream>

LoadBalancer::LoadBalancer(const Config &cfg)
    : config_(cfg),
      firewall_(cfg.blocked_prefixes),
      rng_(std::random_device{}()),
      byte_dist_(0, 255),
      service_time_dist_(cfg.processing_time_min, cfg.processing_time_max),
      prob_dist_(0.0, 1.0),
      batch_size_dist_(cfg.requests_per_arrival_min, cfg.requests_per_arrival_max),
      type_dist_(0, 1)
{
    server_pool_.reserve(static_cast<size_t>(config_.initial_servers + 20));
    for (int i = 0; i < config_.initial_servers; i++)
    {
        server_pool_.push_back(WebServer());
    }
    min_active_servers_ = config_.initial_servers;
    max_active_servers_ = config_.initial_servers;
    ensureLogDir();
    logger_.open(config_.log_file);
}

void LoadBalancer::ensureLogDir()
{
    try
    {
        std::filesystem::create_directories(config_.log_dir);
    }
    catch (...)
    {
    }
}

int LoadBalancer::activeServerCount() const
{
    return static_cast<int>(server_pool_.size());
}

int LoadBalancer::busyServerCount() const
{
    int busy = 0;
    for (const auto &s : server_pool_)
    {
        if (s.isBusy())
        {
            busy++;
        }
    }
    return busy;
}

int LoadBalancer::idleServerCount() const
{
    return activeServerCount() - busyServerCount();
}

std::string LoadBalancer::randomIP()
{
    std::ostringstream oss;
    oss << byte_dist_(rng_) << "."
        << byte_dist_(rng_) << "."
        << byte_dist_(rng_) << "."
        << byte_dist_(rng_);
    return oss.str();
}

char LoadBalancer::randomJobType()
{
    return (type_dist_(rng_) == 0) ? 'P' : 'S';
}

int LoadBalancer::randomTaskTime()
{
    return service_time_dist_(rng_);
}

LoadRequest LoadBalancer::makeRandomRequest(int created_time)
{
    LoadRequest r;
    r.source_ip = randomIP();
    r.destination_ip = randomIP();
    r.service_time = randomTaskTime();
    r.request_type = randomJobType();
    r.arrival_time = created_time;
    return r;
}

void LoadBalancer::seedInitialQueue(int count)
{
    for (int i = 0; i < count; i++)
    {
        LoadRequest r = makeRandomRequest(0);
        if (firewall_.isBlocked(r.source_ip))
        {
            firewall_rejected_count_++;
            continue;
        }
        request_queue_.push(r);
        initial_generated_count_++;
    }
}

void LoadBalancer::writeHeader()
{
    logger_.raw("========================", true);
    logger_.raw("PROJECT 3 — LOAD BALANCER LOG", true);
    logger_.raw("========================", true);

    {
        std::ostringstream oss;
        oss << "Servers (start): " << config_.initial_servers;
        logger_.raw(oss.str(), true);
    }

    {
        std::ostringstream oss;
        oss << "Clock cycles: " << config_.simulation_cycles;
        logger_.raw(oss.str(), true);
    }

    {
        std::ostringstream oss;
        oss << "TASK TIME RANGE (required): [MIN=" << config_.processing_time_min << ", MAX=" << config_.processing_time_max << "] cycles";
        logger_.raw(oss.str(), true);
    }

    logger_.raw("", true);
    logger_.raw("Scaling policy:", true);

    {
        std::ostringstream oss;
        oss << "  Lower threshold = " << config_.queue_lower_threshold << " * active_servers";
        logger_.raw(oss.str(), true);
    }

    {
        std::ostringstream oss;
        oss << "  Upper threshold = " << config_.queue_upper_threshold << " * active_servers";
        logger_.raw(oss.str(), true);
    }

    {
        std::ostringstream oss;
        oss << "  Rest window after scaling (n): " << config_.scale_cooldown_cycles << " cycles";
        logger_.raw(oss.str(), true);
    }

    logger_.raw("", true);
    logger_.raw("Request arrival model:", true);

    {
        std::ostringstream oss;
        oss << "  Arrival chance per cycle: " << config_.request_arrival_probability;
        logger_.raw(oss.str(), true);
    }

    {
        std::ostringstream oss;
        oss << "  Batch size per arrival: " << config_.requests_per_arrival_min << " to " << config_.requests_per_arrival_max;
        logger_.raw(oss.str(), true);
    }

    {
        std::ostringstream oss;
        oss << "  Initial fill rule: servers * " << config_.initial_queue_multiplier;
        logger_.raw(oss.str(), true);
    }

    logger_.raw("", true);
    logger_.raw("Firewall / IP blocker:", true);

    if (config_.blocked_prefixes.empty())
    {
        logger_.raw("  Blocked ranges: (none)", true);
    }
    else
    {
        std::ostringstream oss;
        oss << "  Blocked ranges: ";
        for (size_t i = 0; i < config_.blocked_prefixes.size(); i++)
        {
            if (i)
            {
                oss << ", ";
            }
            oss << config_.blocked_prefixes[i].prefix;
        }
        logger_.raw(oss.str(), true);
    }

    logger_.raw("  Rejection action: discard + log", true);
    logger_.raw("========================", true);
}

void LoadBalancer::maybeGenerateArrivals()
{
    if (prob_dist_(rng_) > config_.request_arrival_probability)
    {
        return;
    }

    int batch = batch_size_dist_(rng_);

    if (batch <= 0)
    {
        return;
    }

    int added = 0;
    int rejected = 0;

    for (int i = 0; i < batch; i++)
    {
        LoadRequest r = makeRandomRequest(cycle_);
        runtime_generated_count_++;

        if (firewall_.isBlocked(r.source_ip))
        {
            firewall_rejected_count_++;
            rejected++;
            continue;
        }

        request_queue_.push(r);
        added++;
    }

    if (added > 0)
    {
        std::ostringstream oss;
        oss << "t=" << std::setw(5) << std::setfill('0') << cycle_
            << " ARRIVAL +" << added
            << " | queue=" << request_queue_.size()
            << " | rejected_total=" << firewall_rejected_count_;
        logger_.log(LogKind::REQUEST_ARRIVAL, oss.str(), false, config_.enable_color_output);
    }

    if (rejected > 0)
    {
        std::ostringstream oss;
        oss << "t=" << std::setw(5) << std::setfill('0') << cycle_
            << " FIREWALL_REJECT +" << rejected
            << " | rejected_total=" << firewall_rejected_count_
            << " | queue=" << request_queue_.size();
        logger_.log(LogKind::FIREWALL_BLOCK, oss.str(), false, config_.enable_color_output);
    }
}

void LoadBalancer::assignToIdleServers()
{
    for (int i = 0; i < activeServerCount(); i++)
    {
        if (request_queue_.empty())
        {
            break;
        }

        if (!server_pool_[static_cast<size_t>(i)].isBusy())
        {
            LoadRequest r = request_queue_.front();
            request_queue_.pop();

            server_pool_[static_cast<size_t>(i)].assign(r);

            if (config_.log_interval > 0 && (cycle_ % config_.log_interval == 0))
            {
                std::ostringstream oss;
                oss << "t=" << std::setw(5) << std::setfill('0') << cycle_
                    << " ASSIGN server=" << i
                    << " job=" << r.request_type
                    << " time=" << r.service_time
                    << " | queue=" << request_queue_.size();
                logger_.log(LogKind::REQUEST_ASSIGN, oss.str(), false, config_.enable_color_output);
            }
        }
    }
}

void LoadBalancer::tickServers()
{
    for (int i = 0; i < activeServerCount(); i++)
    {
        auto &s = server_pool_[static_cast<size_t>(i)];
        bool wasBusy = s.isBusy();
        s.tick();

        if (wasBusy && s.justFinished())
        {
            completed_count_++;

            if (config_.log_interval > 0 && (cycle_ % config_.log_interval == 0))
            {
                std::ostringstream oss;
                oss << "t=" << std::setw(5) << std::setfill('0') << cycle_
                    << " COMPLETE server=" << i
                    << " | completed=" << completed_count_;
                logger_.log(LogKind::REQUEST_COMPLETE, oss.str(), false, config_.enable_color_output);
            }
        }
    }
}

void LoadBalancer::addOneServer()
{
    server_pool_.push_back(WebServer());
    scale_up_count_++;

    if (activeServerCount() > max_active_servers_)
    {
        max_active_servers_ = activeServerCount();
    }
}

bool LoadBalancer::tryRemoveOneServer()
{
    if (activeServerCount() <= 1)
    {
        return false;
    }

    WebServer &last = server_pool_.back();

    if (!last.canBeRemovedSafely())
    {
        return false;
    }

    server_pool_.pop_back();
    scale_down_count_++;

    if (activeServerCount() < min_active_servers_)
    {
        min_active_servers_ = activeServerCount();
    }

    return true;
}
void LoadBalancer::maybeScaleServers()
{
    if (scale_wait_ > 0)
    {
        scale_wait_--;
        return;
    }

    const int servers = activeServerCount();
    const long long qsize = static_cast<long long>(request_queue_.size());

    const long long lower = 1LL * config_.queue_lower_threshold * servers;
    const long long upper = 1LL * config_.queue_upper_threshold * servers;

    if (qsize > upper)
    {
        addOneServer();
        scale_wait_ = config_.scale_cooldown_cycles;

        std::ostringstream oss;
        oss << "t=" << std::setw(5) << std::setfill('0') << cycle_
            << " SCALE_UP to " << activeServerCount()
            << " | queue=" << request_queue_.size()
            << " (above " << config_.queue_upper_threshold << "*servers)";
        logger_.log(LogKind::SERVER_SCALE_UP, oss.str(), true, config_.enable_color_output);
        return;
    }

    if (qsize < lower)
    {
        bool removed = tryRemoveOneServer();
        if (removed)
        {
            scale_wait_ = config_.scale_cooldown_cycles;

            std::ostringstream oss;
            oss << "t=" << std::setw(5) << std::setfill('0') << cycle_
                << " SCALE_DOWN to " << activeServerCount()
                << " | queue=" << request_queue_.size()
                << " (below " << config_.queue_lower_threshold << "*servers)";
            logger_.log(LogKind::SERVER_SCALE_DOWN, oss.str(), true, config_.enable_color_output);
        }
    }
}

void LoadBalancer::writeFinalSummary(int starting_queue)
{
    logger_.raw("========================", true);
    logger_.raw("FINAL SUMMARY", true);
    logger_.raw("========================", true);

    {
        std::ostringstream oss;
        oss << "Starting queue size (required): " << starting_queue;
        logger_.raw(oss.str(), true);
    }
    {
        std::ostringstream oss;
        oss << "Ending queue size (required): " << request_queue_.size();
        logger_.raw(oss.str(), true);
    }
    {
        std::ostringstream oss;
        oss << "Task time range (required): " << config_.processing_time_min
            << " to " << config_.processing_time_max << " cycles";
        logger_.raw(oss.str(), true);
    }

    logger_.raw("", true);
    logger_.raw("Total requests:", true);
    {
        std::ostringstream oss;
        oss << "  Initial generated (queued): " << initial_generated_count_;
        logger_.raw(oss.str(), true);
    }
    {
        std::ostringstream oss;
        oss << "  New generated during run:   " << runtime_generated_count_;
        logger_.raw(oss.str(), true);
    }
    {
        std::ostringstream oss;
        oss << "  Completed:                  " << completed_count_;
        logger_.raw(oss.str(), true);
    }
    {
        std::ostringstream oss;
        oss << "  Rejected (firewall):        " << firewall_rejected_count_;
        logger_.raw(oss.str(), true);
    }
    {
        std::ostringstream oss;
        oss << "  Remaining in queue:         " << request_queue_.size();
        logger_.raw(oss.str(), true);
    }

    logger_.raw("", true);
    logger_.raw("Server scaling:", true);
    {
        std::ostringstream oss;
        oss << "  Active servers start: " << config_.initial_servers;
        logger_.raw(oss.str(), true);
    }
    {
        std::ostringstream oss;
        oss << "  Active servers end:   " << activeServerCount();
        logger_.raw(oss.str(), true);
    }
    {
        std::ostringstream oss;
        oss << "  Min active servers:   " << min_active_servers_;
        logger_.raw(oss.str(), true);
    }
    {
        std::ostringstream oss;
        oss << "  Max active servers:   " << max_active_servers_;
        logger_.raw(oss.str(), true);
    }
    {
        std::ostringstream oss;
        oss << "  Scale up events:      " << scale_up_count_;
        logger_.raw(oss.str(), true);
    }
    {
        std::ostringstream oss;
        oss << "  Scale down events:    " << scale_down_count_;
        logger_.raw(oss.str(), true);
    }

    logger_.raw("", true);
    logger_.raw("Queue behavior:", true);
    {
        double avg = (config_.simulation_cycles > 0)
                         ? static_cast<double>(queue_size_sum_) / static_cast<double>(config_.simulation_cycles)
                         : 0.0;

        std::ostringstream oss;
        oss << "  Peak queue size: " << peak_queue_size_;
        logger_.raw(oss.str(), true);

        std::ostringstream oss2;
        oss2 << "  Avg queue size:  " << std::fixed << std::setprecision(2) << avg;
        logger_.raw(oss2.str(), true);
    }

    logger_.raw("", true);
    logger_.raw("End status (rubric):", true);
    {
        std::ostringstream oss;
        oss << "  Active servers:   " << activeServerCount();
        logger_.raw(oss.str(), true);
    }
    {
        std::ostringstream oss;
        oss << "  Busy servers:     " << busyServerCount();
        logger_.raw(oss.str(), true);
    }
    {
        std::ostringstream oss;
        oss << "  Idle servers:     " << idleServerCount();
        logger_.raw(oss.str(), true);
    }
    {
        std::ostringstream oss;
        oss << "  Remaining queue:  " << request_queue_.size();
        logger_.raw(oss.str(), true);
    }
    {
        std::ostringstream oss;
        oss << "  Rejected requests: " << firewall_rejected_count_;
        logger_.raw(oss.str(), true);
    }

    logger_.raw("========================", true);
}

void LoadBalancer::run()
{
    writeHeader();

    const int initial_requests = config_.initial_servers * config_.initial_queue_multiplier;
    seedInitialQueue(initial_requests);

    const int starting_queue = static_cast<int>(request_queue_.size());

    {
        std::ostringstream oss;
        oss << "Starting queue size (required): " << starting_queue
            << " (seeded from servers * " << config_.initial_queue_multiplier << ")";
        logger_.log(LogKind::LOG_INFO, oss.str(), true, config_.enable_color_output);
    }

    for (cycle_ = 1; cycle_ <= config_.simulation_cycles; cycle_++)
    {
        tickServers();
        assignToIdleServers();
        maybeGenerateArrivals();
        maybeScaleServers();

        long long qsz = static_cast<long long>(request_queue_.size());
        queue_size_sum_ += qsz;
        if (qsz > peak_queue_size_)
        {
            peak_queue_size_ = qsz;
        }

        if (config_.log_interval > 0 && (cycle_ % config_.log_interval == 0))
        {
            std::ostringstream oss;
            oss << "t=" << std::setw(5) << std::setfill('0') << cycle_
                << " queue=" << request_queue_.size()
                << " servers=" << activeServerCount()
                << " busy=" << busyServerCount()
                << " completed=" << completed_count_
                << " rejected=" << firewall_rejected_count_;
            logger_.log(LogKind::STATUS, oss.str(), true, config_.enable_color_output);
        }
    }

    writeFinalSummary(starting_queue);
}
