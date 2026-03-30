
#pragma once

#include "macros.hpp"


// FORWARD DECLARATIONS =====================================================================================

namespace CTRL::math {

    // CONSTANTS ============================================================================================

    // MACROS ===============================================================================================

    // TYPES ================================================================================================

    // STATIC VARIABLES =====================================================================================

    // FUNCTION DECLARATION =================================================================================
    
    // @brief Constrains a value between minimum and maximum bounds
    // @tparam T Type that supports size comparison operations
    // @param value Input value to clamp
    // @param min Lower bound for clamping
    // @param max Upper bound for clamping
    // @return Value constrained to the range [min, max]
    template<typename T>
    static FORCE_INLINE T clamp(const T value, const T min, const T max) { return (value < min) ? min : (value > max) ? max : value; }

    // TEMPLATE DECLARATION =================================================================================

    // CLASS DECLARATION ====================================================================================

}
