#ifndef INPUT_PARAM_RANGE_INTERFACE_H
#define INPUT_PARAM_RANGE_INTERFACE_H

#include <vector>
#include <string>
#include <typeindex>
#include <json/json.h>

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
     * @brief Get the column name of the input range.
     * @return The column name of the as a string.
     * 
     * Get the column name. This is the name of the column in the output file that will contain the parameter values of this criterion.
     */
    virtual std::string getColumnName(){
        return column_name_;
    }


    /**
     * @brief Get the range of the input parameter.
     * @return The range of the input parameter as a Json::Value vector.
     * 
     * Get the range of the input parameter as a Json::Value vector to be put into the respective JSON field for this parameter.
     */
    virtual std::vector<Json::Value> getRange(){
        return range_;
    }

    /**
     * @brief Get the JSON name for the input parameter.
     * @return The JSON name for the input parameter as a string.
     * 
     * The location of this input parameter in a JSON file is characterized by the JSON name, JSON children and JSON target.
     * To find the location, the model handler will search for a node in the JSON tree with the specified name, than traverse the children and 
     * find the JSON target property of the last child.
     * 
     * E.g., name="Inner Layer", children={"rho"}, target="radius" specifies the 'radius' property of a node called 'rho', which is a child of a node with the 'name' property 'Inner Layer'.
     */
    virtual std::string getJSONName(){
        return JSON_name_;
    }

    /**
     * @brief Get the JSON children to locate the input parameter.
     * @return The JSON children for the  input parameter as a vector of strings.
     * 
     * The location of this input parameter in a JSON file is characterized by the JSON name, JSON children and JSON target.
     * To find the location, the model handler will search for a node in the JSON tree with the specified name, than traverse the children and 
     * find the JSON target property of the last child.
     * 
     * E.g., name="Inner Layer", children={"rho"}, target="radius" specifies the 'radius' property of a node called 'rho', which is a child of a node with the 'name' property 'Inner Layer'.
     */
    virtual std::vector<std::string> getJSONChildren(){
        return JSON_children_;
    }

    /**
     * @brief Get the JSON target for the input parameter.
     * @return The JSON target for the input parameter as a string.
     * 
     * The location of this input parameter in a JSON file is characterized by the JSON name, JSON children and JSON target.
     * To find the location, the model handler will search for a node in the JSON tree with the specified name, than traverse the children and 
     * find the JSON target property of the last child.
     * 
     * E.g., name="Inner Layer", children={"rho"}, target="radius" specifies the 'radius' property of a node called 'rho', which is a child of a node with the 'name' property 'Inner Layer'.
     */
    virtual std::string getJSONTarget(){
        return JSON_target_;
    }

    virtual ~InputParamRangeInterface() = default;

protected:
    
        InputParamRangeInterface() = default;

        std::string column_name_;
        std::vector<Json::Value> range_;
        std::string JSON_name_;
        std::vector<std::string> JSON_children_;
        std::string JSON_target_;

};

#endif // INPUT_PARAM_RANGE_INTERFACE_H