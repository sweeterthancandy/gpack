#pragma once

#include <ostream>
#include <boost/preprocessor.hpp>

namespace gpack{ namespace tags{
        #define GPACK_TAGS/**/(bool_)(int_)(string_)(map_)(array_)(nil_)(binary_)(float32_)(float64_)(insert_)(ref_)
        #define GPACK_MAKE_TAG( r, data, tag )                                         \
                struct tag{                                                            \
                        friend std::ostream& operator<<( std::ostream& ostr, tag ) {   \
                                return ostr << BOOST_PP_STRINGIZE( tag );              \
                        }                                                              \
                        static std::string name() {                                    \
                                return BOOST_PP_STRINGIZE( tag );                      \
                        }                                                              \
                };
        BOOST_PP_SEQ_FOR_EACH(GPACK_MAKE_TAG,~,GPACK_TAGS)
        #undef GPACK_MAKE_TAG
        #undef GPACK_TAGS
} }
