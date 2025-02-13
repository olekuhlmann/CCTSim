#include "gtest/gtest.h"
#include "json_range.hh"  
#include <json/json.h>
#include <vector>
#include <limits>
#include <rat/models/pathconnect2.hh>
#include <rat/models/modelgroup.hh>
#include <rat/models/modelcoil.hh>
#include <rat/models/modelclip.hh>
#include <constants.h>
#include <model_handler.h>
#include <model_calculator.h>



class JsonRangeTest : public ::testing::Test {
protected:
    // Setup and teardown can be added here if needed

    // Hardcoded function for fiding connectV2 in Sextupole_V18_3_splice_V9.json
    static rat::mdl::ShPathConnect2Pr findConnectV2(rat::mdl::ShModelGroupPr model_tree)
    {
        rat::mdl::ShPathConnect2Pr connectV2;
        for (auto &model : model_tree->get_models())
        {
            if (model->get_name() == "Clip")
            {
                // cast to its type
                rat::mdl::ShModelClipPr clip = std::dynamic_pointer_cast<rat::mdl::ModelClip>(model);
                for (auto &model : clip->get_models())
                {
                    if (model->get_name() == "Cable (Frenet-Serret)")
                    {
                        // cast to its type
                        rat::mdl::ShModelCoilPr model_coil = std::dynamic_pointer_cast<rat::mdl::ModelCoil>(model);

                        // get the path connect2
                        rat::mdl::ShPathPr path = model_coil->get_input_path();

                        // cast to connectv2
                        if (path->get_name() == "ConnectV2 Cable in")
                        {
                            connectV2 = std::dynamic_pointer_cast<rat::mdl::PathConnect2>(path);
                            return connectV2;
                        }
                    }
                }
            }
        }
        throw std::runtime_error("ConnectV2 not found in model");
    }
};

TEST_F(JsonRangeTest, DoubleLinearTest)
{
    // Test that the function throws an exception if num_steps is less than 2
    EXPECT_THROW({
        JsonRange::double_linear(0.0, 1.0, 1);
    }, std::invalid_argument);

    // Test that the function returns start and end when num_steps is 2
    {
        auto result = JsonRange::double_linear(0.0, 1.0, 2);
        ASSERT_EQ(result.size(), 2u);
        EXPECT_DOUBLE_EQ(result[0].asDouble(), 0.0);
        EXPECT_DOUBLE_EQ(result[1].asDouble(), 1.0);
    }

    // Test that the function returns the correct linear range
    {
        size_t num_steps = 5;
        double start = 0.0;
        double end = 4.0;

        auto result = JsonRange::double_linear(start, end, num_steps);
        ASSERT_EQ(result.size(), num_steps);

        double expected_values[] = {0.0, 1.0, 2.0, 3.0, 4.0};

        for (size_t i = 0; i < num_steps; ++i)
        {
            EXPECT_DOUBLE_EQ(result[i].asDouble(), expected_values[i]);
        }
    }

    // Test with negative values
    {
        size_t num_steps = 3;
        double start = -1.0;
        double end = 1.0;

        auto result = JsonRange::double_linear(start, end, num_steps);
        ASSERT_EQ(result.size(), num_steps);

        double expected_values[] = {-1.0, 0.0, 1.0};

        for (size_t i = 0; i < num_steps; ++i)
        {
            EXPECT_DOUBLE_EQ(result[i].asDouble(), expected_values[i]);
        }
    }

    // Test when start equals end
    {
        size_t num_steps = 2;
        double start = 1.0;
        double end = 1.0;

        auto result = JsonRange::double_linear(start, end, num_steps);
        ASSERT_EQ(result.size(), num_steps);

        for (size_t i = 0; i < num_steps; ++i)
        {
            EXPECT_DOUBLE_EQ(result[i].asDouble(), 1.0);
        }
    }

    // Test when start is greater than end
    {
        size_t num_steps = 3;
        double start = 2.0;
        double end = 0.0;

        EXPECT_THROW({
            JsonRange::double_linear(start, end, num_steps);
        }, std::invalid_argument);
        
    }

    // Test with floating-point numbers
    {
        size_t num_steps = 4;
        double start = 0.1;
        double end = 0.4;

        auto result = JsonRange::double_linear(start, end, num_steps);
        ASSERT_EQ(result.size(), num_steps);

        double expected_values[] = {0.1, 0.2, 0.3, 0.4};

        for (size_t i = 0; i < num_steps; ++i)
        {
            EXPECT_NEAR(result[i].asDouble(), expected_values[i], 1e-9);
        }
    }

    // Test with a large number of steps
    {
        size_t num_steps = 10001; // odd number needed here so that there is a middle value
        double start = 0.0;
        double end = 1.0;

        auto result = JsonRange::double_linear(start, end, num_steps);
        ASSERT_EQ(result.size(), num_steps);

        // Check first, middle, and last values
        EXPECT_DOUBLE_EQ(result[0].asDouble(), 0.0);
        EXPECT_DOUBLE_EQ(result[num_steps - 1].asDouble(), 1.0);
        EXPECT_NEAR(result[num_steps / 2].asDouble(), 0.5, 1e-6);
    }

    // Test with minimal step size
    {
        size_t num_steps = 3;
        double start = 0.0;
        double end = std::numeric_limits<double>::epsilon();

        auto result = JsonRange::double_linear(start, end, num_steps);
        ASSERT_EQ(result.size(), num_steps);

        double expected_values[] = {0.0, end / 2, end};

        for (size_t i = 0; i < num_steps; ++i)
        {
            EXPECT_DOUBLE_EQ(result[i].asDouble(), expected_values[i]);
        }
    }
}

TEST_F(JsonRangeTest, PathConnect2RangeTest){
    // SetUp
    std::string filepath = TEST_DATA_DIR + "Sextupole_V18_3_splice_V9.json";
    CCTools::ModelHandler modelHandler(filepath);
    CCTools::ModelCalculator modelCalculator(modelHandler.getTempJsonPath());
    rat::mdl::ShModelGroupPr model_tree = modelCalculator.get_model_tree();
    rat::mdl::ShPathConnect2Pr connectV2 = JsonRangeTest::findConnectV2(model_tree);
    std::vector<double> lb = connectV2->get_lb();
    std::vector<double> ub = connectV2->get_ub();

    // create range
    std::vector<Json::Value> configs;
    ASSERT_NO_THROW({
        configs = JsonRange::pathconnect2_range(connectV2, 10000);
    });
    ASSERT_EQ(configs.size(), 1000);

    // check that the configs are of the correct size
    for (const auto &config : configs)
    {
        ASSERT_TRUE(config.isArray());
        ASSERT_EQ(config.size() % 6, 0);
    }

    // check that the configs are within the bounds
    for (const auto &config : configs)
    {
        for (Json::ArrayIndex i = 0; i < config.size(); i++)
        {
            double value = config[i].asDouble();
            EXPECT_GE(value, lb[i]);
            EXPECT_LE(value, ub[i]);
        }
    }

    

}

