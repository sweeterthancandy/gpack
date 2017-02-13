#pragma once

#include <list>
#include <string>
#include <iostream>

#include <boost/mpl/map.hpp>
#include <boost/range.hpp>
#include <boost/format.hpp>

#include "vt.h"
#include "vt_builder.h"
#include "ref_table.h"
#include "tags.h"

namespace gpack{

        /*
         * This is the context for the decoder to create 
         * a variant tree
         *
         */
        struct vt_context : boost::noncopyable{

        private:

                struct internal_context{
                        vt_builder& maker(){return maker_stack_.back();}
                        void maker_push(){ maker_stack_.emplace_back(); }
                        void maker_pop(){ maker_stack_.pop_back(); }
                        decltype(auto) rt(){return (rt_);}
                private:
                        std::list<vt_builder> maker_stack_{vt_builder()};
                        detail::ref_table rt_;
                };

                struct start_{
                        bool down_(internal_context&){return false;}
                        std::string to_string()const{return "start_";}
                };
                struct making_map{
                        explicit making_map(size_t n):n_(n){assert(n!=0 && "precondition failed");}
                        bool down_(internal_context& ictx){
                                if( --n_ == 0){
                                        ictx.maker().end_map();
                                        return true;
                                }
                                return false;
                        }
                        std::string to_string()const{return str(boost::format("making_map(%i)") % n_);}
                private:
                        size_t n_;
                };
                struct making_array{
                        explicit making_array(size_t n):n_(n){assert(n!=0 && "precondition failed");}
                        bool down_(internal_context& ictx){
                                if( --n_ == 0){
                                        ictx.maker().end_array();
                                        return true;
                                }
                                return false;
                        }
                        std::string to_string()const{return str(boost::format("making_array(%i)") % n_);}
                private:
                        size_t n_;
                };
                struct inserting{
                        explicit inserting(size_t count, internal_context& ictx)
                                :count_(count)
                        {
                                ictx.maker_push();
                                ictx.maker().begin_array();
                        }
                        bool down_(internal_context& ictx){
                                ictx.maker().end_array();
                                auto root = ictx.maker().make();
                                array_ a = boost::get<array_>(root);
                                assert( a.size() == 1 && "unexpected" );
                                ictx.maker_pop();
                                auto e = a[0];
                                ictx.rt().insert( e, count_ );
                                ictx.maker().append(e);
                                //ictx.rt().debug_();
                                return true;
                        }
                        std::string to_string()const{return str(boost::format("making_ref"));}
                private:
                        size_t count_;
                };

                using var_t = boost::variant<start_,making_map,making_array,inserting>;

        public:
                auto make(){
                        return ictx_.maker().make();
                }

                template<class Size_Type>
                void operator()(tags::string_, Size_Type n, const char* data){
                        ictx_.maker().make_string( std::string(data,data+n) );
                        down_();
                }
                template<class Size_Type>
                void operator()(tags::binary_, Size_Type , const char* ){
                        BOOST_THROW_EXCEPTION(std::domain_error("unsupported"));
                        __builtin_unreachable();
                }

                template<class Int_Type>
                void operator()(tags::int_, Int_Type value){
                        ictx_.maker().make_int(value);
                        down_();
                }
                void operator()(tags::bool_, bool value){
                        ictx_.maker().make_bool(value);
                        down_();
                }
                template<class Float_Type>
                void operator()(tags::float32_, Float_Type value){
                        ictx_.maker().make_float(value);
                        down_();
                }
                template<class Float_Type>
                void operator()(tags::float64_, Float_Type value){
                        ictx_.maker().make_float(value);
                        down_();
                }
                template<class Dummy>
                void operator()(tags::nil_,Dummy&&){
                        ictx_.maker().make_null();
                        down_();
                }

                template<class Size_Type>
                void operator()(tags::map_, Size_Type n){
                        if( n == 0 ){
                                ictx_.maker().begin_map();
                                ictx_.maker().end_map();
                                down_();
                        } else{
                                ictx_.maker().begin_map();
                                stack_.emplace_back( making_map(n*2) );
                        }

                }
                template<class Size_Type>
                void operator()(tags::array_, Size_Type n){
                        if( n == 0 ){
                                ictx_.maker().begin_array();
                                ictx_.maker().end_array();
                                down_();
                        } else{
                                ictx_.maker().begin_array();
                                stack_.emplace_back( making_array(n) );
                        }
                }
                template<class Size_Type>
                void operator()(tags::insert_, Size_Type count){
                        //std::cout << boost::format("vt_context::operator()(tags::insert_,%i)\n") % count;
                        stack_.emplace_back( inserting(count,ictx_) );
                        // NOTE: no down_() call
                }
                template<class Size_Type>
                void operator()(tags::ref_, Size_Type idx){
                        //std::cout << boost::format("vt_context::operator()(tags::ref_,%i)") % idx << std::endl;
                        //ictx_.rt().debug_();
                        auto vt = ictx_.rt().lookup(idx);
                        //std::cout << "   => " << vt << std::endl;
                        ictx_.maker().append( vt );
                        ictx_.rt().down(idx);
                        //ictx_.rt().debug_();
                        down_();
                }
                void debug_(){
                        std::cout << ">>>>> begining debug output >>>>\n";
                        boost::for_each( stack_, [](auto& state){
                                std::cout << "   " << boost::apply_visitor( [](auto&& s){return s.to_string();}, state) << "\n";
                        });
                        std::cout << "<<<<< ending debug output <<<<<<\n";
                }
        private:
                void down_(){
                        for(; boost::apply_visitor( [&](auto&& s){ return s.down_(ictx_); }, stack_.back() ) ; ){
                                stack_.pop_back();
                        }
                }
                internal_context ictx_;
                std::vector<var_t> stack_{start_()};
        };


}
