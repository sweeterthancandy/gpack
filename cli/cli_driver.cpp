#include "cli_driver.h"

#include <iostream>
#include <fstream>
#include <iterator>

#include <boost/exception/all.hpp>

#include "json_parser/json_parser.h"

#include "cli.h"
#include <vt_to_string.h>

cli_driver::cli_driver(int argc, char const* argv[]){

        desc_.add_options()

                ("schema"
                 ,po::value<std::string>()
                        ->default_value("msgpack")
                ,"schema to use for serialization policy")


                /////////////////////////////////////////////
                // SOURCES
                // need to make these two are exclusive
                /////////////////////////////////////////////
                ("input-file"
                 ,po::value<std::string>()
                 ,"file to serialize")
                ("input-binary-file"
                 ,po::value<std::string>()
                 ,"file to deserialize")
                ("stdin"
                 ,po::value<bool>()
                 ->implicit_value(true)
                 ->default_value(false)
                 ,"stdin")
                
                ("input-string"
                 ,po::value<std::string>()
                 ,"file to serialize")
                /////////////////////////////////////////////
                
                
                
                /////////////////////////////////////////////
                // SINKS
                // special semantics of if no sinks use output-stdout sink
                /////////////////////////////////////////////
                ("file-sink"
                 ,po::value<std::string>()
                 ,"file to output")
                
                ("stdout-sink"
                 ,po::value<bool>()
                        ->default_value(false)
                        ->implicit_value(true)
                 ,"flag to output to terminal")
                /////////////////////////////////////////////


                /////////////////////////////////////////////
                // POST HOOKS
                /////////////////////////////////////////////
                ("checked"
                 ,po::value<bool>()
                        ->default_value(false)
                        ->implicit_value(true)
                 ,"check injective mapping")
                
                /////////////////////////////////////////////
                // MISC
                /////////////////////////////////////////////
                ("help"
                 ,po::value<bool>()
                        ->default_value(false)
                        ->implicit_value(true)
                 ,"print this message")
                ("display"
                 ,po::value<bool>()
                        ->default_value(false)
                        ->implicit_value(true)
                 ,"dispay parsed input")
                ("profile"
                 ,po::value<bool>()
                        ->default_value(false)
                        ->implicit_value(true)
                 ,"profile the serialzation")
                ;

        p.add("file",-1);

        po::store( 
                po::command_line_parser(argc,argv)
                        .options(desc_)
                        .positional(p)
                        .run()
               , vm_ 
        );
        po::notify(vm_);

}
int cli_driver::run(){

        if( vm_["help"].as<bool>()){
                std::cout << desc_ << "\n";
                return EXIT_SUCCESS;
        }
        // check excluisive 
        #if 0
        if( vm_.count("input-file") + vm_.count("input-string")  ){
                case 0:
                        std::cerr << "no input!\n";
                        return EXIT_FAILURE;
                case 1:
                        break;
                default:
                        std::cerr << "can only have one input source!";
                        return EXIT_FAILURE;
        }
        #endif


        std::string json_string;


        if( vm_.count("input-file") ){

                std::ifstream fstr;
                fstr.open(vm_["input-file"].as<std::string>());
                if( ! fstr.is_open())
                        BOOST_THROW_EXCEPTION(std::domain_error("unable to open file"));

                json_string.assign(
                        std::istream_iterator<char>(fstr),
                        (std::istream_iterator<char>()));
        } else if( vm_.count("input-string") ){

                json_string = vm_["input-string"].as<std::string>();
        } else if ( vm_.count("input-binary-file") ||
                    vm_.count("stdin") ){

                std::vector<char> mem;
                if( vm_.count("input-binary-file") ){
                        std::string fn = vm_["input-binary-file"].as<std::string>();
                        std::ifstream ifstr(fn, std::ifstream::binary);
                        if( ! ifstr.is_open())
                                BOOST_THROW_EXCEPTION(std::domain_error("can't open " + fn));
                        mem.assign(std::istream_iterator<char>(ifstr),
                                   (std::istream_iterator<char>()));
                } else{
                        mem.assign(std::istream_iterator<char>(std::cin),
                                   (std::istream_iterator<char>()));
                }

                using policy_t     = gpack::msgpack::policy;
                using parser_vec   = policy_t::parser_vec;
                using decoder_t    = gpack::decoder_kernel<parser_vec>;
                using context_t    = gpack::vt_context;
                using subscriber_t = gpack::decoder_subscriber<decoder_t,gpack::vt_context>;
                decoder_t dec;
                context_t ctx;
                dec.parse(ctx, &mem[0], &mem[0] + mem.size());
                auto root = ctx.make();
                gpack::display(root);
                std::cout << "root = " << gpack::to_string(root) << "\n";
                return EXIT_SUCCESS;
        } else{
                std::cout << "need an input\n";
                return EXIT_FAILURE;
        }

        gpack::vt_builder m;
        if( ! json_parser::try_parse(m, json_string.begin(), json_string.end()))
        {
                BOOST_THROW_EXCEPTION(std::domain_error("unable to parse json"));
        }

        gpack::variant_t root(m.make());


        auto maker = cli::policy_factory::get()
                .with_policy( vm_["schema"].as<std::string>() );

        auto target = maker.make( root );
        
        if( vm_["display"].as<bool>() ){
                maker.make_hook( "display" )->start( *target );
        }
        if( vm_["checked"].as<bool>() ){
                maker.make_hook( "checker" )->start( *target );
        }
        if( vm_["profile"].as<bool>() ){
                maker.make_hook( "profiler" )->start( *target );
        }
        if( vm_["stdout-sink"].as<bool>() ){
                maker.make_hook( "stdout-sink" )->start( *target );
        }
        if( vm_.count("file-sink") ){
                maker.make_hook( "file-sink", vm_["file-sink"].as<std::string>() )->start( *target );
        }

        target->run();

        return EXIT_SUCCESS;
}
