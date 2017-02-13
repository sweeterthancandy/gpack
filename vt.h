#pragma once

#include <iostream>
#include <iomanip>
#include <iterator>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/preprocessor.hpp>
#include <boost/variant.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>

#include "base.h"


// only difference between primitive_t and variant_t is that 
// a map M can only be of the form
//      M: primitive_t -> variant_t


namespace gpack{

        #define GPACK_VARIANT_PRIMITVES/**/\
                (std::int64_t)(std::string)(bool)\
                (std::nullptr_t)(float32_t)(float64_t)
        
        struct binary_{
        private:
                using vector_type    = std::vector<char>;
        public:
                using iterator       = vector_type::iterator;
                using const_iterator = vector_type::const_iterator;

                binary_()=default;
                template<class Iter>
                binary_(Iter first, Iter last):data_(first,last){}
                auto size()const{return data_.size();}

                const_iterator begin()const{return data_.begin();}
                const_iterator end()const{return data_.end();}

                iterator begin(){return data_.begin();}
                iterator end(){return data_.end();}

                auto data(){return data_.data();}
                auto data()const{return data_.data();}

                friend bool operator==(binary_ const& left, binary_ const& right){
                        return boost::equal(left,right);
                }
        private:
                std::vector<char> data_;
        };

        using primitive_vec = mpl::vector< BOOST_PP_SEQ_ENUM(GPACK_VARIANT_PRIMITVES) >;

        using primitive_t = boost::variant< BOOST_PP_SEQ_ENUM(GPACK_VARIANT_PRIMITVES) >;

        struct array_;
        struct map_;

        /*
         * string reference sematntics
         * ===========================
         *
         * a transform can be made so that n occurences of a string can reference
         * the same data
         *
         * implementation
         * ==============
         *
         * rather than an explicit dictionary all we need to a logical one with the
         * concept of modifiers
         *
         * modifier -> push | access | remove
         *
         * reference -> modifier args...
         *
         * std::list<variant_t> dict;
         *
         * push args... -> push type     { expr = ( dict.push_back(type),type) }
         * access args... -> access int_ { expr = ( * std::next( dict.begin(), int_ ) ) }
         * remove args... -> remove int_ { expr = (auto iter = std::next(dict.begin(),int_),auto tmp=*iter, dict.erase(iter), tmp) }
         *
         */

        struct insert_;
        struct ref_{
                explicit ref_(std::uint64_t idx):idx_(idx){}
                auto index()const{return idx_;}
        private:
                std::uint64_t idx_;
        };

        using variant_t = boost::make_recursive_variant<
                BOOST_PP_SEQ_ENUM(GPACK_VARIANT_PRIMITVES),
                binary_,
                ref_,
                boost::recursive_wrapper<array_>,
                boost::recursive_wrapper<map_>,
                boost::recursive_wrapper<insert_>
        >::type;

        struct array_{
        private:
                using vector_type    = std::vector<variant_t>;
        public:
                using iterator       = vector_type::iterator;
                using const_iterator = vector_type::const_iterator;
                array_()=default;
                explicit array_(std::vector<variant_t> const&  data):data_(data){}
                explicit array_(std::vector<variant_t>      && data):data_(std::move(data)){}

                template<class T>
                void emplace_back(T&& val){
                        data_.emplace_back(std::forward<T>(val));
                }
                template<class T>
                void push_back(T&& val){
                        data_.push_back(std::forward<T>(val));
                }
                auto size()const{return data_.size();}
                const_iterator begin()const{return data_.begin();}
                const_iterator end()const{return data_.end();}

                decltype(auto) operator[](size_t idx){
                        return (data_[idx]);
                }
                decltype(auto) operator[](size_t idx)const{
                        return (data_[idx]);
                }

                iterator begin(){return data_.begin();}
                iterator end(){return data_.end();}
                
                template<class F>
                void for_each(F&& f){
                        boost::for_each( data_, f);
                }
                template<class F>
                void for_each(F&& f)const{
                        boost::for_each( data_, f);
                }
        private:
                vector_type data_;
        };
        struct map_{
                using key_t = variant_t;
                using value_t = variant_t;
        private:
                using vector_type    = std::vector<std::pair<key_t,value_t> >;
        public:
                using iterator       = vector_type::iterator;
                using const_iterator = vector_type::const_iterator;

                template<class Key, class Value>
                void emplace_back(Key&& key, Value&& value){
                        data_.emplace_back(
                                std::forward<Key>(key),
                                std::forward<Value>(value)
                        );
                }
                auto size()const{return data_.size();}
                const_iterator begin()const{return data_.begin();}
                const_iterator end()const{return data_.end();}

                iterator begin(){return data_.begin();}
                iterator end(){return data_.end();}

                template<class F>
                void for_each(F&& f){
                        boost::for_each( data_, f);
                }
                template<class F>
                void for_each(F&& f)const{
                        boost::for_each( data_, f);
                }

        private:
                vector_type data_;
        };

        struct insert_{
                explicit insert_( variant_t const& var, size_t _count)
                        :var_(var),count_(_count)
                {}
                variant_t const& get()const{return var_;}
                auto count()const{return count_;}
        private:
                variant_t var_;
                size_t count_;
        };

        namespace detail{

                // cast variant_t to primitive_t
                struct primitive_cast : boost::static_visitor<primitive_t>
                {
                        template<class T>
                        typename std::enable_if<
                                boost::mpl::contains<
                                        boost::mpl::vector<BOOST_PP_SEQ_ENUM(GPACK_VARIANT_PRIMITVES)>,
                                        typename std::decay<T>::type
                                >::value,
                        primitive_t>::type
                        operator()(T&& value)const{
                                return primitive_t{std::forward<T>(value)};
                        }
                        template<class T>
                        [[noreturn]]
                        typename std::enable_if<
                                ! boost::mpl::contains<
                                        boost::mpl::vector<BOOST_PP_SEQ_ENUM(GPACK_VARIANT_PRIMITVES)>,
                                        typename std::decay<T>::type
                                >::value,
                        primitive_t>::type
                        operator()(T&& )const{
                                BOOST_THROW_EXCEPTION(std::domain_error("not a primitive"));
                                __builtin_unreachable();
                        }

                };
        }

        #undef GPACK_VARIANT_PRIMITVES

}

