#include <string>
#include <vector>
#include <constants.h>
#include "input_param_range_interface.h"
#include "input_multipole_scaling.hh"
#include "input_layer_pitch.hh"
#include "input_pathconnectv2_value.hh"
#include "output_criterion_interface.h"
#include "output_b_multipole.hh"
#include "output_a_multipole.hh"
#include "output_max_z.hh"
#include "output_min_z.hh"
#include "output_max_curvature.hh"
#include "parameter_search.h"
#include "model_handler.h"
#include "json_range.hh"

int main()
{

    // Set path to model file
    std::string model_path = DATA_DIR_PATH + "model_edge_regression_test_v1.json";
    CCTools::ModelHandler modelHandler(model_path);

    // Set input parameters
    std::vector<std::shared_ptr<InputParamRangeInterface>> inputs;

    // InputLayerPitch pitch_outer("custom cct outer", {2.05}, "_outer");
    // InputLayerPitch pitch_inner("custom cct inner", JsonRange::double_linear(2,2.2, 5000), "_inner");

    // inputs.push_back(std::make_shared<InputParamRangeInterface>(pitch_outer));
    // inputs.push_back(std::make_shared<InputParamRangeInterface>(pitch_inner));

    // // add the a1 multipole
    // inputs.push_back(std::make_shared<InputParamRangeInterface>(InputMultipoleScaling("a1", "A1", HarmonicScalingFunctionTarget::CONST, JsonRange::double_linear(0.1, 0.2, 5000))));

    inputs.push_back(std::make_shared<InputPathConnectV2Value>(InputPathConnectV2Value("cable out", "end", 4, "w", JsonRange::double_linear(-0.5, 0, 510e3))));


    // Set output parameters
    std::vector<std::shared_ptr<OutputCriterionInterface>> outputs;

    // // Add all a_n
    // for (size_t i = 1; i <= 10; i++)
    // {
    //     outputs.push_back(std::make_shared<OutputAMultipole>(OutputAMultipole(i)));
    // }
    // // Add all b_n
    // for (size_t i = 1; i <= 10; i++)
    // {
    //     outputs.push_back(std::make_shared<OutputBMultipole>(OutputBMultipole(i)));
    // }

    // outputs.push_back(std::make_shared<OutputMinZ>(OutputMinZ()));
    // outputs.push_back(std::make_shared<OutputMaxZ>(OutputMaxZ()));

    Cube3DFactory cube_factory(56, 74, 56, 27, 52, 170);

    outputs.push_back(std::make_shared<OutputMaxCurvature>(OutputMaxCurvature(cube_factory)));

    // Run grid search
    ParameterSearch search(inputs, outputs, modelHandler);
    search.run();

    return 0;
}