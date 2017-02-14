#pragma once

#include <map>
#include <functional>
#include <iostream>

#include <boost/type_index.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/format.hpp>

#include <boost/mpl/string.hpp>
#include <boost/mpl/joint_view.hpp>

namespace gpack{

        namespace detail{
                struct dsl_dirty_profiler{
                        template<class T>
                        void hit(){
                                ++m_[ boost::typeindex::type_id<T>().pretty_name() ];
                        }
                        void print(){
                                std::vector<std::pair<std::string,size_t> > aux;
                                boost::for_each(m_, [&](auto&& p){
                                        aux.push_back(p);
                                });
                                boost::sort( aux, [](auto&& l, auto&& r){return l.second < r.second;});
                                boost::for_each( aux , [](auto&& p){
                                        std::cout << boost::format("%-10i => %s\n")
                                                % p.first
                                                % p.second
                                        ;
                                });
                        }
                        static dsl_dirty_profiler& get(){
                                static std::unique_ptr<dsl_dirty_profiler> mem_(new dsl_dirty_profiler);
                                return *mem_;
                        }
                        void reset(){
                                m_.clear();
                        }
                private:
                        dsl_dirty_profiler()=default;
                        std::map<std::string,size_t> m_;
                };

        }

        template<class Tag, class Byte, class Format, class Primitive>
        struct decl_{

                using tag = Tag;
                using byte = Byte;
                using format = Format;
                using primitive = Primitive;

                //template<class Value>
                //inline friend std::ostream& operator<<(std::ostream& ostr, Value&& value){
                        ////return ostr << boost::format("(%s,%s) dispatched to (%s,%s,%s,%s)\n")
                                ////% boost::typeindex::type_id<Value>().pretty_name()
                                ////% value 
                                ////% Tag()
                                ////% Byte::to_string()
                                ////% boost::typeindex::type_id<Format>().pretty_name()
                                ////% boost::typeindex::type_id<Primitive>().pretty_name()
                        ////;
                        //return ostr;
                //}
                template<class Stream, class Value>
                void serialize(Stream&& stream, Value&& value){
                        //GPACK_DEBUG_PRINT( debugger_t, tag() );
                        detail::dsl_dirty_profiler::get().hit<decl_>();
                        Format().template serialize<Primitive>(stream,Byte(),value);
                }
                template<class Context, class Iter>
                Iter deserialize(Context& ctx, Iter first, Iter last){
                        assert( first != last && "precondition failed");
                        return Format().template deserialize<Tag,Primitive>(ctx,first,last,Byte());
                }
        };
        
        template<class Exception, class Msg>
        struct throw_msg{
                template<class Stream, class Value>
                void serialize(Stream&& , Value&& ){
                        BOOST_THROW_EXCEPTION(Exception(mpl::c_str<Msg>::value));
                }
        };

        // any tag
        struct any_{};


        template<class... Cases>
        struct switch_;
        template<class Tag, class... Subs>
        struct case_;
        template<class Tag, class Head, class... Tail>
        struct case_<Tag,Head,Tail...>{
                using tag = Tag;

                using impl_type = case_<Tag,Tail...>;

                using parser_vec = typename mpl::push_back< 
                        typename impl_type::parser_vec
                        , typename Head::parser_type
                >::type;
                

                template<class Stream, class Value>
                void make(Stream&& stream, Value&& value){
                        if( Head().test( value )){
                                Head().dispatch(stream,value);
                        } else {
                                case_<Tag,Tail...>().make(stream, value);
                        }
                }
        };
        template<class Tag>
        struct case_<Tag>{
                using tag = Tag;

                using parser_vec = mpl::vector<>;
                
                template<class Stream, class Value>
                void make(Stream&& , Value&& ){
                        #ifndef NDEBUG
                        std::cout << "unable to dispatch for case_\n"; 
                        #endif
                        BOOST_THROW_EXCEPTION(std::domain_error("unable to dispatch for case"));
                }
        };

        template<class Head, class... Tail>
        struct switch_<Head,Tail...>{
                using impl_type = switch_<Tail...>;
                
                using parser_vec = mpl::joint_view< 
                        typename Head::parser_vec
                        , typename impl_type::parser_vec
                >;

                static void debug(){
                        //std::cout << boost::format("switch_(%s)\n") % boost::typeindex::type_id<Head>().pretty_name();
                        switch_<Tail...>::debug();
                }
                template<class Tag, class Stream, class Value>
                typename std::enable_if<
                        mpl::or_<
                                std::is_same<Tag,typename Head::tag>,
                                std::is_same<any_,typename Head::tag>
                        >::value
                >::type make( Tag , Stream&& stream, Value&& value){
                        Head().make( stream, value );
                }
                template<class Tag, class Stream, class Value>
                typename std::enable_if<
                        ! mpl::or_<
                                std::is_same<Tag,typename Head::tag>,
                                std::is_same<any_,typename Head::tag>
                        >::value
                >::type make( Tag tag, Stream&& stream, Value&& value){
                        switch_<Tail...>().make( tag, stream, value );
                }

                // to get a vector to decls

        };
        template<>
        struct switch_<>{
                using parser_vec = mpl::vector<>;

                static void debug(){}
                template<class Tag, class Stream, class Value>
                void make(Tag&& tag,Stream&&,Value&&){
                        #ifndef NDEBUG
                        std::cerr << "unable to find case for tag " << tag << "\n";
                        #endif
                        BOOST_THROW_EXCEPTION(std::domain_error("unable to dispatch for tag"));
                }
        };
        template<class Pred, class Type>
        struct when_{
                template<class Value>
                bool test(Value&& value){
                        return Pred()(value);
                }
                template<class Stream, class Value>
                void dispatch(Stream&& stream, Value&& value){
                        Type().serialize(stream,value);
                }


                // there Parser Type
                using parser_type = Type;
        };
        template<std::int64_t Left, std::int64_t Right>
        struct range{
                template<class Value>
                typename std::enable_if<
                        std::is_integral<
                                typename std::decay<Value>::type
                        >::value
                        ,bool>::type
                operator()(Value&& value){
                        return Left <= value && value < Right;
                }
                template<class Value>
                typename std::enable_if<
                        ! std::is_integral<
                                typename std::decay<Value>::type
                        >::value
                        ,bool>::type
                operator()(Value&& value){
                        // TODO not sure where im using this open and where im using it closed
                        return Left <= value.size() && value.size() < Right;
                }
        };

        template<class T>
        struct equal_to{
                template<class U>
                bool operator()(U&& value){
                        return T() == value;
                }
        };

        struct always{
                template<class Value>
                auto operator()(Value&&){
                        return true;
                }
        };

}
