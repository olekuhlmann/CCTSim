#ifndef OUTPUT_PATHCONNECV2_STRAIN_ENERGY_HH
#define OUTPUT_PATHCONNECV2_STRAIN_ENERGY_HH

#include "output_criterion_interface.h"
#include <model_calculator.h>
#include <json/json.h>
#include <rat/models/pathconnect2.hh>
#include <rat/models/path.hh>
#include "CustomIterationLog.hh"

using CCTools::Logger;

/**
 * @class OutputPathConnectV2StrainEnergy
 * @brief Class for objective function (integrated strain energy) of a pathconnect2 optimizer as an output criterion.
 */
class OutputPathConnectV2StrainEnergy : public OutputCriterionInterface
{
public:
    /**
     * @brief Construct a new OutputPathConnectV2StrainEnergy object.
     * @param model_calculator The model calculator.
     * @param findConnectV2 The function to find the pathconnect2 node in the model tree.
     * @param column_suffix (Optional) String to append to the column name. Default column name is 'pathconnect2_strain_energy'.
     *
     * Construct a new OutputPathConnectV2StrainEnergy object.
     * This object will compute the objective function (integrated strain energy) of a pathconnect2 optimizer.
     * For every output calculation, the model calculator will be re-loaded to extract the current model tree and
     * the pathconnect2 node will be found in the model tree using the `findConnectV2` function.
     */
    OutputPathConnectV2StrainEnergy(CCTools::ModelCalculator model_calculator, rat::mdl::ShPathConnect2Pr (*findConnectV2)(rat::mdl::ShModelGroupPr), std::string column_suffix="") : model_calculator_(model_calculator), findConnectV2_(findConnectV2)
    {
        column_name_ = "pathconnect2_strain_energy" + column_suffix;
        required_calculations_ = {};
    }


    double computeCriterion(std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> calcResults)
    {
        // Assert that the passed calculation result handlers are of the correct type
        if (!checkCalcResultHandlerTypes(calcResults))
        {
            throw std::runtime_error("Calculation result handlers of the wrong type have been passed to the pathconnect2 strain energy criterion.");
        }

        // Reload the model calculator and extract the connectV2
        model_calculator_.reload();
        rat::mdl::ShModelGroupPr model_tree = model_calculator_.get_model_tree();
        rat::mdl::ShPathConnect2Pr connectV2 = findConnectV2_(model_tree);

        // set use_previous to true so that the strain energy is computed with configuration set in the input param range
        connectV2->set_use_previous(true);

        // Create custom logger. Does not log anything but captures the last iteration values.
        std::shared_ptr<CustomIterationLog> custom_logger = std::make_shared<CustomIterationLog>(); 

        // call the optimization
        connectV2->optimize_control_points(custom_logger);

        // get all values from the last iteration
        CustomIterationLog::IterationValues last_values = custom_logger->get_last_iteration_values();

        double strain_energy = last_values.fval;
        double edge_regression_constraint_value = last_values.ercf;
        double length_constraint_value = last_values.lcf;
        double curvature_constraint_value = last_values.has_ccf ? last_values.ccf : 0.0;

        // Log all values
        Logger::info("Strain energy: " + std::to_string(strain_energy));
        Logger::info("Edge regression constraint: " + std::to_string(edge_regression_constraint_value));
        Logger::info("Length constraint: " + std::to_string(length_constraint_value));
        Logger::info("Curvature constraint: " + std::to_string(curvature_constraint_value));

        // return fval
        return strain_energy;
    }

private:
    CCTools::ModelCalculator model_calculator_;
    rat::mdl::ShPathConnect2Pr (*findConnectV2_)(rat::mdl::ShModelGroupPr);
};

#endif // OUTPUT_PATHCONNECV2_STRAIN_ENERGY_HH