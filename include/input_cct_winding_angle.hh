#ifndef INPUT_CCT_WINDING_ANGLE
#define INPUT_CCT_WINDING_ANGLE

#include "input_param_range_interface.h"
#include <json/json.h>

/**
 * @class InputCCTWindingAngle
 * @brief Class for defining the CCT winding angle in the CCT as an input parameter.
 */
class InputCCTWindingAngle : public InputParamRangeInterface {
public:

    /**
     * @brief Construct a InputCCTWindingAngle object.
     * @param JSON_name The 'name' field of the custom CCT object (rat::mdl::pathcctcustom).
     * @param value_range The range of the winding angle (deg)
     * @param column_name_suffix The suffix to be added to the column name in the output file. Default column name is 'cct_winding_angle'.
     * 
     * Initialize an InputCCTWindingAngle object to define the parameter range for the CCT winding angle of one layer in the CCT.
     * Values are to be provided in [deg] and will be converted to [rad] in the JSON file.
     */
    InputCCTWindingAngle(std::string JSON_name, std::vector<Json::Value> value_range, std::string column_name_suffix="") {
        column_name_ = "cct_winding_angle" + column_name_suffix;

        JSON_name_ = JSON_name;
        JSON_children_ = {"rho"};
        JSON_target_ = "alpha"; // unit of field is rad
        
        range_ = value_range;

        // Convert range to rad
        for (size_t i = 0; i < range_.size(); i++) {
            range_[i] = range_[i].asDouble() * PI / 180.0;
        }
    }

private:
    static constexpr double PI = 3.14159265358979323846;
};

#endif // INPUT_CCT_WINDING_ANGLE