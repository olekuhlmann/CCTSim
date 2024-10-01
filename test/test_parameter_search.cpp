#include "gtest/gtest.h"
#include "parameter_search.h"
#include "input_layer_pitch.hh"
#include "input_param_range_interface.h"
#include "input_multipole_scaling.hh"
#include "output_criterion_interface.h"
#include "output_a_multipole.hh"
#include "output_b_multipole.hh"
#include "constants.h"
#include "model_handler.h"
#include "model_calculator.h"
#include <memory>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <typeindex>

class TestableParameterSearch : public ParameterSearch {
public:
    using ParameterSearch::ParameterSearch;
    using ParameterSearch::initOutputFile;
    using ParameterSearch::checkInputParams;
    using ParameterSearch::getParamRanges;
    using ParameterSearch::getNumSteps;
    using ParameterSearch::getRequiredCalculations;
    using ParameterSearch::getParameterConfiguration;
    using ParameterSearch::runCalculations;
    using ParameterSearch::applyParameterConfiguration;
    using ParameterSearch::computeCriteria;
    using ParameterSearch::writeStepToOutputFile;
};

class ParameterSearchTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test data
        model_path = TEST_DATA_DIR + "quad_test.json";

        // Create model handler
        modelHandler = std::make_shared<CCTools::ModelHandler>(model_path);

        // Create input parameter ranges
        inputs.clear();
        inputs.push_back(std::make_shared<InputLayerPitch>("custom cct outer", std::vector<Json::Value>{2.05}, "_outer"));
        inputs.push_back(std::make_shared<InputLayerPitch>("custom cct inner", std::vector<Json::Value>{2.09, 2.1, 2.11, 2.12}, "_inner"));

        // Create output criteria
        outputs.clear();
        for (size_t i = 1; i <= 10; i++) {
            outputs.push_back(std::make_shared<OutputAMultipole>(i));
            outputs.push_back(std::make_shared<OutputBMultipole>(i));
        }

        // Initialize the parameter search object
        parameterSearch = std::make_shared<TestableParameterSearch>(inputs, outputs, *modelHandler);

        // Initialize the model calculator with the model path
        modelCalculator = std::make_shared<CCTools::ModelCalculator>(modelHandler->getTempJsonPath());
    }

    void TearDown() override {
    }

    // Member variables
    std::string model_path;
    std::shared_ptr<CCTools::ModelHandler> modelHandler;
    std::shared_ptr<CCTools::ModelCalculator> modelCalculator;
    std::vector<std::shared_ptr<InputParamRangeInterface>> inputs;
    std::vector<std::shared_ptr<OutputCriterionInterface>> outputs;
    std::shared_ptr<TestableParameterSearch> parameterSearch;
};

TEST_F(ParameterSearchTest, ConstructorDoesNotThrowForValidInputs) {
    EXPECT_NO_THROW({
        TestableParameterSearch search(inputs, outputs, *modelHandler);
    });
}

TEST_F(ParameterSearchTest, ConstructorDoesNotThrowForAnyInput) {
    // Give all types of inputs
    std::string modelPath = TEST_DATA_DIR + "quad_test_B3_linear.json";

    // Create model handler
    CCTools::ModelHandler model_handler = CCTools::ModelHandler(modelPath);

    std::vector<std::shared_ptr<InputParamRangeInterface>> inputs_new = inputs;

    inputs_new.push_back(std::make_shared<InputMultipoleScaling>("b1", "B1", HarmonicScalingFunctionTarget::CONST, std::vector<Json::Value>{0.0}));
    inputs_new.push_back(std::make_shared<InputMultipoleScaling>("b3", "B3", HarmonicScalingFunctionTarget::LINEAR_OFFSET, std::vector<Json::Value>{0.0}));
    inputs_new.push_back(std::make_shared<InputMultipoleScaling>("b3", "B3", HarmonicScalingFunctionTarget::LINEAR_SLOPE, std::vector<Json::Value>{0.0}));

    EXPECT_NO_THROW({
        TestableParameterSearch search(inputs_new, outputs, model_handler);
    });
}

TEST_F(ParameterSearchTest, RunDoesNotThrowWithCorrectInputs) {
    // Adjust inputs and outputs for this test
    std::vector<std::shared_ptr<InputParamRangeInterface>> testInputs;
    testInputs.push_back(std::make_shared<InputLayerPitch>("custom cct outer", std::vector<Json::Value>{2.05}, "_outer"));

    std::vector<std::shared_ptr<OutputCriterionInterface>> testOutputs;
    testOutputs.push_back(std::make_shared<OutputAMultipole>(1));

    // Create a new parameter search with these inputs and outputs
    TestableParameterSearch testParameterSearch(testInputs, testOutputs, *modelHandler);

    EXPECT_NO_THROW({
        testParameterSearch.run();
    });
}

TEST_F(ParameterSearchTest, InitOutputFileCreatesFileWithCorrectFormatting) {
    // Call initOutputFile
    std::string outputFilePath = parameterSearch->initOutputFile();

    // Check that the directory exists; NOTE: the output dir may already exist before the test. 
    EXPECT_TRUE(std::filesystem::exists(OUTPUT_DIR_PATH));

    // Check that the output file exists
    EXPECT_TRUE(std::filesystem::exists(outputFilePath));

    // Read the header line from the file
    std::ifstream outputFile(outputFilePath);
    ASSERT_TRUE(outputFile.is_open());

    std::string headerLine;
    std::getline(outputFile, headerLine);

    // Expected header line: "index,input_param1,input_param2,...,output_criterion1,output_criterion2,..."
    std::stringstream expectedHeader;
    expectedHeader << "index";

    // Add input parameter column names
    for (const auto& inputParam : inputs) {
        expectedHeader << "," << inputParam->getColumnName();
    }

    // Add output criterion column names
    for (const auto& outputCriterion : outputs) {
        expectedHeader << "," << outputCriterion->getColumnName();
    }

    // Compare the expected header with the actual header
    EXPECT_EQ(headerLine, expectedHeader.str());

    outputFile.close();
}

TEST_F(ParameterSearchTest, CheckInputParamsDoesNotThrowForValidInputs) {
    EXPECT_NO_THROW({
        parameterSearch->checkInputParams(inputs);
    });
}

TEST_F(ParameterSearchTest, CheckInputParamsThrowsForInvalidInputParam) {
    // Create an invalid input param
    auto invalidInput = std::make_shared<InputLayerPitch>("nonexistent layer", std::vector<Json::Value>{2.0}, "_invalid");

    // Add it to a new inputs vector
    std::vector<std::shared_ptr<InputParamRangeInterface>> invalidInputs = inputs;
    invalidInputs.push_back(invalidInput);

    // Create a new parameter search object with the invalid inputs
    // Expect that checkInputParams throws an exception (called in the constructor, make sure to call it explicitly anyway)
    EXPECT_THROW({
        TestableParameterSearch invalidParameterSearch(invalidInputs, outputs, *modelHandler);
        invalidParameterSearch.checkInputParams(invalidInputs);
    }, std::runtime_error);
}

TEST_F(ParameterSearchTest, GetParamRangesCorrectlyExtractsRanges) {
    auto paramRanges = parameterSearch->getParamRanges(inputs);

    // The inputs vector has two input params
    EXPECT_EQ(paramRanges.size(), inputs.size());

    // For each input param, check that the range matches
    for (size_t i = 0; i < inputs.size(); ++i) {
        EXPECT_EQ(paramRanges[i], inputs[i]->getRange());
    }
}

TEST_F(ParameterSearchTest, GetNumStepsYieldsCorrectValue) {
    // Case 1: inputs as defined in SetUp
    auto paramRanges = parameterSearch->getParamRanges(inputs);
    size_t numSteps = parameterSearch->getNumSteps(paramRanges);
    EXPECT_EQ(numSteps, 1 * 4); // 1 (pitch_outer) * 4 (pitch_inner) = 4

    // Case 2: inputs with different ranges
    std::vector<std::shared_ptr<InputParamRangeInterface>> testInputs;
    testInputs.push_back(std::make_shared<InputLayerPitch>("custom cct outer", std::vector<Json::Value>{2.05, 2.06}, "_outer"));
    testInputs.push_back(std::make_shared<InputLayerPitch>("custom cct inner", std::vector<Json::Value>{2.09, 2.1}, "_inner"));

    auto testParamRanges = parameterSearch->getParamRanges(testInputs);
    size_t testNumSteps = parameterSearch->getNumSteps(testParamRanges);
    EXPECT_EQ(testNumSteps, 2 * 2); // 2 * 2 = 4

    // Case 3: inputs with three parameters
    testInputs.push_back(std::make_shared<InputLayerPitch>("custom cct middle", std::vector<Json::Value>{2.0, 2.1}, "_middle"));
    testParamRanges = parameterSearch->getParamRanges(testInputs);
    testNumSteps = parameterSearch->getNumSteps(testParamRanges);
    EXPECT_EQ(testNumSteps, 2 * 2 * 2); // 2 * 2 * 2 = 8

    // Case 4: one input with empty range
    testInputs.clear();
    testInputs.push_back(std::make_shared<InputLayerPitch>("custom cct outer", std::vector<Json::Value>{}, "_outer"));
    testParamRanges = parameterSearch->getParamRanges(testInputs);
    size_t numStepsEmptyRange = parameterSearch->getNumSteps(testParamRanges);
    EXPECT_EQ(numStepsEmptyRange, 0); // empty range, zero steps
}

TEST_F(ParameterSearchTest, GetRequiredCalculationsGivesCorrectHandlers) {
    auto requiredCalculations = parameterSearch->getRequiredCalculations(outputs);

    // Should contain only HarmonicsDataHandler
    EXPECT_EQ(requiredCalculations.size(), 1);
    EXPECT_EQ(requiredCalculations[0], std::type_index(typeid(CCTools::HarmonicsDataHandler)));
}

TEST_F(ParameterSearchTest, GetParameterConfigurationReturnsCorrectValues) {
    auto paramRanges = parameterSearch->getParamRanges(inputs);
    size_t numSteps = parameterSearch->getNumSteps(paramRanges);

    for (size_t stepNum = 0; stepNum < numSteps; ++stepNum) {
        auto config = parameterSearch->getParameterConfiguration(stepNum, paramRanges);

        // For our test case, we have two inputs
        EXPECT_EQ(config.size(), inputs.size());

        // The first input (pitch_outer) has one value (2.05 mm), so config[0] should always be 2.05 / 1000.0
        EXPECT_DOUBLE_EQ(config[0].asDouble(), 2.05 / 1000.0); // Converted to meters

        // The second input (pitch_inner) cycles through its range
        size_t index_in_second_param = (stepNum / 1) % paramRanges[1].size(); // Since first param has size 1

        EXPECT_DOUBLE_EQ(config[1].asDouble(), paramRanges[1][index_in_second_param].asDouble());
    }
}

TEST_F(ParameterSearchTest, RunCalculationsReturnsCorrectHandlers) {
    auto requiredCalculations = parameterSearch->getRequiredCalculations(outputs);

    // Run calculations
    auto calcResults = parameterSearch->runCalculations(requiredCalculations, *modelCalculator, *modelHandler);

    // Should have one calculation result handler, which is a HarmonicsDataHandler
    EXPECT_EQ(calcResults.size(), 1);
    EXPECT_EQ(std::type_index(typeid(*calcResults[0])), std::type_index(typeid(CCTools::HarmonicsDataHandler)));
}

TEST_F(ParameterSearchTest, ApplyParameterConfigurationCorrectlyUpdatesModel) {
    auto paramRanges = parameterSearch->getParamRanges(inputs);
    size_t stepNum = 0;
    auto config = parameterSearch->getParameterConfiguration(stepNum, paramRanges);

    // Apply the parameter configuration
    parameterSearch->applyParameterConfiguration(inputs, config, *modelHandler);

    // Now check that the model has been updated using getValueByName

    // For the first input (pitch_outer)
    Json::Value value_outer = modelHandler->getValueByName(inputs[0]->getJSONName(), inputs[0]->getJSONChildren(), inputs[0]->getJSONTarget());
    double scalingValue_outer = value_outer.asDouble();

    // Compare with the value we set
    EXPECT_DOUBLE_EQ(scalingValue_outer, config[0].asDouble());

    // Similarly for the second input (pitch_inner)
    Json::Value value_inner = modelHandler->getValueByName(inputs[1]->getJSONName(), inputs[1]->getJSONChildren(), inputs[1]->getJSONTarget());
    double scalingValue_inner = value_inner.asDouble();

    EXPECT_DOUBLE_EQ(scalingValue_inner, config[1].asDouble());
}

TEST_F(ParameterSearchTest, ComputeCriteriaReturnsVectorOfCorrectSize) {
    auto requiredCalculations = parameterSearch->getRequiredCalculations(outputs);
    auto calcResults = parameterSearch->runCalculations(requiredCalculations, *modelCalculator, *modelHandler);

    auto criteriaValues = parameterSearch->computeCriteria(calcResults, outputs);

    // Should have criteriaValues.size() == outputs.size()
    EXPECT_EQ(criteriaValues.size(), outputs.size());
}

TEST_F(ParameterSearchTest, WriteStepToOutputFileWritesCorrectlyFormattedLine) {
    // Prepare test data

    // Generate unique filename with timestamp
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch())
                      .count();
    std::stringstream ss;
    ss << "test_output_" << now_ms << ".csv";
    std::string filename = ss.str();

    std::ofstream testOutputFile(filename);
    ASSERT_TRUE(testOutputFile.is_open());

    // Write header
    testOutputFile << "index";
    for (const auto& inputParam : inputs) {
        testOutputFile << "," << inputParam->getColumnName();
    }
    for (const auto& outputCriterion : outputs) {
        testOutputFile << "," << outputCriterion->getColumnName();
    }
    testOutputFile << "\n";

    // Prepare input_values and output_values
    std::vector<Json::Value> input_values = {2.05 / 1000.0, 2.09 / 1000.0}; // Converted to meters
    std::vector<double> output_values(outputs.size(), 0.0); // Dummy output values

    // Call writeStepToOutputFile
    parameterSearch->writeStepToOutputFile(0, testOutputFile, input_values, output_values);

    testOutputFile.close();

    // Read back the line
    std::ifstream inputFile(filename);
    ASSERT_TRUE(inputFile.is_open());

    std::string line;
    std::getline(inputFile, line); // Skip header
    std::getline(inputFile, line); // Read data line

    // Split the line into tokens
    std::vector<std::string> tokens;
    std::stringstream lineStream(line);
    std::string token;
    while (std::getline(lineStream, token, ',')) {
        tokens.push_back(token);
    }

    // Prepare expected values
    std::vector<std::string> expectedTokens;
    expectedTokens.push_back("0"); // index
    for (const auto& value : input_values) {
        std::stringstream ss_value;
        ss_value << std::setprecision(17) << value.asDouble();
        expectedTokens.push_back(ss_value.str());
    }
    for (const auto& value : output_values) {
        expectedTokens.push_back("0");
    }

    // Compare the number of tokens
    EXPECT_EQ(tokens.size(), expectedTokens.size());

    // Compare each token
    for (size_t i = 0; i < tokens.size(); ++i) {
        // For the input values, compare as doubles with a tolerance
        if (i == 1 || i == 2) { // indices 1 and 2 are input_values
            double actualValue = std::stod(tokens[i]);
            double expectedValue = std::stod(expectedTokens[i]);
            EXPECT_NEAR(actualValue, expectedValue, 1e-10) << "Mismatch at token index " << i;
        } else {
            // For index and output_values (which are zeros in this case), compare as strings
            EXPECT_EQ(tokens[i], expectedTokens[i]) << "Mismatch at token index " << i;
        }
    }

    inputFile.close();

    // Clean up
    std::filesystem::remove(filename);
}

