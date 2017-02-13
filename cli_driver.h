#pragma once

#include <boost/program_options.hpp>


namespace po = boost::program_options;

struct cli_driver{
        cli_driver(int argc, char const* argv[]);
        int run();
private:
        po::options_description desc_;
        po::positional_options_description p;
        po::variables_map vm_;
};
