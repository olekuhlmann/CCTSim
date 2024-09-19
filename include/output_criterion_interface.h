#ifndef OUTPUT_CRITERION_INTERFACE_H
#define OUTPUT_CRITERION_INTERFACE_H

#include <vector>
#include <typeindex>
#include <string>
#include <memory>
#include <rat/models/calc.hh>
#include <any>
#include <calc_result_handler_base.h>


/**
 * @interface OutputCriterionInterface
 * @brief Interface for output criteria.
 * 
 * Interface for output criteria. This interface is used to define the output criteria that will be computed for each step of the parameter search.
 */
class OutputCriterionInterface
{
public:

    /**
     * @brief Get the column name of the output criterion.
     * @return The column name of the output criterion as a string.
     * 
     * Get the column name of the output criterion. This is the name of the column in the output file that will contain the values of this criterion.
     */
    virtual std::string getColumnName(){
        return column_name_;
    }

    /**
     * @brief Get the required calculation result handlers for the output criterion.
     * @return The type info of the required calculation result handlers for the output criterion as a type index vector.
     * 
     * Get the type info of the CCTools calculation result handlers (harmonics, mesh, ...) that are required for this criterion.
     * These handlers will be passed to this object in order to compute the value of the output criterion.
     */
    virtual std::vector<std::type_index> getRequiredCalculations(){
        return required_calculations_;
    }

    /**
     * @brief Compute the output criterion.
     * @param calcResults The calculation results required to compute the output criterion.
     * @return The value of the output criterion as a double.
     * 
     * Compute the value of the output criterion. This function will be called by the parameter search object to compute the value of the output criterion for a given set of input parameters.
     * Throws an exception of the types of the calculation results do not match the required calculation result handlers.
     */
    virtual double computeCriterion(std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> calcResults) = 0;

    /**
     * @brief Assert that the calculation result handler types match the required types.
     * @param calcResults The calculation result handlers to be checked.
     * @return True if the types of the calculation result handlers match the required types, false otherwise.
     * 
     * Assert that the types of the calculation result handlers match the required types in the correct order. This function will be called whenever calc result handlers are passed to this object.
     */
    bool checkCalcResultHandlerTypes(std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> calcResults){
        // Check if the sizes of both vectors match
        if (calcResults.size() != required_calculations_.size()) {
            return false;
        }

        // Check if the types of the calculation result handlers match the required types in the correct order
        for (size_t i = 0; i < calcResults.size(); i++){
            if (std::type_index(typeid(*calcResults[i])) != required_calculations_[i]){
                return false;
            }
        }
        return true;
    }

    virtual ~OutputCriterionInterface() = default;

protected:
        
    OutputCriterionInterface() = default;

    std::string column_name_;
    std::vector<std::type_index> required_calculations_;

};

#endif // OUTPUT_CRITERION_INTERFACE_H