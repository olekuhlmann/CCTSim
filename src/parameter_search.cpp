#include "parameter_search.h"

ParameterSearch::ParameterSearch(std::vector<std::shared_ptr<InputParamRangeInterface>> inputParamsRanges,
                                 std::vector<std::shared_ptr<OutputCriterionInterface>> outputCriteria, CCTools::ModelHandler &modelHandler) : inputParamsRanges_(inputParamsRanges),
                                                                                                                                               outputCriteria_(outputCriteria)
{
    // Initialize
    modelHandler_ = modelHandler;
    modelCalculator_ = CCTools::ModelCalculator(modelHandler.getTempJsonPath());

    // Check if the input params are valid
    checkInputParams(inputParamsRanges_);

    // Run the parameter search
    run();
}

void ParameterSearch::run()
{
    Logger::info("=== Starting parameter search ===");

    // Initialize the output file
    initOutputFile();

    // Get all parameter ranges
    std::vector<std::vector<Json::Value>> param_ranges = getParamRanges(inputParamsRanges_);

    // Compute the number of steps
    size_t num_steps = getNumSteps(param_ranges);

    Logger::info("Number of steps: " + std::to_string(num_steps));

    // Check what computations are necessary for the output criteria
    std::vector<std::type_index> required_calculations_ = getRequiredCalculations(outputCriteria_);

    // Loop over all steps
    for (size_t step_num = 0; step_num < num_steps; step_num++)
    {
        Logger::info("== Starting step with index " + std::to_string(step_num) + " / " + std::to_string(num_steps - 1) + " ==");

        // Get the next input param configuration
        std::vector<Json::Value> next_config = getParameterConfiguration(step_num, param_ranges);

        // Apply paramater configuration for the current step
        applyParameterConfiguration(inputParamsRanges_, next_config, modelHandler_);

        // Run the necessary calculations
        std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> calc_results = runCalculations(required_calculations_, modelCalculator_, modelHandler_);

        // Compute the output criteria
        std::vector<double> output_values = computeCriteria(calc_results, outputCriteria_);

        // Write the output values to the output file
        writeStepToOutputFile(step_num, outputFile_, next_config, output_values);
    }

    // Close the output file
    closeOutputFile();
}

void ParameterSearch::checkInputParams(std::vector<std::shared_ptr<InputParamRangeInterface>> &inputParamsRanges)
{
    // Try to get the value of each input parameter
    for (size_t i = 0; i < inputParamsRanges.size(); i++)
    {
        // Get JSON location of the target
        std::string JSON_name = (*inputParamsRanges[i]).getJSONName();
        std::vector<std::string> JSON_children = (*inputParamsRanges[i]).getJSONChildren();
        std::string JSON_target = (*inputParamsRanges[i]).getJSONTarget();

        // Try to get the value, this will throw an exception if the value cannot be found
        try
        {
            modelHandler_.getValueByName(JSON_name, JSON_children, JSON_target);
        }

        catch (const std::runtime_error &e)
        {
            Logger::error("Invalid input parameter " + (*inputParamsRanges[i]).getColumnName() + ": " + std::string(e.what()));
            throw;
        }
    }

    Logger::info("All input parameters are valid.");
}

void ParameterSearch::initOutputFile()
{
    // Check if the output directory exists
    if (!std::filesystem::exists(OUTPUT_DIR_PATH))
    {
        std::filesystem::create_directory(OUTPUT_DIR_PATH);
    }

    // Create the output file name with the current date and time
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time_t);

    std::ostringstream oss;
    oss << OUTPUT_DIR_PATH << "CCTSim_output_" << std::put_time(&now_tm, "%Y_%m_%d_%H_%M_%S") << ".csv";

    std::string output_file_path = oss.str();

    // Create the output file
    outputFile_.open(output_file_path);

    // Write the index and the column names of the input params and output criteria to the file
    outputFile_ << "index,";
    // input params
    for (size_t i = 0; i < inputParamsRanges_.size(); i++)
    {
        outputFile_ << inputParamsRanges_[i]->getColumnName() << ",";
    }
    // output criteria
    for (size_t i = 0; i < outputCriteria_.size(); i++)
    {
        outputFile_ << outputCriteria_[i]->getColumnName();
        if (i < outputCriteria_.size() - 1)
        {
            outputFile_ << ",";
        }
    }

    // Write a newline
    outputFile_ << std::endl;

    Logger::info("Output file initialized: " + output_file_path);
}

std::vector<std::vector<Json::Value>> ParameterSearch::getParamRanges(std::vector<std::shared_ptr<InputParamRangeInterface>> &inputParamsRanges)
{
    std::vector<std::vector<Json::Value>> param_ranges;
    for (size_t i = 0; i < inputParamsRanges.size(); i++)
    {
        param_ranges.push_back(inputParamsRanges[i]->getRange());
    }
    return param_ranges;
}

size_t ParameterSearch::getNumSteps(const std::vector<std::vector<Json::Value>> &param_ranges)
{
    if (param_ranges.size() == 0)
    {
        throw std::runtime_error("No parameter ranges provided. Number of steps cannot be calculated.");
    }

    size_t num_steps = 1;

    for (size_t i = 0; i < param_ranges.size(); i++)
    {
        num_steps *= param_ranges[i].size();
    }

    return num_steps;
}

std::vector<std::type_index> ParameterSearch::getRequiredCalculations(std::vector<std::shared_ptr<OutputCriterionInterface>> &outputCriteria)
{
    // Use a set to avoid duplicates
    std::set<std::type_index, std::less<>> unique_calculations;

    // Get the required calculations for each output criterion
    for (size_t i = 0; i < outputCriteria.size(); i++)
    {
        std::vector<std::type_index> criterion_calculations = outputCriteria[i]->getRequiredCalculations();
        unique_calculations.insert(criterion_calculations.begin(), criterion_calculations.end());
    }

    // Convert the set to a vector
    std::vector<std::type_index> required_calculations(unique_calculations.begin(), unique_calculations.end());

    return required_calculations;
}

void ParameterSearch::applyParameterConfiguration(std::vector<std::shared_ptr<InputParamRangeInterface>> &inputParamsRanges, std::vector<Json::Value> next_config, CCTools::ModelHandler &model_handler)
{

    // Apply param config
    for (size_t i = 0; i < inputParamsRanges.size(); i++)
    {
        // Get JSON location of the target
        std::string JSON_name = (*inputParamsRanges[i]).getJSONName();
        std::vector<std::string> JSON_children = (*inputParamsRanges[i]).getJSONChildren();
        std::string JSON_target = (*inputParamsRanges[i]).getJSONTarget();
        Json::Value value = next_config[i];

        // Apply the parameter configuration
        model_handler.setValueByName(JSON_name, JSON_children, JSON_target, value);
    }

    // Log the parameter configuration
    std::string param_config_str = "";
    for (size_t i = 0; i < inputParamsRanges.size(); i++)
    {
        param_config_str += (*inputParamsRanges[i]).getColumnName();
        param_config_str += ": ";
        param_config_str += next_config[i].asString();
        if (i != inputParamsRanges.size() - 1)
        {
            param_config_str += ", ";
        }
    }
    Logger::info("Applied parameter configuration: " + param_config_str);
}

std::vector<Json::Value> ParameterSearch::getParameterConfiguration(size_t step_num, std::vector<std::vector<Json::Value>> &param_ranges)
{
    size_t n = param_ranges.size();
    if (n == 0)
    {
        throw std::invalid_argument("param_ranges cannot be empty");
    }

    // Calculate sizes of each parameter range
    std::vector<size_t> sizes(n);
    for (size_t i = 0; i < n; ++i)
    {
        sizes[i] = param_ranges[i].size();
        if (sizes[i] == 0)
        {
            throw std::invalid_argument("Parameter range cannot be empty");
        }
    }

    // Calculate total number of configurations
    size_t total_steps = getNumSteps(param_ranges);

    if (step_num >= total_steps)
    {
        throw std::out_of_range("step_num exceeds total configurations");
    }

    // Calculate strides
    std::vector<size_t> strides(n);
    strides[n - 1] = 1;
    for (int i = n - 2; i >= 0; --i)
    {
        strides[i] = sizes[i + 1] * strides[i + 1];
    }

    // Generate parameter configuration based on step_num
    std::vector<Json::Value> configuration(n);
    for (size_t i = 0; i < n; ++i)
    {
        size_t index = (step_num / strides[i]) % sizes[i];
        configuration[i] = param_ranges[i][index];
    }

    return configuration;
}

std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> ParameterSearch::runCalculations(std::vector<std::type_index> required_calculations, CCTools::ModelCalculator &modelCalculator, CCTools::ModelHandler &modelHandler)
{
    // Return vector
    std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> calc_results;

    // Check all required calculations
    for (const std::type_index &type : required_calculations)
    {
        // Check for harmonics calculation
        if (type == std::type_index(typeid(CCTools::HarmonicsDataHandler)))
        {
            // Run
            CCTools::HarmonicsDataHandler handler;
            modelCalculator.reload_and_calc_harmonics(modelHandler.getTempJsonPath(), handler);
            calc_results.push_back(std::make_shared<CCTools::HarmonicsDataHandler>(handler));
        }
        // Check for mesh calculation
        else if (type == std::type_index(typeid(CCTools::MeshDataHandler)))
        {
            // Run
            CCTools::MeshDataHandler handler;
            modelCalculator.reload_and_calc_mesh(modelHandler.getTempJsonPath(), handler);
            calc_results.push_back(std::make_shared<CCTools::MeshDataHandler>(handler));
        }
        else
        {
            std::string type_name = type.name();
            throw std::invalid_argument("Unknown calculation type " + type_name + " in required calculations");
        }
    }

    return calc_results;
}

std::vector<double> ParameterSearch::computeCriteria(std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> calcResults, std::vector<std::shared_ptr<OutputCriterionInterface>> outputCriteria)
{
    // Return vector
    std::vector<double> output_values;

    // Compute all output criteria
    for (auto &output_criterion_ptr : outputCriteria)
    {
        OutputCriterionInterface &output_criterion = *output_criterion_ptr;

        // Get required calculation results
        std::vector<std::type_index> required_calc_results = output_criterion.getRequiredCalculations();

        // Find each required calculation result
        std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> criterion_calc_results;
        for (const std::type_index &required_calc_result : required_calc_results)
        {
            bool found = false;
            for (auto &calc_result : calcResults)
            {
                if (std::type_index(typeid(*calc_result)) == required_calc_result)
                {
                    criterion_calc_results.push_back(calc_result);
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                std::string required_calc_result_name = required_calc_result.name();
                throw std::invalid_argument("Required calculation result " + required_calc_result_name + " not found for output criterion " + output_criterion.getColumnName());
            }
        }

        // Compute the output criterion
        double output_value = output_criterion.computeCriterion(criterion_calc_results);
        output_values.push_back(output_value);
        Logger::info_double("Computed output criterion " + output_criterion.getColumnName(), output_value);
    }

    return output_values;
}

void ParameterSearch::writeStepToOutputFile(size_t step_num, std::ofstream &outputFile, std::vector<Json::Value> &input_values, std::vector<double> &output_values)
{
    // Write the step number
    outputFile << step_num << ",";

    // Write the input values
    for (size_t i = 0; i < input_values.size(); i++)
    {
        outputFile << input_values[i] << ",";
    }

    // Write the output values
    for (size_t i = 0; i < output_values.size(); i++)
    {
        outputFile << output_values[i];
        if (i < output_values.size() - 1)
        {
            outputFile << ",";
        }
    }

    // Write a newline
    outputFile << std::endl;
}

void ParameterSearch::closeOutputFile()
{
    // Close the output file
    if (outputFile_.is_open())
    {
        outputFile_.close();
    }
}

ParameterSearch::~ParameterSearch()
{
    closeOutputFile();
}