#pragma once

/*
 * g++ -std=gnuc++11 -pthread logging.h
 */

#include <atomic>
#include <chrono>
#include <ctime>  //std::localtime
#include <fstream>
#include <iomanip>  //std::iomanip
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>

namespace {

/**
 * @brief format_time
 * @return std::string formatted as "year/mo/dy hour:min:sec.ms(3 digits)
 */
inline std::string format_time() {
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) %
            1000;

  std::stringstream ss;
  ss << std::put_time(std::localtime(&in_time_t), "%Y/%m/%d %X") << '.'
     << std::setfill('0') << std::setw(3) << ms.count();
  return ss.str();
}

}  // namespace

namespace pd {

enum class log_level : uint8_t {
  TRACE = 0,
  DEBUG = 1,
  INFO = 2,
  WARNING = 3,
  ERROR = 4
};

//  using a lambda for hash_helper is impossible because it's a variable rather
//  than a type The best practice is struct functor The example of url
//  <https://en.cppreference.com/w/cpp/utility/hash> shows how it works

// auto log_level_hash_helper = [](log_level t) -> size_t const {
//  return static_cast<size_t>(t);
//};

/**
 * @brief translate log_level to string
 */
struct log_level_hash_helper {
  size_t operator()(log_level t) const noexcept {
    return static_cast<size_t>(t);
  }
};

const std::unordered_map<log_level, std::string, log_level_hash_helper>
    level2string{
        {log_level::INFO, " [INFO] "},   {log_level::TRACE, " [TRACE] "},
        {log_level::DEBUG, " [DEBUG] "}, {log_level::WARNING, " [WARNING] "},
        {log_level::ERROR, " [ERROR] "},
    };

/**
 * @brief
 * set the lowest log_level. Any level under it will be discarded.
 * It is defined as an atomic operation so it's thread-safe
 * @param level: the level wanted
 */
void set_log_level(log_level level);
/**
 * @brief
 * Called before sending any log to stream. If param level is under the setted
 * level cutoff, this log will be discarded.
 * Thread-safe is garenteed because of atomic operation.
 * @param level to be checked
 * @return
 */
bool check_log_level(log_level level);

/**
 *config should be like
 * {
 *  {"type","std_out/file"},
 *  {"file_name","test.log"},
 *  {"reopen_interval","1"},
 * }
 */
using logger_config_t = std::unordered_map<std::string, std::string>;

/**
 * @brief The logger_base class is an ABSTRACT-CLASS which has some defined
 * interfaces
 */
class logger_base {
 public:
  //  logger_base(const logger_config_t& config) {}

  virtual void log(const std::string&, const log_level) = 0;
  virtual void log(const std::string&) = 0;
  virtual ~logger_base() = 0;

  logger_base() {}

 protected:
  std::mutex mutex_;
  uint8_t id_;
};

class std_out_logger : public logger_base {
 public:
  std_out_logger() = delete;
  std_out_logger(const logger_config_t& config);
  void log(const std::string&, const log_level) override;
  void log(const std::string&) override;
  ~std_out_logger() override {}

 private:
  const std::unordered_map<log_level, std::string, log_level_hash_helper>
      levels_;
};

using logger_creator = logger_base* (*)(const logger_config_t&);

class logger_factory {
 public:
  logger_factory();
  logger_base* produce(const logger_config_t& config);

 protected:
  std::unordered_map<std::string, logger_creator> creators;
};

inline logger_factory& get_factory();
logger_base& get_logger(const logger_config_t& config = {{"type", "std_out"}});

inline void log(const std::string& message, const log_level lv) {
  if (!check_log_level(lv)) return;
  get_logger().log(message, lv);
}

inline void log(const std::string& message) { get_logger().log(message); }

inline void TRACE(const std::string& message) {
  log(message, log_level::TRACE);
};
inline void DEBUG(const std::string& message) {
  log(message, log_level::DEBUG);
};
inline void INFO(const std::string& message) { log(message, log_level::INFO); };

inline void WARN(const std::string& message) {
  log(message, log_level::WARNING);
};
inline void ERROR(const std::string& message) {
  log(message, log_level::ERROR);
};

}  // namespace pd
