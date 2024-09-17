#include "parameter_search.h"

ParameterSearch::ParameterSearch(std::vector<std::shared_ptr<InputParamRangeInterface>> inputParamsRanges,
                                 std::vector<std::shared_ptr<OutputCriterionInterface>> outputCriteria, CCTools::ModelHandler &modelHandler) : inputParamsRanges_(inputParamsRanges),
                                                                                                                                               outputCriteria_(outputCriteria)
{
    modelHandler_ = modelHandler;
    modelCalculator_ = CCTools::ModelCalculator(modelHandler.getTempJsonPath());
    run();
}

void ParameterSearch::run()
{
    Logger::info("=== Starting parameter search ===");

    // Initialize the output file
    initOutputFile();

    // Get all parameter ranges
    std::vector<std::vector<std::string>> param_ranges = getParamRanges(inputParamsRanges_);

    // Compute the number of steps
    size_t num_steps = getNumSteps(param_ranges);

    Logger::info("Number of steps: " + std::to_string(num_steps));

    // Check what computations are necessary for the output criteria
    std::vector<std::type_info> required_calculations_ = getRequiredCalculations(outputCriteria_);

    // Loop over all steps
    for (size_t step_num = 0; step_num < num_steps; step_num++)
    {
        Logger::info("== Starting step with index" + std::to_string(step_num) + " of " + std::to_string(num_steps - 1) + " ==");
        
        // Apply paramater configuration for the current step
        applyParameterConfiguration(inputParamsRanges_, step_num, param_ranges, modelHandler_);

        // Run the necessary calculations
        std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> calc_results = runCalculations(required_calculations_, modelCalculator_);

        // Compute the output criteria
        std::vector<double> output_values = computeCriteria(calc_results, outputCriteria_);

        // Write the output values to the output file
        writeStepToOutputFile(step_num, outputFile_, output_values);
    }

    // Close the output file
    closeOutputFile();
}

void ParameterSearch::initOutputFile()
{
    // Check if the output directory exists
    if (!std::filesystem::exists(OUTPUT_DIR_PATH))
    {
        std::filesystem::create_directory(OUTPUT_DIR_PATH);
    }

    // Create the output file name with the current date and time
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string date_time = std::ctime(&now);
    std::replace(date_time.begin(), date_time.end(), ' ', '_');
    std::replace(date_time.begin(), date_time.end(), ':', '-');
    std::string output_file_path = OUTPUT_DIR_PATH + "output_" + date_time + ".csv";

    // Create the output file
    outputFile_.open(output_file_path);

    // Write the index and the column names of the output criteria to the file
    outputFile_ << "index,";
    for (size_t i = 0; i < outputCriteria_.size(); i++)
    {
        outputFile_ << outputCriteria_[i]->getColumnName();
        if (i < outputCriteria_.size() - 1)
        {
            outputFile_ << ",";
        }
    }

    Logger::info("Output file initialized: " + output_file_path);
}

std::vector<std::vector<std::string>> ParameterSearch::getParamRanges(std::vector<std::shared_ptr<InputParamRangeInterface>> &inputParamsRanges)
{
    std::vector<std::vector<std::string>> param_ranges;
    for (size_t i = 0; i < inputParamsRanges.size(); i++)
    {
        param_ranges.push_back(inputParamsRanges[i]->getRange());
    }
    return param_ranges;
}

size_t ParameterSearch::getNumSteps(const std::vector<std::vector<std::string>> &param_ranges)
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

std::vector<std::type_info> ParameterSearch::getRequiredCalculations(std::vector<std::shared_ptr<OutputCriterionInterface>> &outputCriteria)
{
    // Use a set to avoid duplicates
    std::set<std::type_info, std::less<>> unique_calculations;

    // Get the required calculations for each output criterion
    for (size_t i = 0; i < outputCriteria.size(); i++)
    {
        std::vector<std::type_info> criterion_calculations = outputCriteria[i]->getRequiredCalculations();
        unique_calculations.insert(criterion_calculations.begin(), criterion_calculations.end());
    }

    // Convert the set to a vector
    std::vector<std::type_info> required_calculations(unique_calculations.begin(), unique_calculations.end());

    // Log the required calculations
    std::string required_calculations_str = "Required calculations: ";
    for (size_t i = 0; i < required_calculations.size(); i++)
    {
        required_calculations_str += required_calculations[i].name();
        if (i < required_calculations.size() - 1)
        {
            required_calculations_str += ", ";
        }
    }

    Logger::info(required_calculations_str);

    return required_calculations;
}

void ParameterSearch::applyParameterConfiguration(std::vector<std::shared_ptr<InputParamRangeInterface>> &inputParamsRanges, size_t step_num, std::vector<std::vector<std::string>> &param_ranges, CCTools::ModelHandler &model_handler)
{
    // TODO: Implement this function
    //TODO : log the parameter configuration
}

std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> ParameterSearch::runCalculations(std::vector<std::type_info> required_calculations, CCTools::ModelCalculator &modelCalculator){
    //TODO: Implement this function
}

std::vector<double> ParameterSearch::computeCriteria(std::vector<std::shared_ptr<CCTools::CalcResultHandlerBase>> calcResults, std::vector<std::shared_ptr<OutputCriterionInterface>> outputCriteria){
    //TODO: Implement this function 
    //TODO: Log all computed output criteria
}

void ParameterSearch::writeStepToOutputFile(size_t step_num, std::ofstream &outputFile, std::vector<double> &output_values){
    //TODO: Implement this function
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