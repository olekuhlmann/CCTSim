#ifndef INPUT_PATHCONNECTV2_UVW_HH
#define INPUT_PATHCONNECTV2_UVW_HH

#include "input_param_range_interface.h"
#include <json/json.h>

/**
 * @class InputPathConnectV2UVW
 * @brief Class for defining the uvw1 and uvw2 parameters of a pathconnect2 node as the input parameter.
 */
class InputPathConnectV2UVW : public InputParamRangeInterface
{
public:
    /**
     * @brief Construct a InputPathConnectV2UVW object.
     * @param JSON_name The 'name' field of the pathconnect2 node (rat::mdl::pathconnect2).
     * @param uvw_configs The configurations to be applied.
     * @param column_name_suffix The suffix to be added to the column name in the output file. Default column name is 'pathconnect2_uvw'.
     *
     * Initialize an InputPathConnectV2UVW object to define the uvw1 and uvw2 configurations of a pathconnect2 node.
     * Format for items of `uvw_configs` is as follows: [uvw1(0, u), uvw1(0, v), uvw1(0, w), ... uvw1(n, w), uvw2(0, u), uvw2(0, v), uvw2(0, w), ... uvw2(n, w)]
     * Values are to be provided in [mm] and will be converted to [m] in the JSON file.
     */
    InputPathConnectV2UVW(std::string JSON_name, const std::vector<Json::Value> &uvw_configs, std::string column_name_suffix = "")
    {
        column_name_ = "pathconnect2_uvw" + column_name_suffix;

        JSON_name_ = JSON_name;
        // JSON children and JSON target variables are not used for this parameter

        // for this parameter, range_ does not contain the final Json Value that can be applied.
        // before applying, the configuration must be converted to the correct JSON format
        range_ = uvw_configs;
    }

    void applyParamConfig(CCTools::ModelHandler &model_handler, Json::Value value) override
    {
        // convert the config to JSON
        auto [uvw1_config, uvw2_config] = convertConfig(value);

        // apply uvw1
        model_handler.setValueByName(getJSONName(), {}, "uvw1", uvw1_config);

        // apply uvw2
        model_handler.setValueByName(getJSONName(), {}, "uvw2", uvw2_config);
    }

private:
    /**
     * @brief Convert a uvw configuration to the JSON format.
     * @param uvw_config The configuration to be converted.
     * @returns The JSON objects for uvw1 and uvw2 in a pair.
     *
     * Converts a single uvw configuration (format: [uvw1(0, u), uvw1(0, v), uvw1(0, w), ... uvw1(n, w), uvw2(0, u), uvw2(0, v), uvw2(0, w), ... uvw2(n, w)] as Json::Value)
     * to the JSON format to be put into the respective JSON field for these parameters ([{"u:" ..., "v": ..., "w": ...}]).
     * This configuration is to be provided in [mm] and will be converted to [m] in the JSON file.
     */
    std::pair<Json::Value, Json::Value> convertConfig(const Json::Value &uvw_config)
    {
        if (uvw_config.size() == 0)
        {
            throw std::invalid_argument("uvw_config cannot be empty");
        }

        // 3 values for each control point, same number of control points for uvw1 and uvw2
        if (uvw_config.size() % 6 != 0)
        {
            throw std::invalid_argument("uvw_config must have a length that is a multiple of 6");
        }

        // get number of control points
        size_t num_control_points = uvw_config.size() / 6;

        // convert from mm to m
        double scaling_inverse = 1000.0;

        // JSON array for the configurations
        Json::Value uvw1_config(Json::arrayValue);
        Json::Value uvw2_config(Json::arrayValue);

        for (Json::Value::ArrayIndex j = 0; j < num_control_points; j++)
        {
            // Get values and convert to m
            Json::Value u1 = uvw_config[j * 3].asDouble() / scaling_inverse;
            Json::Value v1 = uvw_config[j * 3 + 1].asDouble() / scaling_inverse;
            Json::Value w1 = uvw_config[j * 3 + 2].asDouble() / scaling_inverse;
            Json::Value u2 = uvw_config[static_cast<Json::Value::ArrayIndex>(num_control_points * 3 + j * 3)].asDouble() / scaling_inverse;
            Json::Value v2 = uvw_config[static_cast<Json::Value::ArrayIndex>(num_control_points * 3 + j * 3 + 1)].asDouble() / scaling_inverse;
            Json::Value w2 = uvw_config[static_cast<Json::Value::ArrayIndex>(num_control_points * 3 + j * 3 + 2)].asDouble() / scaling_inverse;

            // Create a JSON object for the first set of values (for uvw1)
            Json::Value point1;
            point1["u"] = u1;
            point1["v"] = v1;
            point1["w"] = w1;

            // Create a JSON object for the second set of values (for uvw2)
            Json::Value point2;
            point2["u"] = u2;
            point2["v"] = v2;
            point2["w"] = w2;

            // Append the JSON objects to the current configuration array
            uvw1_config.append(point1);
            uvw2_config.append(point2);
        }

        return std::make_pair(uvw1_config, uvw2_config);
    }
};

#endif // INPUT_PATHCONNECTV2_UVW_HH