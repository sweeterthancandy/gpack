#pragma once

#include <type_traits>
#include <memory>

#include <boost/signals2/signal.hpp>

#include <boost/mpl/for_each.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/push_back.hpp>

#include "print.h"
#include "backend.h"
#include "byte.h"

namespace gpack{
	
	// For deserialzing, it's dependant on the scheme to be surjective, that is
	// that for every json object J, the mapping M : J -> J', has a corresponding
	// mapping M' : J' -> J, such that M(M'(J)) = J, that is M, is invertiable.

        namespace mpl = boost::mpl;

        namespace detail{
                template<class First, class Second>
                struct match
                        : mpl::integral_c< bool,
                                ( Second::value & First::mask ) == ( First::value )
                        >
                {
                        static_assert( Second::constant_pattern ,"");
                };
                template<class T>
                struct type_wrap{
                        using wrapped_type = T;
                };
        }

        template<class Sub_Parsers>
        struct decoder_kernel{
                using iter_type = char const*;


                template<class First_Byte, class Context, class Iter>
                auto do_byte_(Context& ctx, Iter first, Iter last)->Iter{
                        #if 0
                        ////////////////////////////////////////////////////////
                        // This code is for checking injectivness
                        ////////////////////////////////////////////////////////
                        size_t count{0};
                        mpl::for_each<
                                typename mpl::fold<
                                        Sub_Parsers,
                                        mpl::vector<>,
                                        mpl::push_back<mpl::_1, detail::type_wrap<mpl::_2> >
                                >::type
                        >([&](auto&& wrap){
                                using T = typename std::decay<decltype(wrap)>::type::wrapped_type;

                                if( detail::match<typename T::byte, First_Byte>() ){
                                        ++count;
                                } 
                        });
                        if( count != 1 ){
                                std::cout << "------ multiple matches !!!! ----------\n";
                                PRINT(count);
                                PRINT(First_Byte::to_string());
                                using sz = typename mpl::size<Sub_Parsers>::type;
                                PRINT( sz::value );
                                mpl::for_each<
                                        typename mpl::fold<
                                                Sub_Parsers,
                                                mpl::vector<>,
                                                mpl::push_back<mpl::_1, detail::type_wrap<mpl::_2> >
                                        >::type
                                >([&](auto&& wrap){
                                        using T = typename std::decay<decltype(wrap)>::type::wrapped_type;

                                        PRINT( T::byte::to_string() );
                                        PRINT(( (bool)detail::match<typename T::byte, First_Byte>() ));
                                });

                                assert( 0 && "not surjection");
                        }
                        #endif

                        // this is actually a workaroud for a bug, I can't
                        // get mpl::find_if to work
                        // 
                        // NOTE these are local to the function contexts, not related to the paramete
                        bool ctx_running{true};
                        Iter ctx_result{first};

                        mpl::for_each<
                                typename mpl::fold<
                                        Sub_Parsers,
                                        mpl::vector<>,
                                        mpl::push_back<mpl::_1, detail::type_wrap<mpl::_2> >
                                >::type
                        >([&](auto&& wrap){
                                using T = typename std::decay<decltype(wrap)>::type::wrapped_type;

                                if( ! ctx_running )
                                        return;
                                if( detail::match<typename T::byte, First_Byte>() ){
                                        ctx_result = T().deserialize(ctx,first,last);
                                        ctx_running = false;
                                } 
                        });
                        return ctx_result;
                }
                template <class Context>
                iter_type parse( Context& ctx, iter_type first, iter_type last ) {
                        assert( first != last && "precondition failed" );
                        do {
                                iter_type next_first = first;
                                switch ( static_cast<unsigned char>( *first ) ) {
                                #define AUX( z, n, d )                                     \
                                        case n:                                            \
                                                next_first = this->do_byte_<               \
                                                        uchar_<n>>( ctx, first, last );    \
                                                break;
                                        BOOST_PP_REPEAT( 256, AUX, ~)
                                #undef AUX
                                default:
                                        __builtin_unreachable();
                                }

                                if ( next_first == first )
                                        break;
                                first = next_first;
                        } while ( first != last );
                        return first;
                }
        };
        
        template <class Decoder, class Context>
        struct decoder_subscriber
            : std::enable_shared_from_this<decoder_subscriber<Decoder, Context>> {
                explicit decoder_subscriber( Decoder& decoder, Context& ctx )
                    : decoder_( &decoder ), ctx_( &ctx ) {}

                void connect( dispatch_backend& that ) {
                        auto self = std::enable_shared_from_this<
                            decoder_subscriber<Decoder, Context>>::shared_from_this();
                        con_ = that.connect( [this, self]( auto&& data ) {
                                boost::copy( data, std::back_inserter( buffer_ ) );

                                using iter_type = char const*;

                                iter_type first = &buffer_.front();
                                iter_type last = first + buffer_.size();
                                auto result = decoder_->parse( *ctx_, first, last );

                                if ( result != first ) {
                                        std::vector<char> tmp( result, last );
                                        buffer_ = std::move( tmp );
                                }
                        } );
                }

              private:
                Decoder* decoder_;
                Context* ctx_;
                boost::signals2::connection con_;
                std::vector<char> buffer_;
        };
}
