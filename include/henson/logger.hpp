#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
namespace spd = spdlog;

namespace henson {
    inline
    std::shared_ptr<spd::logger>
    create_logger(std::string log_level)
    {
        auto log = spd::stderr_logger_mt("henson");
        int lvl = spd::level::from_str(log_level);
        log->set_level(static_cast<spd::level::level_enum>(lvl));
        return log;
    }

    inline
    std::shared_ptr<spd::logger>
    get_logger()
    {
        auto log = spd::get("henson");
        if (!log) {
            auto null_sink = std::make_shared<spd::sinks::null_sink_mt>();
            log = std::make_shared<spd::logger>("null_logger", null_sink);
        }
        return log;
    }

    inline
    std::shared_ptr<spd::logger>
    get_or_create_logger(std::string log_level = "info")
    {
        auto log = spd::get("henson");
        if (!log) {
            log = create_logger(log_level);
        }
        return log;
    }

    inline
    spd::level::level_enum
    get_log_level()
    {
        auto log = spd::get("henson");
        if (log)
            return log->level();
        else
            return spd::level::off;
    }

    inline
    void set_log_level(std::string log_level)
    {
        auto log = spd::get("henson");
        if (log) {
            int lvl = spd::level::from_str(log_level);
            log->set_level(static_cast<spd::level::level_enum>(lvl));
        }
    }
}
