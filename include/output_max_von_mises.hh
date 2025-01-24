#ifndef OUTPUT_MAX_VON_MISES_HH
#define OUTPUT_MAX_VON_MISES_HH

#include "output_criterion_interface.h"
#include "mesh_data_handler.h"
#include <json/json.h>

/**
 * @class OutputMaxVonMises
 * @brief Class for defining the max Von Mises pressure (MPa) in the magnet model as an output criterion.
 */
class OutputMaxVonMises : public OutputCriterionInterface
{
public:
    /**
     * @brief Construct a new OutputMaxVonMises object.
     *
     * Construct a new OutputMaxVonMises object.
     * This object will compute the max Von Mises pressure (MPa) in the magnet model.
     */
    OutputMaxVonMises()
    {
        setup();
    }

    double computeCriterion(std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> calcResults)
    {
        // Assert that the passed calculation result handlers are of the correct type
        if (!checkCalcResultHandlerTypes(calcResults))
        {
            throw std::runtime_error("Calculation result handlers of the wrong type have been passed to the max von mises criterion.");
        }

        // Extract the mesh data handler from the calculation result handlers
        auto mesh_data_handler = std::dynamic_pointer_cast<CCTools::MeshDataHandler>(calcResults[0]);

        // Get the max von mises
        double max_von_mises = mesh_data_handler->getMaxVonMises();

        // Return the value
        return max_von_mises;
    }

private:
    /**
     * @brief Setup function called by constructors.
     */
    void setup(){
        column_name_ = "max_von_mises";
        required_calculations_ = {std::type_index(typeid(CCTools::MeshDataHandler))};
    }
};

#endif // OUTPUT_MAX_VON_MISES_HH