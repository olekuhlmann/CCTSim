#ifndef OUTPUT_MAX_CURVATURE_HH
#define OUTPUT_MAX_CURVATURE_HH

#include "output_criterion_interface.h"
#include "mesh_data_handler.h"
#include "cube3d_factory.hh"
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
        setup();
    }

    /**
     * @brief Construct a new OutputMaxCurvature object with a specified filter cube.
     * @param filter_cube 3D coordinate cube to filter nodes.
     * @param column_suffix (Optional) String to append to the column name. 
     * 
     * Construct a new OutputMaxCurvature object.
     * This object will compute the max curvature in the magnet with respect to the 'magnitude' field component.
     * Only curvature values for nodes inside `filter_cube` will be considered.
     */
    OutputMaxCurvature(Cube3DFactory &filter_cube, std::string column_suffix="") : filter_cube_(filter_cube.getCube())
    {
        setup();
        column_name_ += column_suffix;
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
        double max_curvature;
        if(filter_cube_ != nullptr){
            max_curvature = mesh_data_handler->getMaxCurvature(CCTools::MeshFieldComponent::MAGNITUDE, *filter_cube_);
        } else {
            max_curvature = mesh_data_handler->getMaxCurvature(CCTools::MeshFieldComponent::MAGNITUDE);
        }

        // Return the value
        return max_curvature;
    }

private:
    /**
     * @brief Setup function called by constructors.
     */
    void setup(){
        column_name_ = "max_curvature_magnitude";
        required_calculations_ = {std::type_index(typeid(CCTools::MeshDataHandler))};
    }

    // Optional filter to consider only curvature values from nodes within a the specified cube.
    std::shared_ptr<CCTools::Cube3D> filter_cube_ = nullptr;
};

#endif // OUTPUT_MAX_CURVATURE_HH