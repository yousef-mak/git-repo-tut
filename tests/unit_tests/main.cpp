#include <gtest/gtest.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <vector>
#include <string>

// Custom printer: only final summary printed after all tests
class CustomTestResultPrinter : public ::testing::EmptyTestEventListener {
private:
    int total_tests = 0;
    int passed_tests = 0;
    int failed_tests = 0;
    std::chrono::steady_clock::time_point start_time;

    struct FailedTestInfo {
        std::string test_case_name;
        std::string test_name;
    };
    std::vector<FailedTestInfo> failed_test_list;

public:
    void OnTestProgramStart(const ::testing::UnitTest& unit_test) override {
        total_tests = unit_test.test_to_run_count();
        start_time = std::chrono::steady_clock::now();
    }

    void OnTestEnd(const ::testing::TestInfo& test_info) override {
        if (test_info.result()->Passed()) {
            passed_tests++;
        } else {
            failed_tests++;
            failed_test_list.push_back({test_info.test_case_name(), test_info.name()});
        }
    }

    void OnTestProgramEnd(const ::testing::UnitTest& unit_test) override {
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "               TEST RESULTS SUMMARY\n";
        std::cout << std::string(60, '=') << "\n";
        std::cout << "Total Tests Run : " << total_tests << "\n";
        std::cout << "Tests Passed   : " << passed_tests << " ✓\n";
        std::cout << "Tests Failed   : " << failed_tests << (failed_tests == 0 ? " ✓" : " ✗") << "\n";
        std::cout << "Execution Time : " << duration.count() << " ms\n";

        if (failed_tests > 0) {
            std::cout << "\nList of Failed Tests:\n";
            for (const auto& fail : failed_test_list) {
                std::cout << " - " << fail.test_case_name << "." << fail.test_name << "\n";
            }
        } else {
            std::cout << "\n🎉 ALL TESTS PASSED SUCCESSFULLY!\n";
        }

        std::cout << std::string(60, '=') << "\n";
    }
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
    delete listeners.Release(listeners.default_result_printer());
    listeners.Append(new CustomTestResultPrinter);

    return RUN_ALL_TESTS();
}
