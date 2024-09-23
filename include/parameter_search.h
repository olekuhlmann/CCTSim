#ifndef PARAMETER_SEARCH_H
#define PARAMETER_SEARCH_H

#include <vector>
#include <memory>
#include <filesystem>
#include <json/json.h>
#include <constants.h>
#include <model_handler.h>
#include <model_calculator.h>
#include <iomanip>
#include <sstream>
#include "input_param_range_interface.h"
#include "output_criterion_interface.h"

using CCTools::Logger;

class ParameterSearch
{

public:
    /**
     * @brief Construct a ParameterSearch object.
     * @param inputParamsRanges The input parameter ranges.
     * @param outputCriteria The output criteria.
     *
     * Initialize a ParameterSearch object to run a grid search on the input parameters.
     */
    ParameterSearch(std::vector<std::shared_ptr<InputParamRangeInterface>> inputParamsRanges, std::vector<std::shared_ptr<OutputCriterionInterface>> outputCriteria, CCTools::ModelHandler &modelHandler);
    ~ParameterSearch();

    /**
     * @brief Run the grid search.
     *
     * Run the grid search on the input parameters and compute the output criteria for each step.
     * The results will be written to a CSV in the `OUTPUT_DIR_PATH` directory.
     */
    void run();

private:
    /**
     * @brief Initialize the output file.
     *
     * Initialize the output CSV file with the column names of the output criteria and keeps the file open.
     * Will create the output folder if it does not exist.
     */
    void initOutputFile();

    /**
     * @brief Close the output file.
     *
     * Close the output file if it is open.
     */
    void closeOutputFile();

    /**
     * @brief Check if the input parameters are valid.
     * @param inputParamsRanges The input parameter ranges.
     * 
     * Check if the input parameters are valid, i.e. if every input parameter target can be located in the JSON file. Throws an exception if the input parameters are not valid.
     */
    void checkInputParams(std::vector<std::shared_ptr<InputParamRangeInterface>> &inputParamsRanges);

    /**
     * @brief Get the parameter ranges from all input parameters.
     * @param inputParamsRanges The input parameter ranges.
     * @return The parameter ranges as a Json::Value vector.
     *
     * Get the parameter ranges from all input parameters as a string vector to be put into the respective JSON field for these parameters.
     */
    static std::vector<std::vector<Json::Value>> getParamRanges(std::vector<std::shared_ptr<InputParamRangeInterface>> &inputParamsRanges);

    /**
     * @brief Get the number of steps in the grid search.
     * @param param_ranges The parameter ranges.
     * @return The number of steps in the grid search.
     *
     * Get the number of steps in the grid search by multiplying the number of steps in each parameter range.
     */
    static size_t getNumSteps(const std::vector<std::vector<Json::Value>> &param_ranges);

    /**
     * @brief Get the required calculations for the output criteria.
     * @param outputCriteria The output criteria.
     * @return The required calculation handlers for the output criteria as a type index vector.
     */
    static std::vector<std::type_index> getRequiredCalculations(std::vector<std::shared_ptr<OutputCriterionInterface>> &outputCriteria);

    /**
     * @brief Apply the parameter configuration.
     * @param inputParamsRanges The input parameter ranges.
     * @param step_num The current step number.
     * @param param_ranges The Json::Value vector representation of the input parameter ranges.
     * @param modelHandler The model handler.
     *
     * Pass the parameter configuration for the step number to the model handler.
     */
    static void applyParameterConfiguration(std::vector<std::shared_ptr<InputParamRangeInterface>> &inputParamsRanges, size_t step_num, std::vector<std::vector<Json::Value>> &param_ranges, CCTools::ModelHandler &modelHandler);

    /**
     * @brief Get the next parameter configuration.
     * @param step_num The current step number (0-indexed).
     * @param param_ranges The JSON::Value vector representation of the input parameter ranges.
     * @return The parameter configuration as a vector of `Json::Value`
     * 
     * Get the parameter configuration for the step `step_num` of the parameter search.
     * The parameter confguration is selected based on a grid search approach. 
     */
    static std::vector<Json::Value> getParameterConfiguration(size_t step_num, std::vector<std::vector<Json::Value>> &param_ranges);
    /**
     * @brief Run the necessary calculations for the output criteria.
     * @param required_calculations Type info of the required calculation handlers for the output criteria. Is assumed to be duplicate-free.
     * @param modelCalculator The model calculator.
     * @param modelHandler The model handler.
     * @return The calculation results as a vector of shared pointers to CalcResultHandlerBase.
     *
     * Run the necessary calculations for the output criteria and return the results as a vector of shared pointers to CalcResultHandlerBase.
     */
    static std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> runCalculations(std::vector<std::type_index> required_calculations, CCTools::ModelCalculator &modelCalculator, CCTools::ModelHandler &modelHandler);

    /**
     * @brief Compute the output criteria.
     * @param calcResults The calculation results.
     * @param outputCriteria The output criteria.
     * @return The values of the output criteria as a double vector.
     *
     * Compute the output criteria for the given calculation results and return the values as a double vector.
     */
    static std::vector<double> computeCriteria(std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> calcResults, std::vector<std::shared_ptr<OutputCriterionInterface>> outputCriteria);

    /**
     * @brief Write the step results to the output file.
     * @param step_num The current step number.
     * @param outputFile The output file stream.
     * @param output_values The values of the output criteria.
     *
     * Write the values of the output criteria for the current step to the output file. Assumes that file is present and open.
     */
    void writeStepToOutputFile(size_t step_num, std::ofstream &outputFile, std::vector<double> &output_values);

    std::vector<std::shared_ptr<InputParamRangeInterface>> inputParamsRanges_;
    std::vector<std::shared_ptr<OutputCriterionInterface>> outputCriteria_;
    std::ofstream outputFile_;
    CCTools::ModelHandler modelHandler_;
    CCTools::ModelCalculator modelCalculator_;
};

#endif // PARAMETER_SEARCH_H