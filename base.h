#pragma once

namespace boost{ namespace mpl { } } 

namespace gpack{
        namespace mpl = boost::mpl;
        using uchar = unsigned char;
        
        using float32_t = float;
        using float64_t = double;

        static_assert(sizeof(float32_t) == 4 ,"");
        static_assert(sizeof(float64_t) == 8 ,"");
}
