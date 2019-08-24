#include "PDLog.h"
#include <functional>
#include <future>
#include <iostream>
#include <thread>
#include <vector>
using namespace pd;
size_t work()
{
    std::ostringstream s;
    s << "hi my name is: " << std::this_thread::get_id();
    set_log_level(log_level::TRACE);

    for (size_t i = 0; i < 2000; ++i) {
        //std::async is pretty uninteresting unless you make things yield
        ERROR(s.str());
        WARN(s.str());
        INFO(s.str());
        DEBUG(s.str());
        TRACE(s.str());
    }
    return 10;
}

int main()
{
    //optional : init_logger()
    //Without it , the default setting is std_out_logger
    //the file_log will be written to "logs/{file_name}"
    //note that whithout the std::filesystem's support
    //C++ doesn't provide a machanism to check whether the dir "logs" exsits
    //you must create it mannully.
    init_logger({{"type", "file"}, {"file_name", "test.log"}});
    std::vector<std::future<size_t>> results;
    for (size_t i = 0; i < 4; ++i) {
        results.emplace_back(std::async(std::launch::async, work));
    }

    size_t exit_code = 0;
    for (auto &result : results) {
        try {
            size_t count = result.get();
        } catch (std::exception &e) {
            std::cout << e.what();
            exit_code++;
        }
    }
    return exit_code;
}
