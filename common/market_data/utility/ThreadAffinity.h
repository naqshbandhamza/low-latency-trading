#pragma once

#include <cerrno>
#include <thread>

#if defined(__linux__)
#include <pthread.h>
#include <sched.h>
#endif

namespace llt::utility
{

    struct ThreadRealtimeConfig
    {
        int coreId{-1};       // < 0 disables pinning
        int fifoPriority{0};  // 1..99 enables SCHED_FIFO (only applied if pinned to core > 0)
    };

    struct ThreadRealtimeStatus
    {
        bool pinned{false};
        bool realtime{false};
        int assignedCore{-1};
        int appliedPriority{0};
        int pinErrno{0};
        int schedErrno{0};
    };

    [[nodiscard]]
    inline ThreadRealtimeStatus configureCurrentThread(
        const ThreadRealtimeConfig &config) noexcept
    {
        ThreadRealtimeStatus status{};

#if defined(__linux__)
        const unsigned int cpuCount = std::thread::hardware_concurrency();

        if (config.coreId >= 0)
        {
            if (cpuCount > 0 && static_cast<unsigned int>(config.coreId) >= cpuCount)
            {
                status.pinErrno = EINVAL;
            }
            else
            {
                cpu_set_t cpuset;
                CPU_ZERO(&cpuset);
                CPU_SET(config.coreId, &cpuset);

                const int rc = ::pthread_setaffinity_np(
                    ::pthread_self(),
                    sizeof(cpu_set_t),
                    &cpuset);

                if (rc == 0)
                {
                    status.pinned = true;
                    status.assignedCore = config.coreId;
                }
                else
                {
                    status.pinErrno = rc;
                }
            }
        }

        // Only elevate a spinning thread to SCHED_FIFO if it is pinned away from core 0.
        if (config.fifoPriority > 0 && status.pinned && status.assignedCore > 0)
        {
            sched_param param{};
            param.sched_priority = config.fifoPriority;

            const int rc = ::pthread_setschedparam(
                ::pthread_self(),
                SCHED_FIFO,
                &param);

            if (rc == 0)
            {
                status.realtime = true;
                status.appliedPriority = config.fifoPriority;
            }
            else
            {
                status.schedErrno = rc;
            }
        }
#else
        (void)config;
        status.pinErrno = ENOTSUP;
        status.schedErrno = ENOTSUP;
#endif

        return status;
    }

} // namespace llt::utility