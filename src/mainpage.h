/**
 * @mainpage LoadBalancer Project 3
 *
 * @section overview Overview
 * This project simulates a cloud load balancer that distributes web requests across a dynamic pool of web servers.
 * Each request contains a source IP, destination IP, service time (clock cycles), request type (Processing/Streaming),
 * and the cycle when it arrived.
 *
 * @section workflow Workflow
 * - Seed an initial queue of requests (typically servers * multiplier).
 * - For each simulation cycle:
 *   - Update servers (decrement remaining service time).
 *   - Assign queued requests to idle servers.
 *   - Randomly generate new arrivals.
 *   - Scale server count based on queue thresholds and a cooldown window.
 * - Enforce an IP blocker (firewall) to reject requests from blocked prefixes.
 *
 * @section outputs Outputs
 * - Console output (optionally colored).
 * - Log file containing events and a final summary:
 *   - Starting queue size
 *   - Ending queue size
 *   - Task/service time range
 *   - Requests completed / rejected / remaining
 *   - Server scaling statistics
 *
 * @section author Author
 * Ketan Verma CSCE 412
 */