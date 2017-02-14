#pragma once

#include <sstream>

#include "vt.h"
#include "vt_to_string.h"

namespace gpack{
        /*
         * This is to construct a vt (a recursve structure), with a simple class
         *
         * vt_builder builder;
         *
         * builder.begin_map();
         *      builder.make_int(2);
         *      builder.begin_array();
         *              builder.make_string("one");
         *      builder.end_array();
         * builder.end_map();
         *
         * variant_t root = builder.make();
         *
         */
        struct vt_builder{
        private:
                struct detail{
                        struct start_{
                                template<class T>
                                void append_(T&& value){
                                        if( opt_ )
                                                BOOST_THROW_EXCEPTION(std::domain_error("can only have one root"));
                                        opt_ = value;
                                }
                                variant_t to_terminal()const{
                                        if( opt_ )
                                                return opt_.get();
                                        BOOST_THROW_EXCEPTION(std::domain_error("start_ has no variant_t"));
                                        __builtin_unreachable();
                                }
                                std::string to_string()const{
                                        if( opt_ )
                                                return str(boost::format("start_(true)") % ::gpack::to_string(opt_.get()));
                                        return "start_(false)";
                                }
                                void display_()const{
                                        if( opt_)
                                                display(opt_.get());
                                        std::cout << "(none)\n";
                                }

                        private:
                                boost::optional<variant_t> opt_;
                        };
                        struct making_array{
                                template<class T>
                                void append_(T&& value){
                                        data_.emplace_back(std::forward<T>(value));
                                }
                                array_ const& to_terminal()const{return data_;}
                                std::string to_string()const{
                                        return str(boost::format("making_array(%s)") % ::gpack::to_string(data_) );
                                }
                                void display_(){
                                        display(data_);
                                }
                        private:
                                array_ data_;
                        };
                        struct making_map{
                                // to add {"one":1}, it goes
                                //      begin_map();
                                //              make_string("one");
                                //              make_int(1);
                                //      end_map();
                                //
                                // thus, want to buffer the first one
                                template<class T>
                                void append_(T&& value){
                                        if( key_ ){
                                                data_.emplace_back(key_.get(), value );
                                                key_ = boost::none;
                                        } else{
                                                key_ = std::forward<T>(value);
                                        }
                                }
                                map_ const& to_terminal()const{
                                        if( key_ )
                                                BOOST_THROW_EXCEPTION(std::domain_error("having matched keys with values"));
                                        return data_;
                                }
                                std::string to_string()const{
                                        return str(boost::format("making_map(%s%s)") 
                                                        % ( key_ ? std::string("(none)") : ::gpack::to_string(key_.get()) )
                                                        % ::gpack::to_string(data_) 
                                                );
                                }
                                void display_(){
                                        if( key_ )
                                                display( key_.get() );
                                        else
                                                std::cout << "(none)\n";
                                        display(data_);
                                }
                        private:
                                boost::optional<variant_t> key_;
                                map_ data_;
                        };
                };

                using state_t = boost::variant<
                        detail::start_,
                        detail::making_array,
                        detail::making_map
                >;

                // for primitives
                template<class T>
                struct append_ : boost::static_visitor<>
                {
                        explicit append_(T const& value):value_(value){}

                        template<class State>
                        void operator()(State&& state)const{
                                state.append_(std::move(value_));
                        }
                private:
                        T value_;
                };
                template<class T>
                static append_<std::decay_t<T>> make_append_(T&& val){
                        return append_<std::decay_t<T>>{std::forward<T>(val)};
                }

                struct node_cast : boost::static_visitor<variant_t>{
                        template<class State>
                        variant_t operator()(State&& state)const{
                                return state.to_terminal();
                        }
                };
        public:

                void begin_map(){
                        state_.emplace_back( detail::making_map() );
                } 
                void end_map(){
                        detail::making_map aux = std::move(boost::get<detail::making_map>(state_.back()));
                        state_.pop_back();
                        boost::apply_visitor( make_append_(aux.to_terminal()), state_.back() );
                } 
                void begin_array(){
                        state_.emplace_back( detail::making_array() );
                } 
                void end_array(){
                        detail::making_array aux = std::move(boost::get<detail::making_array>(state_.back()));
                        state_.pop_back();
                        boost::apply_visitor( make_append_(aux.to_terminal()), state_.back() );
                } 
                void make_string(std::string const& value){
                        boost::apply_visitor( make_append_(value), state_.back() );
                }
                void make_int(std::int64_t value){
                        boost::apply_visitor( make_append_(value), state_.back() );
                }
                void make_float32(float32_t value){
                        boost::apply_visitor( make_append_(value), state_.back() );
                }
                void make_float64(float64_t value){
                        boost::apply_visitor( make_append_(value), state_.back() );
                }
                void make_float(float64_t value){
                        boost::apply_visitor( make_append_(value), state_.back() );
                }
                void make_nil(){
                        boost::apply_visitor( make_append_(nullptr), state_.back() );
                }
                void make_null(){
                        boost::apply_visitor( make_append_(nullptr), state_.back() );
                }
                void make_bool(bool value){
                        boost::apply_visitor( make_append_(value), state_.back() );
                }
                void make_true(){
                        boost::apply_visitor( make_append_(true), state_.back() );
                }
                void make_false(){
                        boost::apply_visitor( make_append_(false), state_.back() );
                }
                void append(variant_t const& vt){
                        boost::apply_visitor( make_append_(vt), state_.back() );
                }

                variant_t make(){ 
                        switch(state_.size()){
                                case 1:
                                        return boost::apply_visitor( node_cast(), state_.back());
                                default:
                                        debug_();
                                        BOOST_THROW_EXCEPTION(std::domain_error("too many beings, not ehough ends"));
                        }

                } 
                void debug_() {
                        boost::for_each( state_, []( auto&& s ) {
                                boost::apply_visitor( []( auto&& v ) {
                                        std::cout << "   " << v.to_string() << "\n";
                                        v.display_();
                                }, s );
                        } );
                }
                void reset(){
                        state_.clear();
                        state_.emplace_back( detail::start_() );
                }
        private:
                std::vector<state_t> state_{detail::start_()};
        };
}
