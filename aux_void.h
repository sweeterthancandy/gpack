#pragma once

#include <ostream>

namespace gpack{
        struct void_{
                friend std::ostream& operator<<(std::ostream& ostr, void_){
                        return ostr << "(void_)";
                }
        };
};
