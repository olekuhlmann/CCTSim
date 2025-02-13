#ifndef INPUT_PATHCONNECTV2_UVW_HH
#define INPUT_PATHCONNECTV2_UVW_HH

#include "input_param_range_interface.h"
#include <json/json.h>
#include <rat/models/pathconnect2.hh>

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
     * @param x_configs The x vectors of the uvw configurations to be applied.
     * @param pathconnect2 The pathconnect2 node to be configured.
     * @param column_name_suffix The suffix to be added to the column name in the output file. Default column name is 'pathconnect2_uvw'.
     *
     * Initialize an InputPathConnectV2UVW object to define the uvw1 and uvw2 configurations of a pathconnect2 node.
     * Format for items of `x_configs`, see see rat::mdl::PathConnect2:get_x0().
     * Values of `x_configs` are to be provided in [m].
     * Optimization variables (e.g., `is_symmetric`, `enable_w`) will be taken from the pathconnect2 node, thus it must be fully configured beforehand.
     */
    InputPathConnectV2UVW(std::string JSON_name, const std::vector<Json::Value> &x_configs, rat::mdl::ShPathConnect2Pr pathconnect2, std::string column_name_suffix = "")
    {
        column_name_ = "pathconnect2_uvw" + column_name_suffix;

        JSON_name_ = JSON_name;
        // JSON children and JSON target variables are not used for this parameter
        // set to dummy values so that checkInputParams() can verify the JSON path
        JSON_children_ = {};
        JSON_target_ = "uvw1";

        // for this parameter, range_ does not contain the final Json Value that can be applied.
        // before applying, the configuration must be converted to the correct JSON format
        range_ = x_configs;

        num_control_points_ = pathconnect2->get_order() + 1;
        is_symmetric_ = pathconnect2->get_is_symmetric();
        enable_w_ = pathconnect2->get_enable_w();
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

    std::string getConfigAsString(Json::Value value) override
    {
        // the value is an array here, convert to string
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

private:
    /**
     * @brief Convert a uvw configuration to the JSON format.
     * @param x The x vector to be converted.
     * @returns The JSON objects for uvw1 and uvw2 in a pair.
     *
     * Converts a x vector (format: see rat::mdl::PathConnect2:get_x0()) as Json::Value to JSON values for 'uvw1' and 'uvw2'.
     * JSON format: ([{"u:" ..., "v": ..., "w": ...}]).
     * This configuration is to be provided in [mm] and will be converted to [m] in the JSON file.
     */
    std::pair<Json::Value, Json::Value> convertConfig(const Json::Value &x)
    {
        unsigned int num_vars = x.size();
        if (num_vars == 0)
        {
            throw std::invalid_argument("x cannot be empty");
        }

        // JSON array for the configurations
        Json::Value uvw1_config(Json::arrayValue);
        Json::Value uvw2_config(Json::arrayValue);

        // fill uvw1 and uvw2 with zeros
        for (unsigned int i = 0; i < num_control_points_; i++)
        {
            Json::Value point1;
            point1["u"] = 0.0;
            point1["v"] = 0.0;
            point1["w"] = 0.0;

            Json::Value point2;
            point2["u"] = 0.0;
            point2["v"] = 0.0;
            point2["w"] = 0.0;

            uvw1_config.append(point1);
            uvw2_config.append(point2);
        }

        // fill in uvw values from the x vector analogoue to rat::mdl::PathConnect2:set_uvw()
        Json::Value::ArrayIndex cnt = 0;

        for (Json::Value::ArrayIndex i = 1; i < 4; i++)
        {
            uvw1_config[i]["u"] = uvw1_config[i - 1]["u"].asDouble() + x[cnt++].asDouble();
        }

        for (Json::Value::ArrayIndex i = 4; i < num_control_points_; i++)
        {
            uvw1_config[i]["u"] = uvw1_config[i - 1]["u"].asDouble() + x[cnt++].asDouble();
        }

        for (Json::Value::ArrayIndex i = 4; i < num_control_points_; i++)
        {
            uvw1_config[i]["v"] = uvw1_config[i - 1]["v"].asDouble() + x[cnt++].asDouble();
        }

        if (enable_w_)
        {
            for (Json::Value::ArrayIndex i = 7; i < num_control_points_; i++)
            {
                uvw1_config[i]["w"] = uvw1_config[i - 1]["w"].asDouble() + x[cnt++].asDouble();
            }
        }

        if (!is_symmetric_)
        {
            for (Json::Value::ArrayIndex i = 1; i < 4; i++)
            {
                uvw2_config[i]["u"] = uvw2_config[i - 1]["u"].asDouble() + x[cnt++].asDouble();
            }

            for (Json::Value::ArrayIndex i = 4; i < num_control_points_; i++)
            {
                uvw2_config[i]["u"] = uvw2_config[i - 1]["u"].asDouble() + x[cnt++].asDouble();
            }

            for (Json::Value::ArrayIndex i = 4; i < num_control_points_; i++)
            {
                uvw2_config[i]["v"] = uvw2_config[i - 1]["v"].asDouble() + x[cnt++].asDouble();
            }

            if (enable_w_)
            {
                for (Json::Value::ArrayIndex i = 7; i < num_control_points_; i++)
                {
                    uvw2_config[i]["w"] = uvw2_config[i - 1]["w"].asDouble() + x[cnt++].asDouble();
                }
            }
        }
        else
        {
            uvw2_config = uvw1_config;
        }

        if(cnt != num_vars)
        {
            throw std::invalid_argument("x vector has incorrect number of elements");
        }

        return std::make_pair(uvw1_config, uvw2_config);
    }

    /**
     * @brief The number of control points for uvw1 (same for uvw2).
     */
    unsigned int num_control_points_;

    /**
     * @brief The `is_symmetric` flag of the pathconnect2 node.
     */
    bool is_symmetric_;

    /**
     * @brief The `enable_w` flag of the pathconnect2 node.
     */
    bool enable_w_;
};

#endif // INPUT_PATHCONNECTV2_UVW_HH