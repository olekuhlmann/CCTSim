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
    std::string model_path =  "../examples/cct.json";

    // Create model handler
    CCTools::ModelHandler modelHandler(model_path);

    // Set input parameters

    std::vector<std::shared_ptr<InputParamRangeInterface>> inputs;

    // add the b1 multipole
    inputs.push_back(std::make_shared<InputParamRangeInterface>(InputMultipoleScaling("b1", "B1", HarmonicScalingFunctionTarget::CONST, JsonRange::double_linear(0.1, 0.2, 500))));


    // Set output parameters

    std::vector<std::shared_ptr<OutputCriterionInterface>> outputs;

    // Add all a_n
    for (size_t i = 1; i <= 10; i++)
    {
        outputs.push_back(std::make_shared<OutputAMultipole>(OutputAMultipole(i)));
    }
    // Add all b_n
    for (size_t i = 1; i <= 10; i++)
    {
        outputs.push_back(std::make_shared<OutputBMultipole>(OutputBMultipole(i)));
    }

    // Run grid search
    ParameterSearch search(inputs, outputs, modelHandler);
    search.run();

    return 0;
}