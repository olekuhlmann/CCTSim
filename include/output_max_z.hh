#ifndef OUTPUT_MAX_Z_HH
#define OUTPUT_MAX_Z_HH

#include "output_criterion_interface.h"
#include "mesh_data_handler.h"
#include <json/json.h>

/**
 * @class OutputMaxZ
 * @brief Class for defining the maximum z coordinate of the model as an output criterion.
 */
class OutputMaxZ : public OutputCriterionInterface
{
public:
    /**
     * @brief Construct a new OutputMaxZ object.
     *
     * Construct a new OutputMaxZ object. This object will compute maximum z coordinate of the model.
     */
    OutputMaxZ()
    {
        column_name_ = "z_max";
        required_calculations_ = {std::type_index(typeid(CCTools::MeshDataHandler))};
    }

    double computeCriterion(std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> calcResults)
    {
        // Assert that the passed calculation result handlers are of the correct type
        if (!checkCalcResultHandlerTypes(calcResults))
        {
            throw std::runtime_error("Calculation result handlers of the wrong type have been passed to the max Z criterion.");
        }

        // Extract the mesh data handler from the calculation result handlers
        auto mesh_data_handler = std::dynamic_pointer_cast<CCTools::MeshDataHandler>(calcResults[0]);

        // Get the max_z value
        auto max_z = mesh_data_handler->getMinMaxZValues().second;

        // Return the value
        return max_z;
    }
};

#endif // OUTPUT_MAX_Z_HH