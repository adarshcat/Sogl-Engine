#pragma once

namespace sogl
{
    namespace helper
    {
        static float lerp(const float a, const float b, const float c){
            return a + (b - a)*c;
        }
    } // namespace helper
    
} // namespace sogl
