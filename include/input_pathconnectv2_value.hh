#ifndef INPUT_PATHCONNECTV2_VALUE_HH
#define INPUT_PATHCONNECTV2_VALUE_HH

#include "input_param_range_interface.h"
#include <json/json.h>

class InputPathConnectV2Value : public InputParamRangeInterface
{
public:
    /**
     * @brief Construct a InputPathConnectV2Value object.
     * @param JSON_name The 'name' field of the Path ConnectV2 object (rat::mdl::pathconnect2).
     * @param control_point_group Group of desired control point. Either "start" or "end".
     * @param control_point_id Id of the desired control point within the group.
     * @param control_point_dimension Desired dimension within control point. Either "u", "v", or "w".
     * @param value_range The range of value [mm].
     * @param column_name_suffix The suffix to be added to the column name in the output file. Default column name is 'pathconnectv2_`control_point_group`_`control_point_id`_`control_point_dimension`'.
     *
     * Initialize an InputPathConnectV2Value object to define the parameter range for a value of a path connectv2 control point.
     * Values are to be provided in [mm] and will be converted to [m] in the JSON file.
     */
    InputPathConnectV2Value(std::string JSON_name, std::string control_point_group, size_t control_point_id, std::string control_point_dimension, std::vector<Json::Value> value_range, std::string column_name_suffix = "")
    {
        column_name_ = "pathconnectv2_" + control_point_group + "_" + std::to_string(control_point_id) + "_" + control_point_dimension + column_name_suffix;

        JSON_name_ = JSON_name;

        // add point group to children
        if(control_point_group == "start"){
            JSON_children_ = {"uvw1"};
        } else if (control_point_group == "end"){
            JSON_children_ = {"uvw2"};
        } else {
            throw std::invalid_argument("control_point_group must be either 'start or 'end'"); 
        }

        // add point index to children
        JSON_children_.emplace_back(static_cast<Json::ArrayIndex>(control_point_id));

        // Set dimension as target
        if (control_point_dimension != "u" && control_point_dimension != "v" && control_point_dimension != "w")
        {
            throw std::invalid_argument("control_point_dimension must be either 'u', 'v', or 'w'");
        }
        JSON_target_ = control_point_dimension; // unit of field is m

        range_ = value_range;

        // Convert range to m
        for (size_t i = 0; i < range_.size(); i++)
        {
            range_[i] = range_[i].asDouble() / 1000.0;
        }
    }
};

#endif // INPUT_PATHCONNECTV2_VALUE_HH