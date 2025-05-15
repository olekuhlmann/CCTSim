#ifndef CCTSIM_JSON_RANGE_HH
#define CCTSIM_JSON_RANGE_HH

#include <vector>
#include <json/json.h>
#include <rat/models/pathconnect2.hh>
#include <Logger.hh>

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

    /**
     * @brief Create a vector of random uvw configs for a pathconnect2 node.
     * @param pathconnect2 The pathconnect2 node.
     * @param num_configs The number of configs to create. Must be at least 1.
     * @return The vector of configs.
     * 
     * Create a set of configs from a pathconnect2 node with respect to the lower and upper bounds lb and ub.
     * The first config in the returned vector is the default config used when use_previous is disabled, see `pathconnect2_default_config()`.
     * It is advisable to set `symmetric`, `enable_w` and the number of control points before calling this function as the format of the configs
     * depend on these values.
     */
    static std::vector<Json::Value> pathconnect2_range(rat::mdl::ShPathConnect2Pr pathconnect2, size_t num_configs) {
        // Seed the random number generator with the current time
        srand(static_cast<unsigned int>(time(nullptr)));

        if(num_configs < 1){
            throw std::invalid_argument("num_configs must be greater than 0");
        }

        // vector to be returned
        std::vector<Json::Value> configs;

        // get lb and ub
        std::vector<double> lb = pathconnect2->get_lb();
        std::vector<double> ub = pathconnect2->get_ub();

        // clamp ub to ell/(num_control_points+1) so the sum of u cannot be longer than ell
        unsigned int num_control_points = pathconnect2->get_order() + 1;
        double ell = pathconnect2->get_ell();
        for (size_t i = 0; i < ub.size()-1; i++){ //do not clamp w
            ub[i] = std::min(ub[i], ell / (num_control_points+1));
        }
        // clamp lb to -ell/(num_control_points+1)
        for (size_t i = 0; i < lb.size()-1; i++){ //do not clamp w
            lb[i] = std::max(lb[i], -ell / (num_control_points+1));
        }


        configs.push_back(pathconnect2_default_config(pathconnect2));

        for (size_t i = 1; i < num_configs; ++i) {
            // create a random config
            Json::Value config(Json::arrayValue);
            for (size_t j = 0; j < lb.size(); ++j) {
                double value = lb[j] + static_cast<double>(rand()) / RAND_MAX * (ub[j] - lb[j]);
                config.append(value);
            }
            configs.push_back(config);
        }

        return configs;
    }

    /**
     * @brief Returns the default x configuration for a pathconnect2 node.
     * @param pathconnect2 The pathconnect2 node.
     * @return The default x configuration in JSON.
     * 
     * Returns the default x configuration for a pathconnect2 node. This is the configuration used when use_previous is disabled.
     */
    static Json::Value pathconnect2_default_config(rat::mdl::ShPathConnect2Pr pathconnect2){
        // Retrieve necessary parameters from the node.
        arma::uword order = pathconnect2->get_order();
        double ell = pathconnect2->get_ell();
    
        // Initialize uvw matrices with zeros (3 rows, order+1 columns).
        arma::Mat<double> uvw1(3, order + 1, arma::fill::zeros);
        arma::Mat<double> uvw2(3, order + 1, arma::fill::zeros);
    
        // Set the first row to be a linspace from 0 to ell/4.
        uvw1.row(0) = arma::linspace<arma::Row<double>>(0.0, ell / 4.0, order + 1);
        uvw2.row(0) = arma::linspace<arma::Row<double>>(0.0, ell / 4.0, order + 1);
    
        // For indices 4 to order (i.e. columns 4..order), set the second row.
        // Before index 4 the values remain zero.
        for (arma::uword i = 4; i < order + 1; ++i) {
            uvw1(1, i) = i * ell / 24.0;
            uvw2(1, i) = i * ell / 24.0;
        }
        // Note: The third row (index 2) remains zero.
    
        // Update the pathconnect2 node with these default uvw configurations.
        pathconnect2->set_uvw1(uvw1);
        pathconnect2->set_uvw2(uvw2);
    
        // Now, obtain the configuration vector (x0) that the node would use,
        // which is computed as the differences between consecutive control points.
        std::vector<double> x0 = pathconnect2->get_x0();
    
        // Convert the vector x0 into a JSON array, matching the format used in pathconnect2_range.
        Json::Value config(Json::arrayValue);
        for (const double val : x0) {
            config.append(val);
        }
    
        return config;
    }
};

#endif // CCTSIM_JSON_RANGE_HH