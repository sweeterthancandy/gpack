#pragma once

#include <iostream>

#include <boost/io/ios_state.hpp>
#include <boost/format.hpp>

#include "tags.h"

namespace gpack{

        struct cout_context{
                template<class Size_Type>
                void operator()(tags::string_, Size_Type n, const char* data){
                        std::cout << boost::format("found string of len %s \"%s\" \n")
                                % (size_t)n
                                % std::string(data,data+n)
                        ;
                }
                template<class Size_Type>
                void operator()(tags::binary_, Size_Type n, const char* data){
                        std::cout << boost::format("found binary of len %s ") % (size_t)n;
                        boost::io::ios_flags_saver ifs(std::cout);
                        std::cout << std::hex;
                        for(const char* iter=data;iter!=data+n;++iter){
                                std::cout << static_cast<unsigned>(static_cast<unsigned char>(*iter));
                        }
                        std::cout << "\n";
                }
                template<class Value_Type>
                void operator()(tags::int_, Value_Type&& value){
                        std::cout << boost::format("found integer of value %s\n")
                                % (ssize_t)value
                        ;
                }
                template<class Value_Type>
                void operator()(tags::bool_, Value_Type&& value){
                        std::cout << boost::format("found bool %s\n")
                                % ( value ? "true" : "false" )
                        ;
                }
                template<class Value_Type>
                void operator()(tags::nil_, Value_Type&& value){
                        std::cout << "found (nil)\n";
                }
                template<class Value_Type>
                void operator()(tags::float32_, Value_Type&& value){
                        std::cout << boost::format("found float32 %s\n") % value;
                }
                template<class Value_Type>
                void operator()(tags::float64_, Value_Type&& value){
                        std::cout << boost::format("found float64 %s\n") % value;
                }
                template<class Size_Type>
                void operator()(tags::insert_, Size_Type count){
                        std::cout << boost::format("found insert %s\n") % count;
                }
                template<class Size_Type>
                void operator()(tags::ref_, Size_Type index){
                        std::cout << boost::format("found ref %s\n") % index;
                }
                template<class Size_Type>
                void operator()(tags::map_, Size_Type n){
                        std::cout << boost::format("found map of len %s\n")
                                % (size_t)n
                        ;
                }
                template<class Size_Type>
                void operator()(tags::array_, Size_Type n){
                        std::cout << boost::format("found array of len %s\n")
                                % (size_t)n
                        ;
                }
        };

}
