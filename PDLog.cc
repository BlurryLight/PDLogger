#include "PDLog.h"

static std::atomic<unsigned int> log_level_num{
    static_cast<unsigned int>(pd::log_level::INFO)};

void pd::set_log_level(pd::log_level level) {
  log_level_num.store(static_cast<size_t>(level));
}

bool pd::check_log_level(pd::log_level level) {
  return static_cast<size_t>(level) >=
         log_level_num.load(std::memory_order_relaxed);
}

pd::std_out_logger::std_out_logger(const pd::logger_config_t &config)
    : levels_(level2string) {}

void pd::std_out_logger::log(const std::string &message,
                             const pd::log_level lv) {
  std::string output;
  output.reserve(message.length() + 64);
  output.append(::format_time())
      .append(levels_.find(lv)->second)
      .append(message)
      .push_back('\n');
  log(output);
}

void pd::std_out_logger::log(const std::string &message) {
  std::cout << message;
  std::cout.flush();
}

pd::logger_base::~logger_base() {}

pd::logger_factory::logger_factory() {
  creators.emplace("std_out",
                   [](const logger_config_t &config) -> logger_base * {
                     return new std_out_logger(config);
                   });
}

pd::logger_base *pd::logger_factory::produce(
    const pd::logger_config_t &config) {
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

pd::logger_factory &pd::get_factory() {
  static logger_factory factory_singleton;
  return factory_singleton;
}

pd::logger_base &pd::get_logger(const pd::logger_config_t &config) {
  static std::unique_ptr<logger_base> logger_singleton(
      get_factory().produce(config));
  return *logger_singleton;
}
