#include "PDLog.h"

static std::atomic<unsigned int> log_level_num{static_cast<unsigned int>(pd::log_level::INFO)};

void pd::set_log_level(pd::log_level level)
{
    log_level_num.store(static_cast<size_t>(level));
}

bool pd::check_log_level(pd::log_level level)
{
    return static_cast<size_t>(level) >= log_level_num.load(std::memory_order_relaxed);
}

pd::std_out_logger::std_out_logger(const pd::logger_config_t &config)
    : levels_(level2string)
{}

void pd::std_out_logger::log(const std::string &message, const pd::log_level lv)
{
    std::string output;
    output.reserve(message.length() + 64);
    output.append(::format_time())
        .append(levels_.find(lv)->second)
        .append("[Thread ID:")
        .append(std::to_string(threadid_to_index(std::this_thread::get_id())))
        .append("] ")
        .append(message)
        .push_back('\n');
    log(output);
}

void pd::std_out_logger::log(const std::string &message)
{
    std::cout << message;
    std::cout.flush();
}

pd::logger_base::~logger_base() {}

pd::logger_factory::logger_factory()
{
    creators.emplace("std_out", [](const logger_config_t &config) -> logger_base * {
        return new std_out_logger(config);
    });
    creators.emplace("file", [](const logger_config_t &config) -> logger_base * {
        return new file_logger(config);
    });
}

pd::logger_base *pd::logger_factory::produce(const pd::logger_config_t &config)
{
    auto type = config.find("type");
    if (type == config.end()) {
        throw std::runtime_error("LOGGER TYPE ERROR");
    }
    auto found = creators.find(type->second);
    if (type == config.end()) {
        throw std::runtime_error("CREATORS ERROR");
    }
    return found->second(config);
}

pd::logger_factory &pd::get_factory()
{
    static logger_factory factory_singleton;
    return factory_singleton;
}

pd::logger_base &pd::get_logger(const pd::logger_config_t &config)
{
    static std::unique_ptr<logger_base> logger_singleton(get_factory().produce(config));
    return *logger_singleton;
}

static std::atomic<size_t> log_id{0};

pd::file_logger::file_logger(const pd::logger_config_t &config)
    : levels_(level2string)
    , os_(std::unique_ptr<std::ofstream>(new std::ofstream))
{
    auto name = config.find("file_name");
    if (name == config.end())
        throw std::runtime_error("NO FILE PROVIDED");
    filename_ = name->second;
    roll_a_file();
}

void pd::file_logger::log(const std::string &message, const pd::log_level lv)
{
    std::string output;
    output.reserve(message.length() + 64);
    output.append(::format_time())
        .append(levels_.find(lv)->second)
        .append("[Thread ID:")
        .append(std::to_string(threadid_to_index(std::this_thread::get_id())))
        .append("] ")
        .append(message)
        .push_back('\n');
    log(output);
}

void pd::file_logger::log(const std::string &message)
{
    log_id++; //atomic operation
    if ((log_id.load(std::memory_order_relaxed) % 10000) == 0) {
        roll_a_file();
    }
    std::lock_guard<std::mutex> lock(mutex_);
    *os_.get() << message;
    os_->flush();
}

void pd::file_logger::roll_a_file()
{
    size_t num_tail = (log_id.load(std::memory_order_relaxed)) / 10000;
    std::lock_guard<std::mutex> lock(mutex_);
    if (os_->is_open()) {
        os_->flush();
        os_->close();
    }
    os_->open(std::to_string(num_tail) + "_" + filename_, std::ofstream::out | std::ofstream::app);
}
