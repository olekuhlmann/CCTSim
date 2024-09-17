#ifndef INPUT_PARAM_RANGE_INTERFACE_H
#define INPUT_PARAM_RANGE_INTERFACE_H

#include <vector>
#include <string>
#include <typeindex>

/**
 * @interface InputParamRangeInterface
 * @brief Interface for input parameter ranges.
 * 
 * Interface for input parameter ranges. This interface is used to define the range of an input parameter, the required calculations for this parameter, and the identifier of the parameter.
 * The parameter ranges will be used to run the parameter search. 
 */
class InputParamRangeInterface
{

public:

    /**
     * @brief Get the range of the input parameter.
     * @return The range of the input parameter as a string vector.
     * 
     * Get the range of the input parameter as a string vector to be put into the respective JSON field for this parameter.
     */
    virtual std::vector<std::string> getRange(){
        return range_;
    }

    /**
     * @brief Get the identifier of the input parameter.
     * @return The identifier of the input parameter as a string.
     * 
     * Get the identifier of the input parameter. This is either the name of a JSON field or the type of the JSON field, depending on the `isIdentifiableByName` function.
     */
    virtual std::string getIdentifier(){
        return identifier_;
    }

    /**
     * @brief Check if the input parameter is identifiable by name.
     * @return True if the input parameter is identifiable by name, false otherwise.
     * 
     * Check if the input parameter is identifiable by name. If true, the identifier is the name of the JSON field. If false, the identifier is the type of the JSON field.
     */
    virtual bool isIdentifiableByName(){
        return is_identifiable_by_name_;
    }

    virtual ~InputParamRangeInterface() = default;

protected:
    
        InputParamRangeInterface() = default;

        std::vector<std::string> range_;
        std::string identifier_;
        bool is_identifiable_by_name_;        

};

#endif // INPUT_PARAM_RANGE_INTERFACE_H