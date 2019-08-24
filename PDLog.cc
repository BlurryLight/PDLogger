#include "PDLog.h"

static std::atomic<unsigned int> log_level_num{static_cast<unsigned int>(pd::log_level::INFO)};

void pd::set_log_level(pd::log_level level)
{
    log_level_num.store(static_cast<size_t>(level));
}

bool pd::details::check_log_level(pd::log_level level)
{
    return static_cast<size_t>(level) >= log_level_num.load(std::memory_order_relaxed);
}

pd::details::std_out_logger::std_out_logger(const pd::details::logger_config_t &config)
    : levels_(level2string)
{}

//void pd::details::std_out_logger::log(const std::string &message, const pd::details::log_level lv)
//{
//    std::string output;
//    output.reserve(message.length() + 128);
//    output.append(::format_time())
//        .append(levels_.find(lv)->second)
//        .append("[Thread ID:")
//        .append(std::to_string(threadid_to_index(std::this_thread::get_id())))
//        .append("] ")
//        .append(message)
//        .push_back('\n');
//    if (lv >= log_level::WARNING) {
//        output.append(__FILE__).append(std::to_string(__LINE__)).append(__FUNCTION__);
//    }
//    log(output);
//}

void pd::details::std_out_logger::log(const std::string &message)
{
    std::cout << message;
    std::cout.flush();
}

pd::details::logger_base::~logger_base() {}

pd::details::logger_factory::logger_factory()
{
    creators.emplace("std_out", [](const logger_config_t &config) -> logger_base * {
        return new std_out_logger(config);
    });
    creators.emplace("file", [](const logger_config_t &config) -> logger_base * {
        return new file_logger(config);
    });
}

pd::details::logger_base *pd::details::logger_factory::produce(
    const pd::details::logger_config_t &config)
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

pd::details::logger_factory &pd::details::get_factory()
{
    static logger_factory factory_singleton;
    return factory_singleton;
}

pd::details::logger_base &pd::details::get_logger(const pd::details::logger_config_t &config)
{
    static std::unique_ptr<logger_base> logger_singleton(get_factory().produce(config));
    return *logger_singleton;
}

static std::atomic<size_t> log_id{0};

pd::details::file_logger::file_logger(const pd::details::logger_config_t &config)
    : levels_(level2string)
    , os_(std::unique_ptr<std::ofstream>(new std::ofstream))
    , buf_(std::unique_ptr<std::vector<char>>(new std::vector<char>))
{
    auto name = config.find("file_name");
    if (name == config.end())
        throw std::runtime_error("NO FILE PROVIDED");
    filename_ = name->second;

    //Benchmark shows increasing ofstream buffer cannot get an higher performance :(
    //But I don't want to delete it so just let it go
    constexpr int buf_size = 1024 * 128; //128k
    buf_->resize(buf_size);

    //mannualy increase stream's buffer
    os_->rdbuf()->pubsetbuf(buf_->data(), buf_size);
    roll_a_file();
}

void pd::details::logger_base::log(const std::string &message, const pd::log_level lv)
{
    std::string output;
    output.reserve(message.length() + 256);
    output.append(::format_time())
        .append(level2string.find(lv)->second)
        .append("[Thread ID:")
        .append(std::to_string(threadid_to_index(std::this_thread::get_id())))
        .append("] ")
        .append(message);
    if (lv >= log_level::WARNING) {
        output.append(" [FILE:] ")
            .append(__FILE__)
            .append(" [LINE:] ")
            .append(std::to_string(__LINE__))
            .append("[FUNC:] ")
            .append(__FUNCTION__);
    }

    output.push_back('\n');
    log(output);
}

void pd::details::file_logger::log(const std::string &message)
{
    log_id++; //atomic operation
    if ((log_id.load(std::memory_order_relaxed) % 10000) == 0) {
        roll_a_file();
    }
    std::lock_guard<std::mutex> lock(mutex_);
    *os_.get() << message;
    // I think there is no need to flush everytime. It is much quicker ( Benchmark shows about 5-10X improvement) to write chars to buf_ before writting to disk.
    // flush() will be invoked when roll_a_file(). Under the lock's protection  it will be thread-safe and I hope
    // no logline will be discarded.  I will keep an eye on it.

    //    os_->flush();
}

void pd::details::file_logger::roll_a_file()
{
    size_t num_tail = (log_id.load(std::memory_order_relaxed)) / 10000;
    std::lock_guard<std::mutex> lock(mutex_);
    if (os_->is_open()) {
        os_->flush();
        os_->close();
    }
    os_->open("logs/" + std::to_string(num_tail) + "_" + filename_,
              std::ofstream::out | std::ofstream::app);
}
