#ifndef INPUT_LAYER_PITCH_HH
#define INPUT_LAYER_PITCH_HH

#include "input_param_range_interface.h"
#include <json/json.h>

/**
 * @class InputLayerPitch
 * @brief Class for defining the pitch of one layer in the CCT as an input parameter.
 */
class InputLayerPitch : public InputParamRangeInterface {
public:

    /**
     * @brief Construct a InputLayerPitch object.
     * @param JSON_name The 'name' field of the custom CCT object (rat::mdl::pathcctcustom).
     * @param value_range The range of pitch [mm].
     * @param column_name_suffix The suffix to be added to the column name in the output file. Default column name is 'layer_pitch'.
     * 
     * Initialize an InputLayerPitch object to define the parameter range for the pitch of one layer in the CCT.
     * Values are to be provided in [mm] and will be converted to [m] in the JSON file.
     */
    InputLayerPitch(std::string JSON_name, std::vector<Json::Value> value_range, std::string column_name_suffix="") {
        column_name_ = "layer_pitch" + column_name_suffix;

        JSON_name_ = JSON_name;
        JSON_children_ = {"omega"};
        JSON_target_ = "scaling"; // unit of field is m
        
        range_ = value_range;

        // Convert range to m
        for (size_t i = 0; i < range_.size(); i++) {
            range_[i] = range_[i].asDouble() / 1000.0;
        }
    }

};

#endif // INPUT_LAYER_PITCH_HH