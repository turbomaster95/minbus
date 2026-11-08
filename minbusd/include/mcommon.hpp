#pragma once

#include <csignal>
#include <atomic>
#include <unistd.h>
#include <iostream>

std::atomic<bool> g_running{true};

inline void handle_signal(int signum) {
    g_running = false;
}

inline int setup_signals() {
    struct sigaction sa{};
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, nullptr) < 0) return -1;
    if (sigaction(SIGTERM, &sa, nullptr) < 0) return -1;

    return 0;
}
