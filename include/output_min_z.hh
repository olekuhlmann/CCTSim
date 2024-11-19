#ifndef OUTPUT_MIN_Z_HH
#define OUTPUT_MIN_Z_HH

#include "output_criterion_interface.h"
#include "mesh_data_handler.h"
#include <json/json.h>

/**
 * @class OutputMinZ
 * @brief Class for defining the minimum z coordinate of the model as an output criterion.
 */
class OutputMinZ : public OutputCriterionInterface
{
public:
    /**
     * @brief Construct a new OutputMinZ object.
     *
     * Construct a new OutputMinZ object. This object will compute minimum z coordinate of the model.
     */
    OutputMinZ()
    {
        column_name_ = "z_min";
        required_calculations_ = {std::type_index(typeid(CCTools::MeshDataHandler))};
    }

    double computeCriterion(std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> calcResults)
    {
        // Assert that the passed calculation result handlers are of the correct type
        if (!checkCalcResultHandlerTypes(calcResults))
        {
            throw std::runtime_error("Calculation result handlers of the wrong type have been passed to the min Z criterion.");
        }

        // Extract the mesh data handler from the calculation result handlers
        auto mesh_data_handler = std::dynamic_pointer_cast<CCTools::MeshDataHandler>(calcResults[0]);

        // Get the min_z value
        auto min_z = mesh_data_handler->getMinMaxZValues().first;

        // Return the value
        return min_z;
    }
};

#endif // OUTPUT_MIN_Z_HH