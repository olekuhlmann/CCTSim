#ifndef OUTPUT_MAX_CURVATURE_HH
#define OUTPUT_MAX_CURVATURE_HH

#include "output_criterion_interface.h"
#include "mesh_data_handler.h"
#include <json/json.h>

class OutputMaxCurvature : public OutputCriterionInterface
{
public:
    /**
     * @brief Construct a new OutputMaxCurvature object.
     *
     * Construct a new OutputMaxCurvature object. 
     * This object will compute the max curvature in the magnet with respect to the 'magnitude' field component.
     */
    OutputMaxCurvature()
    {
        column_name_ = "max_curvature_magnitude";
        required_calculations_ = {std::type_index(typeid(CCTools::MeshDataHandler))};
    }

    double computeCriterion(std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> calcResults)
    {
        // Assert that the passed calculation result handlers are of the correct type
        if (!checkCalcResultHandlerTypes(calcResults))
        {
            throw std::runtime_error("Calculation result handlers of the wrong type have been passed to the max curvature criterion.");
        }

        // Extract the mesh data handler from the calculation result handlers
        auto mesh_data_handler = std::dynamic_pointer_cast<CCTools::MeshDataHandler>(calcResults[0]);

        // Get the max curvature value
        auto max_curvature = mesh_data_handler->getMaxCurvature(CCTools::MeshFieldComponent::MAGNITUDE);

        // Return the value
        return max_curvature;
    }
};

#endif // OUTPUT_MAX_CURVATURE_HH