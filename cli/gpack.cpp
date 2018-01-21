#include "cli_driver.h"

#include <vector>
#include <iostream>

#include <boost/exception/all.hpp>

int main(int argc, char* argv[]){
        try{
                if( argc <= 1 ){
                        std::vector<const char*> sbuf = {
                                "(dummy)"
                                , "--input-file"
                                , "test.json"
                                , "--checked"
                                , nullptr
                        };
                        cli_driver drv( sbuf.size()-1, &sbuf.front() );
                        return drv.run();
                } else{
                        cli_driver drv(argc,const_cast<const char**>(argv));
                        return drv.run();
                }
                __builtin_unreachable();
        } catch( const std::exception& e){
                std::cerr 
                        << "Caught exception: "
                        << boost::diagnostic_information(e)
                        ;
                return EXIT_FAILURE;
        }
}
