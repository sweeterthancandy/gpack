#pragma once


#include <boost/none.hpp>
#include <boost/format.hpp>

#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>
#include <boost/xpressive/regex_constants.hpp>

/*
 * json -> { key_value_pairs_opt } 
 * key_value_pairs_opt -> \epsilon | key_value_pairs
 * key_value_pairs -> key_value | key_value , key_value_pairs
 * key_value -> prim : prim
 * prim -> map | array | null | true | false | int | string
 *
 *
 */

namespace json_parser{ namespace detail{

        namespace xpr = boost::xpressive;

        struct context{
                maker maker_;
        };

        struct on_string_impl{
                using result_type = void;

                void operator()(context& ctx, std::string const& value)const{
                        ctx.maker_.make_string(value.substr(
                                1,value.size()-2
                        ));
                        std::cout << boost::format("on_string(%s)\n") % value;
                }
        };
        struct on_int_impl{
                using result_type = void;

                void operator()(context& ctx, std::string const& value)const{
                        ctx.maker_.make_int(boost::lexical_cast<std::int64_t>(value));
                        std::cout << boost::format("on_int(%s)\n") % value;
                }
        };
        struct on_float_impl{
                using result_type = void;

                void operator()(context& , std::string const& value)const{
                        //ctx.maker_.maker_float(value);
                        std::cout << boost::format("on_float(%s)\n") % value;
                }
        };
        struct on_id_impl{
                using result_type = void;

                void operator()(context& ctx, std::string const& value)const{
                        std::cout << boost::format("on_id(%s)\n") % value;
                        if( value == "null"){
                                ctx.maker_.make_null();
                        } else if( value == "true"){
                                ctx.maker_.make_true();
                        } else if( value == "false"){
                                ctx.maker_.make_false();
                        } else{
                                BOOST_THROW_EXCEPTION(std::domain_error("unknonwn id (" + value + ")"));
                        }
                }
        };
        struct on_map_begin_impl{
                using result_type = void;

                void operator()(context& ctx)const{
                        ctx.maker_.begin_map();
                        std::cout << "on_map_begin()\n";
                }
        };
        struct on_map_end_impl{
                using result_type = void;

                void operator()(context& ctx)const{
                        ctx.maker_.end_map();
                        std::cout << "on_map_end()\n";
                }
        };
        struct on_array_begin_impl{
                using result_type = void;

                void operator()(context& ctx)const{
                        ctx.maker_.begin_array();
                        std::cout << "on_array_begin()\n";
                }
        };
        struct on_array_end_impl{
                using result_type = void;

                void operator()(context& ctx)const{
                        ctx.maker_.end_array();
                        std::cout << "on_array_end()\n";
                }
        };

        xpr::function<on_string_impl>::type const on_string = {{}};
        xpr::function<on_int_impl>::type const on_int = {{}};
        xpr::function<on_float_impl>::type const on_float = {{}};
        xpr::function<on_id_impl>::type const on_id = {{}};
        xpr::function<on_map_end_impl>::type const on_map_end = {{}};
        xpr::function<on_map_begin_impl>::type const on_map_begin = {{}};
        xpr::function<on_array_end_impl>::type const on_array_end = {{}};
        xpr::function<on_array_begin_impl>::type const on_array_begin = {{}};

        struct xpr_parser{
                xpr_parser()
                {
                        using namespace xpr;

                        int_ = (s1=+_d)                                           [on_int(ref(ctx_),s1)];
                        float_ = (s1=+_d >> "." >> +_d)                            [on_float(ref(ctx_),s1)];
                        string_ = (s1=(as_xpr("\"") >> +(~(set='"')) >> "\""))    [on_string(ref(ctx_),s1)];
                        id_ = (s1 = ( (alpha | '_' ) >> *_w ) )                   [on_id(ref(ctx_),s1)];
                        
                        prim_ = string_ | int_ | float_;

                        prim_or_obj_ = prim_ | by_ref( obj_ ) | id_;
                        
                        map_ =
                                as_xpr("{")                                  [on_map_begin(ref(ctx_))]
                                >>  skip(_s)(!(
                                        prim_ >> ":" >> prim_or_obj_ >> *( 
                                                "," >> prim_ >> ":" >> prim_or_obj_ 
                                        )     
                                ))
                                >> as_xpr("}")                                     [on_map_end(ref(ctx_))]
                        ;
                        
                        array_ =
                                as_xpr("[")                         [on_array_begin(ref(ctx_))] 
                                >>  skip(_s)(!(
                                        prim_or_obj_ >> *( 
                                                "," >> prim_or_obj_
                                        )     
                                ))
                                >> as_xpr("]")                                     [on_array_end(ref(ctx_))]
                        ;

                        obj_ = map_ | array_;

                        root_ = skip(_s)(map_);

                }
                template<class Iter>
                boost::optional<node> parse(Iter first, Iter last){
                        ctx_.maker_.reset();
                        if( xpr::regex_match( first, last, root_ ) )
                                return ctx_.maker_.make();
                        return boost::none;
                }
        private:
                context ctx_;
                xpr::sregex map_, prim_, int_, string_, array_, root_, obj_, prim_or_obj_, float_, id_;
        };
} }
