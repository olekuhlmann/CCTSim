#ifndef CCTSIM_JSON_RANGE_HH
#define CCTSIM_JSON_RANGE_HH

#include <vector>
#include <json/json.h>

/**
 * @class JsonRange
 * @brief Class for creating ranges of JSON values.
 */
class JsonRange
{
public:
    /**
     * @brief Create a linear range of doubles.
     * @param start The start of the range (inclusive).
     * @param end The end of the range (inclusive).
     * @param num_steps The number of steps in the range. Must be at least 2 since `start` and `end` are included.
     * @return The linear range of doubles.
     * 
     * Create a linear range of doubles from `start` to `end` with `num_steps` steps. Throws an exception if `num_steps` is less than 2 or if `start` > `end`.
     */
    static std::vector<Json::Value> double_linear(double start, double end, int num_steps) {
        // Check that inputs are valid
        if (num_steps < 2) {
            throw std::invalid_argument("num_steps must be at least 2");
        }
        if (start > end) {
            throw std::invalid_argument("start must be less than or equal to end");
        }
        if (start == end) {
            // Return a vector with the same value repeated num_steps times
            return std::vector<Json::Value>(num_steps, Json::Value(start));
        }

        std::vector<Json::Value> range;
        double step = (end - start) / (num_steps - 1);

        for (int i = 0; i < num_steps; ++i) {
            double value = start + i * step;
            range.push_back(Json::Value(value));
        }

        return range;
    }
};

#endif // CCTSIM_JSON_RANGE_HH