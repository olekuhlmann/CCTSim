#ifndef CUBE3D_FACTORY_HH
#define CUBE3D_FACTORY_HH

#include "mesh_data_handler.h"

/**
 * @class Cube3DFactory
 * @brief Factory class for constructing a CCTools::Cube3D object.
 */
class Cube3DFactory
{
public:
    /**
     * @brief Constructs a Cube3DFactory object using two Plane Calculations from RAT-GUI.
     * @param length1 The `length1` value of both planes (mm).
     * @param length2 The `length2` value of both planes (mm).
     * @param offset_x The offset `x` value of both planes (mm).
     * @param offset_y_plane_bottom The offset `y` value of the bottom plane (mm).
     * @param offset_y_plane_top The offset `y` value of the top plane (mm).
     * @param offset_z The offset `z` value of both planes (mm).
     * @param invert_cube If true, the cube is considered inverted, meaning the cube spans across the entire coordinate space, excluding the area between the two planes.
     *
     * To visually set a cube, use two Plane Calculations (`top` and `bottom`) in RAT-GUI with `normal`=`y`.
     * Use the same `length1`, `length2`, offset `x` and offset `z` but different offset `y` settings in the two planes to
     * span the 3D cube between the two planes.
     */
    Cube3DFactory(double length1, double length2, double offset_x, double offset_y_plane_bottom, double offset_y_plane_top, double offset_z, bool invert_cube = false) : cube_(
                                                                                                                                                   (offset_x - length1 / 2) * 1e-3,
                                                                                                                                                   (offset_x + length1 / 2) * 1e-3,
                                                                                                                                                   offset_y_plane_bottom * 1e-3,
                                                                                                                                                   offset_y_plane_top * 1e-3,
                                                                                                                                                   (offset_z - length2 / 2) * 1e-3,
                                                                                                                                                   (offset_z + length2 / 2) * 1e-3,
                                                                                                                                                   invert_cube) {}

    /**
     * @brief Constructs a Cube3DFactory object using an existing Cube3D object.
     * @param cube Cube3D object. Values in (m).
     */
    Cube3DFactory(CCTools::Cube3D cube) : cube_(cube) {}

    /**
     * @brief Get the cube.
     * @returns A pointer to the Cube3D object.
     */
    const std::shared_ptr<CCTools::Cube3D> getCube()
    {
        return std::make_shared<CCTools::Cube3D>(cube_);
    }

private:
    CCTools::Cube3D cube_;
};

#endif // CUBE3D_FACTORY_HH