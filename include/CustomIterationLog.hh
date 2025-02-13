// CustomIterationLog.hh
#ifndef CUSTOM_ITERATION_LOG_HH
#define CUSTOM_ITERATION_LOG_HH

#include <rat/common/log.hh>
#include <mutex>
#include <sstream>
#include <string>
#include <cstdio>
#include <cstdarg>

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
     * @brief Logs a message.
     *
     * This method overrides the base msg() method to capture iteration values if they are present
     * in the formatted message. No message is actually output.
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

        // Attempt to parse iteration values.
        std::istringstream iss(buffer);
        int iter;
        double fval, ercf, lcf, ccf;
        if (iss >> iter >> fval >> ercf >> lcf) {
            bool has_ccf = false;
            if (iss >> ccf) {
                has_ccf = true;
            }
            std::lock_guard<std::mutex> lock(iter_mutex_);
            last_values_.iter  = iter;
            last_values_.fval  = fval;
            last_values_.ercf  = ercf;
            last_values_.lcf   = lcf;
            last_values_.ccf   = has_ccf ? ccf : 0.0;
            last_values_.has_ccf = has_ccf;
        }
        // Message is swallowed (no output).
    }

    /**
     * @brief Logs a message with indentation adjustment.
     *
     * This method overrides the base msg() method to capture iteration values if they are present
     * and to adjust the indentation. No message is actually output.
     *
     * @param incr Indentation change.
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

        // Attempt to parse iteration values.
        std::istringstream iss(buffer);
        int iter;
        double fval, ercf, lcf, ccf;
        if (iss >> iter >> fval >> ercf >> lcf) {
            bool has_ccf = false;
            if (iss >> ccf) {
                has_ccf = true;
            }
            std::lock_guard<std::mutex> lock(iter_mutex_);
            last_values_.iter  = iter;
            last_values_.fval  = fval;
            last_values_.ercf  = ercf;
            last_values_.lcf   = lcf;
            last_values_.ccf   = has_ccf ? ccf : 0.0;
            last_values_.has_ccf = has_ccf;
        }
        // Update indentation but do not output anything.
        num_indent_ += incr;
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
