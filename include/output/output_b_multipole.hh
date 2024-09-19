#ifndef OUTPUT_B_MULTIPOLE_HH
#define OUTPUT_B_MULTIPOLE_HH

#include "output_criterion_interface.h"
#include "harmonics_data_handler.h"
#include <json/json.h>

class OutputBMultipole : OutputCriterionInterface {
public:

    /**
     * @brief Construct a new OutputBMultipole object.
     * @param n_poles The number of the poles for the B component (1 to 10).
     * 
     * Construct a new OutputBMultipole object. This object will compute the b_n value of the B_n multipole for the specified pole.
     */
    OutputBMultipole(size_t n_poles){
        n_poles_ = n_poles;
        column_name_ = "b" + std::to_string(n_poles);


        required_calculations_ = {std::type_index(typeid(CCTools::HarmonicsDataHandler))};
    }

    double OutputBMultipole::computeCriterion(std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> calcResults){
        // Assert that the passed calculation result handlers are of the correct type
        if (!checkCalcResultHandlerTypes(calcResults)){
            throw std::runtime_error("Calculation result handlers of the wrong type have been passed to the output B multipole criterion.");
        }

        // Extract the harmonics data handler from the calculation result handlers
        auto harmonics_data_handler = std::dynamic_pointer_cast<CCTools::HarmonicsDataHandler>(calcResults[0]);

        // Get the b_n values
        auto bn = harmonics_data_handler->get_bn();

        // Return the b_n value for the specified pole
        double value = bn[n_poles_ - 1];

        return value;
    }

private:
    size_t n_poles_;

};

#endif // OUTPUT_B_MULTIPOLE_HH