
#pragma once



// FORWARD DECLARATIONS =====================================================================================

namespace CTRL {

    // CONSTANTS ============================================================================================

    // MACROS ===============================================================================================
    
    // Force code to be inline
    #define FORCE_INLINE                                    inline __attribute__((always_inline))
    
    // getters ---------------------------------------------------------------------------------------------------------

    #define DEFAULT_GETTER(type, name)				        [[nodiscard]] FORCE_INLINE type get_##name() { return m_##name; }
    #define DEFAULT_GETTER_P(type, name)				    [[nodiscard]] FORCE_INLINE type* get_##name() { return m_##name.get(); }
    #define DEFAULT_GETTER_REF(type, name)			        [[nodiscard]] FORCE_INLINE type& get_##name##_ref() { return m_##name; }
    #define DEFAULT_GETTER_C(type, name)			        [[nodiscard]] FORCE_INLINE type get_##name() const { return m_##name; }
    #define DEFAULT_GETTER_CC(type, name)			        [[nodiscard]] FORCE_INLINE const type get_##name() const { return m_##name; }
    #define DEFAULT_GETTER_POINTER(type, name)		        [[nodiscard]] FORCE_INLINE type* get_##name##_pointer() { return &m_##name; }

    #define DEFAULT_GETTERS(type, name)				        DEFAULT_GETTER(type, name)					    \
                                                            DEFAULT_GETTER_REF(type, name)					\
                                                            DEFAULT_GETTER_POINTER(type, name)

    #define DEFAULT_GETTERS_C(type, name)			        DEFAULT_GETTER_C(type, name)			        \
                                                            DEFAULT_GETTER_POINTER(type, name)

    #define GETTER(type, func_name, var_name)		        [[nodiscard]] FORCE_INLINE type get_##func_name() { return var_name; }
    #define GETTER_C(type, func_name, var_name)		        [[nodiscard]] FORCE_INLINE const type get_##func_name() const { return var_name; }

    // setters ---------------------------------------------------------------------------------------------------------

    #define DEFAULT_SETTER(type, name)				        FORCE_INLINE void set_##name(const type name) { m_##name = name; }
    #define SETTER(type, func_name, var_name)               FORCE_INLINE void set_##func_name(const type value) { var_name = value; }

    // both together ---------------------------------------------------------------------------------------------------

    #define DEFAULT_GETTER_SETTER(type, name)				DEFAULT_GETTER(type, name)				        \
                                                            DEFAULT_SETTER(type, name)

    #define DEFAULT_GETTER_SETTER_C(type, name)				DEFAULT_GETTER_C(type, name)			        \
                                                            DEFAULT_SETTER(type, name)

    #define DEFAULT_GETTER_SETTER_ALL(type, name)			DEFAULT_SETTER(type, name)				        \
                                                            DEFAULT_GETTER(type, name)				        \
                                                            DEFAULT_GETTER_POINTER(type, name)

    #define GETTER_SETTER(type, func_name, var_name)		GETTER(type, func_name, var_name)		        \
                                                            SETTER(type, func_name, var_name)

    #define GETTER_SETTER_C(type, func_name, var_name)		GETTER_C(type, func_name, var_name)	            \
                                                            SETTER(type, func_name, var_name)

    // util ------------------------------------------------------------------------------------------------------------

    #define BIT(x)													(1 << x)

    #define BIND_FUNCTION(x)										std::bind(&x, this, std::placeholders::_1)

    // TYPES ================================================================================================

    // STATIC VARIABLES =====================================================================================

    // FUNCTION DECLARATION =================================================================================

    // TEMPLATE DECLARATION =================================================================================

    // CLASS DECLARATION ====================================================================================

}
