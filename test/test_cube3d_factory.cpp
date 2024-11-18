#include "gtest/gtest.h"
#include "output_max_curvature.hh"

class Cube3DFactoryTest : public ::testing::Test
{
protected:
    // Setup and teardown can be added here if needed
};

TEST_F(Cube3DFactoryTest, ConstructorAndGetterTest)
{
    std::shared_ptr<Cube3DFactory> cube_factory;

    // Create object
    ASSERT_NO_THROW({
        cube_factory = std::make_shared<Cube3DFactory>(Cube3DFactory(56, 74, 56, 27, 52, 170));
    });

    // Get object and make sure the values are correct
    std::shared_ptr<CCTools::Cube3D> cube_ptr;

    ASSERT_NO_THROW({
        cube_ptr = cube_factory->getCube();
    });

    CCTools::Cube3D cube = *cube_ptr;

    ASSERT_NEAR(cube.x_min, 28e-3, 1e-08);
    ASSERT_NEAR(cube.x_max, 84e-3, 1e-08);
    ASSERT_NEAR(cube.y_min, 27e-3, 1e-08);
    ASSERT_NEAR(cube.y_max, 52e-3, 1e-08);
    ASSERT_NEAR(cube.z_min, 133e-3, 1e-08);
    ASSERT_NEAR(cube.z_max, 207e-3, 1e-08);
}
