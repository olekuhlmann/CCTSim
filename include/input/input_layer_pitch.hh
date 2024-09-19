#ifndef INPUT_LAYER_PITCH_HH
#define INPUT_LAYER_PITCH_HH

#include "input_param_range_interface.h"
#include <json/json.h>

class InputLayerPitch : InputParamRangeInterface {
public:

    /**
     * @brief Construct a InputLayerPitch object.
     * @param value_range The range of pitch [mm].
     * @param JSON_name The 'name' field of the custom CCT object (rat::mdl::pathcctcustom).
     * @param column_name_suffix The suffix to be added to the column name in the output file. Default column name is 'layer_pitch'.
     * 
     * Initialize an InputLayerPitch object to define the parameter range for the pitch of one layer in the CCT.
     */
    InputLayerPitch(std::vector<Json::Value> value_range, std::string JSON_name, std::string column_name_suffix="") {
        column_name_ = "layer_pitch" + column_name_suffix;
        range_ = value_range;

        JSON_name_ = JSON_name;
        JSON_children_ = {"omega"};
        JSON_target_ = "scaling"; //unit of field is m
    }

};

#endif // INPUT_LAYER_PITCH_HH