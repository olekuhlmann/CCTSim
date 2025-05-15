// CustomIterationLog.hh
#ifndef CUSTOM_ITERATION_LOG_HH
#define CUSTOM_ITERATION_LOG_HH

#include <rat/common/log.hh>
#include <mutex>
#include <sstream>
#include <string>
#include <cstdio>
#include <cstdarg>
#include <algorithm>
#include <cctype>

/**
 * @brief A custom log class that captures iteration values from log messages.
 *
 * This class derives from the Log class and overrides logging methods to capture
 * the most recent iteration log values (iteration number, function value,
 * edge regression constraint, length constraint, and optionally curvature constraint).
 * It does not output any messages.
 */
class CustomIterationLog : public rat::cmn::Log {
public:
    /**
     * @brief Structure holding iteration log values.
     */
    struct IterationValues {
        int iter;       /**< Iteration number */
        double fval;    /**< Function value */
        double ercf;    /**< Edge regression constraint function value */
        double lcf;     /**< Length constraint function value */
        double ccf;     /**< Curvature constraint function value (valid if has_ccf is true) */
        bool has_ccf;   /**< Flag indicating if the curvature constraint value is valid */
    };

private:
    mutable std::mutex iter_mutex_;  /**< Mutex protecting iteration values */
    IterationValues last_values_;    /**< Most recent iteration values */

    // State for collecting one set of iteration data.
    enum class DataState {
        WAIT_FOR_ITER_FVAL,    // expecting a msg() without incr carrying iter & fval
        WAIT_FOR_ERCF,         // expecting first msg(incr=0): ercf value
        WAIT_FOR_LCF,          // expecting second msg(incr=0): lcf value
        WAIT_FOR_OPTIONAL_CCF, // optionally expecting third msg(incr=0): ccf value
        WAIT_FOR_SET_END       // expecting finishing msg() (without incr) with blank to signal set complete
    };

    DataState current_state_ = DataState::WAIT_FOR_ITER_FVAL;

    // Temporary storage for the current set of iteration data.
    int temp_iter_;
    double temp_fval_;
    double temp_ercf_;
    double temp_lcf_;
    double temp_ccf_;
    bool temp_ccf_provided_ = false; // true if ccf was provided

    // Helper function to trim whitespace.
    std::string trim(const std::string &s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        size_t end = s.find_last_not_of(" \t\r\n");
        if (start == std::string::npos)
            return "";
        return s.substr(start, end - start + 1);
    }

    // Helper function to remove ANSI escape sequences.
    std::string removeAnsi(const std::string &s) {
        std::string result;
        size_t i = 0;
        while (i < s.size()) {
            if (s[i] == '\033') { // ANSI escape sequence starts with ESC
                // Skip until we hit 'm'
                while (i < s.size() && s[i] != 'm')
                    ++i;
                if (i < s.size())
                    ++i; // skip the 'm'
            } else {
                result.push_back(s[i]);
                ++i;
            }
        }
        return result;
    }

public:
    /**
     * @brief Constructor.
     *
     * Initializes the iteration values to zero and sets the log type to NONE.
     */
    CustomIterationLog() : Log(NONE) {
        last_values_.iter = 0;
        last_values_.fval = 0.0;
        last_values_.ercf = 0.0;
        last_values_.lcf = 0.0;
        last_values_.ccf = 0.0;
        last_values_.has_ccf = false;
    }

    /**
     * @brief Logs a message without an indentation change.
     *
     * This version is used both to start a new set (with iter and fval)
     * and to signal the end of a set (a blank message).
     *
     * @param fmt Format string.
     * @param ... Variable arguments.
     */
    void msg(const char* fmt, ...) override {
        const int BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];

        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, BUFFER_SIZE, fmt, args);
        va_end(args);

        // Debug output.
        //std::fprintf(stderr, "DEBUG: msg: %s\n", buffer);

        std::lock_guard<std::mutex> lock(iter_mutex_);
        std::string msgStr(buffer);
        // Remove ANSI escape sequences.
        std::string cleanStr = removeAnsi(msgStr);
        std::string trimmed = trim(cleanStr);

        // If we're in the start state, expect iter and fval.
        if (current_state_ == DataState::WAIT_FOR_ITER_FVAL) {
            if (!trimmed.empty()) {
                std::istringstream iss(trimmed);
                double iter_d, fval;
                if (iss >> iter_d >> fval) {
                    temp_iter_ = static_cast<int>(iter_d);
                    temp_fval_ = fval;
                    current_state_ = DataState::WAIT_FOR_ERCF;
                }
            }
        }
        // In either WAIT_FOR_OPTIONAL_CCF or WAIT_FOR_SET_END state,
        // a blank message signals the end of the set.
        else if ((current_state_ == DataState::WAIT_FOR_OPTIONAL_CCF ||
                  current_state_ == DataState::WAIT_FOR_SET_END) &&
                 trimmed.empty()) {
            // Commit the collected values.
            last_values_.iter = temp_iter_;
            last_values_.fval = temp_fval_;
            last_values_.ercf = temp_ercf_;
            last_values_.lcf  = temp_lcf_;
            last_values_.ccf  = (current_state_ == DataState::WAIT_FOR_SET_END) ? temp_ccf_ : 0.0;
            last_values_.has_ccf = (current_state_ == DataState::WAIT_FOR_SET_END);
            // Reset state for next set.
            current_state_ = DataState::WAIT_FOR_ITER_FVAL;
        }
        // Otherwise, ignore messages.
    }

    /**
     * @brief Logs a message with an indentation change.
     *
     * This version (when incr==0) is used to supply the constraint values:
     * first call provides ercf, second provides lcf, third (if present) provides ccf.
     *
     * @param incr Indentation change (should be 0 for data messages).
     * @param fmt Format string.
     * @param ... Variable arguments.
     */
    void msg(const int incr, const char* fmt, ...) override {
        const int BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];

        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, BUFFER_SIZE, fmt, args);
        va_end(args);

        // Debug output.
        //std::fprintf(stderr, "DEBUG: msg(incr=%d): %s\n", incr, buffer);

        // If incr is non-zero, update indentation and exit.
        if (incr != 0) {
            num_indent_ += incr; // update parent's indentation if needed.
            return;
        }

        std::lock_guard<std::mutex> lock(iter_mutex_);
        std::string msgStr(buffer);
        std::string cleanStr = removeAnsi(msgStr);
        std::string trimmed = trim(cleanStr);
        if (trimmed.empty()) return; // ignore blank messages here

        std::istringstream iss(trimmed);
        double value;
        if (!(iss >> value)) return; // if parsing fails, ignore

        // Depending on the current state, assign the value.
        if (current_state_ == DataState::WAIT_FOR_ERCF) {
            temp_ercf_ = value;
            current_state_ = DataState::WAIT_FOR_LCF;
        } else if (current_state_ == DataState::WAIT_FOR_LCF) {
            temp_lcf_ = value;
            current_state_ = DataState::WAIT_FOR_OPTIONAL_CCF;
        } else if (current_state_ == DataState::WAIT_FOR_OPTIONAL_CCF) {
            temp_ccf_ = value;
            temp_ccf_provided_ = true;
            current_state_ = DataState::WAIT_FOR_SET_END;
        }
        // Ignore messages if not expected.
    }

    /**
     * @brief Overrides newl() to do nothing.
     */
    void newl() override {}

    /**
     * @brief Overrides hline() to do nothing.
     *
     * @param width Width of the horizontal line.
     * @param ch Character to fill the line.
     * @param str1 Optional string.
     * @param str2 Optional string.
     */
    void hline(const int width, const char ch = '=', const std::string& str1 = {}, const std::string& str2 = {}) override {}

    /**
     * @brief Retrieves the most recent iteration values.
     *
     * @return A copy of the IterationValues structure containing the last captured values.
     */
    IterationValues get_last_iteration_values() const {
        std::lock_guard<std::mutex> lock(iter_mutex_);
        return last_values_;
    }
};

#endif // CUSTOM_ITERATION_LOG_HH
