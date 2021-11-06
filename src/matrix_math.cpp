#include "matrix_math.h"

namespace matrices
{

matrix_3d rotation_z_axis( double angle )
{
    return matrix_3d( {
        std::cos( angle ), -std::sin( angle ), 0,
        std::sin( angle ), std::cos( angle ), 0,
        0, 0, 1
    } );
}

} // namespace matrices
