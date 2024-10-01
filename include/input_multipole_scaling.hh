#ifndef INPUT_MULTIPOLE_SCALING
#define INPUT_MULTIPOLE_SCALING

#include "input_param_range_interface.h"
#include <json/json.h>


/**
 * @enum HarmonicScalingFunctionTarget
 * @brief Enum for the type of the targeted scaling function value for custom CCT harmonics.
 * 
 * Enum to specify the type of the targeted scaling function value for custom CCT harmonics. For harmonics with an amplitude of 'const', the target is the single constant value.
 * For harmonics with an amplitude of 'linear', the target can be the offset or slope value of the scaling function.
 */
enum HarmonicScalingFunctionTarget {
    CONST,
    LINEAR_OFFSET,
    LINEAR_SLOPE
};

class InputMultipoleScaling : public InputParamRangeInterface {
public:

    /**
     * @brief Construct a InputMultipoleScaling object.
     * @param multipole The type of multipole in the format 'a1', 'b10', etc.
     * @param JSON_name The 'name' field of the custom CCT harmonic (rat::mdl::cctharmonicdrive).
     * @param scaling_function_target The targeted scaling function value of the custom harmonic.
     * @param value_range The range of the targeted value [m] or [m/coil].
     * @param column_name_suffix The suffix to be added to the column name in the output file. Default column name is the value of `multipole`.
     * 
     * Initialize an InputMultipoleScaling object to define the parameter range for a scaling function value of a custom CCT harmonic.
     * Values are to be provided in [m] or [m/coil], depending on the type of target.
     */
    InputMultipoleScaling(std::string multipole, std::string JSON_name, HarmonicScalingFunctionTarget scaling_function_target, std::vector<Json::Value> value_range, std::string column_name_suffix="") {
        column_name_ = multipole + column_name_suffix;

        JSON_name_ = JSON_name;
        JSON_children_ = {"harmonic_drive"};

        switch (scaling_function_target) {
            case CONST:
            JSON_target_ = "scaling";
            break;
            case LINEAR_OFFSET:
            JSON_target_ = "offset";
            break;
            case LINEAR_SLOPE:
            JSON_target_ = "slope";
            break;
            default:
            throw std::invalid_argument("Invalid HarmonicScalingFunctionTarget value");
        }
        
        range_ = value_range;
    }

};

#endif // INPUT_MULTIPOLE_SCALING