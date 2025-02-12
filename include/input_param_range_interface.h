#ifndef INPUT_PARAM_RANGE_INTERFACE_H
#define INPUT_PARAM_RANGE_INTERFACE_H

#include <vector>
#include <string>
#include <typeindex>
#include <json/json.h>
#include <model_handler.h>

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
     * @brief Apply a parameter configuration.
     * @param model_handler The model handler.
     * @param value The value to be applied. Usually an element from `range_`.
     * 
     * Apply the parameter configuration to the model handler. This will set the value of the parameter in the model handler.
     */
    virtual void applyParamConfig(CCTools::ModelHandler &model_handler, Json::Value value){
        // Get JSON location of the target
        std::string JSON_name = getJSONName();
        std::vector<CCTools::JSONChildrenIdentifierType> JSON_children = getJSONChildren();
        CCTools::JSONChildrenIdentifierType JSON_target = getJSONTarget();

        // Apply the parameter configuration
        model_handler.setValueByName(JSON_name, JSON_children, JSON_target, value);
    }

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
     * setValueByName("Connect South V2", {"uvw1", 0}, "u", 0.1) will find "Connect South V2", traverse the children "uvw1", traverse to the first element of its array and set the 'u' value.
     */
    virtual std::string getJSONName(){
        return JSON_name_;
    }

    /**
     * @brief Get the JSON children to locate the input parameter.
     * @return The JSON children for the  input parameter as a vector of CCTools::JSONChildrenIdentifierType.
     * 
     * The location of this input parameter in a JSON file is characterized by the JSON name, JSON children and JSON target.
     * To find the location, the model handler will search for a node in the JSON tree with the specified name, than traverse the children and 
     * find the JSON target property of the last child.
     * 
     * E.g., name="Inner Layer", children={"rho"}, target="radius" specifies the 'radius' property of a node called 'rho', which is a child of a node with the 'name' property 'Inner Layer'.
     * setValueByName("Connect South V2", {"uvw1", 0}, "u", 0.1) will find "Connect South V2", traverse the children "uvw1", traverse to the first element of its array and set the 'u' value.
     */
    virtual std::vector<CCTools::JSONChildrenIdentifierType> getJSONChildren(){
        return JSON_children_;
    }

    /**
     * @brief Get the JSON target for the input parameter.
     * @return The JSON target for the input parameter as a CCTools::JSONChildrenIdentifierType.
     * 
     * The location of this input parameter in a JSON file is characterized by the JSON name, JSON children and JSON target.
     * To find the location, the model handler will search for a node in the JSON tree with the specified name, than traverse the children and 
     * find the JSON target property of the last child.
     * 
     * E.g., name="Inner Layer", children={"rho"}, target="radius" specifies the 'radius' property of a node called 'rho', which is a child of a node with the 'name' property 'Inner Layer'.
     * setValueByName("Connect South V2", {"uvw1", 0}, "u", 0.1) will find "Connect South V2", traverse the children "uvw1", traverse to the first element of its array and set the 'u' value.
     */
    virtual CCTools::JSONChildrenIdentifierType getJSONTarget(){
        return JSON_target_;
    }

    virtual ~InputParamRangeInterface() = default;

protected:
    
        InputParamRangeInterface() = default;

        std::string column_name_;
        std::vector<Json::Value> range_;
        std::string JSON_name_;
        std::vector<CCTools::JSONChildrenIdentifierType> JSON_children_;
        CCTools::JSONChildrenIdentifierType JSON_target_;

};

#endif // INPUT_PARAM_RANGE_INTERFACE_H