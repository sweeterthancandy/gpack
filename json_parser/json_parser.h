#pragma once

#define PRINT(X) do{ std::cout << #X " => " << (X) << "\n";}while(0)

#include "json_parser_tree.h"
#include "json_parser_maker.h"
#include "json_parser_io.h"
#include "json_parser_tokenizer.h"
#include "json_parser_fast_parser.h"
#include "json_parser_xpr_parser.h"

namespace json_parser{
        template<class Iter>
        auto parse(Iter first, Iter last)->boost::optional<node>{
                //detail::xpr_parser p;
                maker m;
                detail::basic_parser<maker,Iter> p(m,first,last);
                try{ 
                        p.parse();
                } catch(std::exception const& e){
                        return boost::none;
                }
                return m.make();
        }
        template<class Maker, class Iter>
        auto parse(Maker& m, Iter first, Iter last)->bool{
                //detail::xpr_parser p;
                detail::basic_parser<Maker,Iter> p(m,first,last);
                try{ 
                        // this throws if ! eos
                        p.parse();
                        assert( p.eos() && "post condition violated");
                        return true;
                } catch(std::exception const& e){
                        return false;
                }
                __builtin_unreachable();
        }
        decltype(auto) parse(std::string const& s){
                return parse(s.begin(), s.end());
        }

        namespace test__{
                void test0(){
                        std::vector<std::string> test_strings = {
                                "{}",
                                "[]",
                                "   {  }    ",
                                "  [ ]   ",

                                "{1:{}}",
                                "[[]]",
                                "[{}]",
                                "{1:[]}",
                                " { 1:{ } } ",
                                " [ [ ] ] ",
                                " [ { } ] ",
                                " { 1:[ ] } ",

                                "[1]",
                                "[1,2]",
                                "[{},1]",
                                "[1,{}]",
                                "{1:1,1:1}",
                                "{1:1,1:{}}",
                                "{1:{},1:{}}",
                                "{1:[],1:[]}",
                                "{1:[],1:[1,2]}",
                                
                                R"([23])",
                                R"([+23])",
                                R"([-3455])",
                                R"([23.433])",
                                R"(["hello"])",
                                R"([null])",
                                R"([true])",
                                R"([false])",
                                R"([23,23])",
                                R"([-3455,-3455])",
                                R"([23.433,23.433])",
                                R"(["hello","hello"])",
                                R"([null,null])",
                                R"([true,true])",
                                R"([false,false])",
                                R"([23])",
                                R"([23,-3455])",
                                R"([23,-3455,23.433])",
                                R"([23,-3455,23.433,"hello"])",
                                R"([23,-3455,23.433,"hello",null])",
                                R"([23,-3455,23.433,"hello",null,true])",
                                R"([23,-3455,23.433,"hello",null,true,false])",
                                
                                R"( [ 23 ] )" , 
                                R"( [ +23 ] )" , 
                                R"( [ -3455 ] )" , 
                                R"( [ 23.433 ] )" , 
                                R"( [ "hello" ] )" , 
                                R"( [ null ] )" , 
                                R"( [ true ] )" , 
                                R"( [ false ] )" , 
                                R"( [ 23 , 23 ] )" , 
                                R"( [ -3455 , -3455 ] )" , 
                                R"( [ 23.433 , 23.433 ] )" , 
                                R"( [ "hello" , "hello" ] )" , 
                                R"( [ null , null ] )" , 
                                R"( [ true , true ] )" , 
                                R"( [ false , false ] )" , 
                                R"( [ 23 ] )" , 
                                R"( [ 23 , -3455 ] )" , 
                                R"( [ 23 , -3455 , 23.433 ] )" , 
                                R"( [ 23 , -3455 , 23.433 , "hello" ] )" , 
                                R"( [ 23 , -3455 , 23.433 , "hello" , null ] )" , 
                                R"( [ 23 , -3455 , 23.433 , "hello" , null , true ] )" , 
                                R"( [ 23 , -3455 , 23.433 , "hello" , null , true , false ] )" , 
                                
                                R"( ["|","|","|","|","|","|","|","|"] )" , 
                                R"( ["|","|","_","_","_","_","|","|"] )" , 
                                R"( ["|","|","|","|","_","_","_","_"] )" , 
                                R"( ["_","|","|","|","_","_","_","_"] )" , 
                                R"( ["_","|","a","|","_","_","_","a"] )" , 

                                "{}"
                        };
                }
        }

}

