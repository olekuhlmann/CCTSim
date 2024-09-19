#ifndef OUTPUT_A_MULTIPOLE_HH
#define OUTPUT_A_MULTIPOLE_HH

#include "output_criterion_interface.h"
#include "harmonics_data_handler.h"
#include <json/json.h>

class OutputAMultipole : OutputCriterionInterface {
public:

    /**
     * @brief Construct a new OutputAMultipole object.
     * @param n_poles The number of the poles for the A component (1 to 10).
     * 
     * Construct a new OutputAMultipole object. This object will compute the a_n value of the A_n multipole for the specified pole.
     */
    OutputAMultipole(size_t n_poles){
        n_poles_ = n_poles;
        column_name_ = "a" + std::to_string(n_poles);


        required_calculations_ = {std::type_index(typeid(CCTools::HarmonicsDataHandler))};
    }

    double OutputAMultipole::computeCriterion(std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> calcResults){
        // Assert that the passed calculation result handlers are of the correct type
        if (!checkCalcResultHandlerTypes(calcResults)){
            throw std::runtime_error("Calculation result handlers of the wrong type have been passed to the output A multipole criterion.");
        }

        // Extract the harmonics data handler from the calculation result handlers
        auto harmonics_data_handler = std::dynamic_pointer_cast<CCTools::HarmonicsDataHandler>(calcResults[0]);

        // Get the a_n values
        auto an = harmonics_data_handler->get_an();

        // Return the a_n value for the specified pole
        double value = an[n_poles_ - 1];

        return value;
    }

private:
    size_t n_poles_;

};

#endif // OUTPUT_A_MULTIPOLE_HH