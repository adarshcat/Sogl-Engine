#include "util/sogl_helper.hpp"

namespace sogl
{
    
    float Helper::lerp(const float a, const float b, const float c){
        return a + (b - a)*c;
    }
    
} // namespace sogl
