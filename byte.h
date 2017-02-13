#pragma once

#include <boost/preprocessor.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/arithmetic.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/vector.hpp>

// point of this is to represent byte patterns

namespace gpack{

        namespace mpl = boost::mpl;

        namespace detail{
                template<class T, T Base, T Exp>
                struct pow_c :
                        mpl::fold<
                                mpl::range_c<T,0,Exp>,
                                mpl::integral_c<T,1>,
                                mpl::multiplies<mpl::_1,mpl::integral_c<T,Base>>
                        >::type
                {
                        static_assert( Exp >= 0,"" );
                };
        }


        struct _0{
                static const char glyph = '0';
                static const char mask = 1;
                static const char value = 0;
        };
        struct _1{
                static const char glyph = '1';
                static const char mask = 1;
                static const char value = 1;
        };
        struct X{
                static const char glyph = 'X';
                static const char mask = 0;
                static const char value = 0; // not valid
        };




        template<BOOST_PP_ENUM_PARAMS(8,class b)>
        struct byte_{
                #define BYTE_AUX_ASSERT(z,n,d) static_assert( mpl::contains<mpl::vector<_0,_1,X>,BOOST_PP_CAT(b,n)>::value,"" );
                BOOST_PP_REPEAT(8,BYTE_AUX_ASSERT,~)
                #undef BYTE_AUX_ASSERT

                static std::string to_string(){
                        std::string buf;
                        #define BYTE_AUX_TO_STRING(z,n,d) \
                                buf += BOOST_PP_CAT(b,n)::glyph;
                        BOOST_PP_REPEAT(8,BYTE_AUX_TO_STRING,~)
                        #undef BYTE_AUX_TO_STRING
                        return buf;
                }

                #define BYTE_AUX_MASK(z,n,data) \
                        + BOOST_PP_CAT(b,n)::mask * detail::pow_c<int,2,BOOST_PP_SUB(7,n)>()
                #define BYTE_AUX_VALUE(z,n,data) \
                        + BOOST_PP_CAT(b,n)::value * BOOST_PP_CAT(b,n)::mask * detail::pow_c<int,2,BOOST_PP_SUB(7,n)>()

                using uchar = unsigned char;
                static const uchar  mask = 0 BOOST_PP_REPEAT(8,BYTE_AUX_MASK,~) ;
                static const uchar  value = 0 BOOST_PP_REPEAT(8,BYTE_AUX_VALUE,~) ;

                #undef BYTE_AUX_MASK
                #undef BYTE_AUX_VALUE

                static const bool constant_pattern = ( mask == 255 );

        };

        namespace detail{
                
                template<unsigned char C>
                struct placeholder_cast;
                template<>
                struct placeholder_cast<0> 
                        : mpl::identity<_0>
                {};
                template<>
                struct placeholder_cast<1> 
                        : mpl::identity<_1>
                {};
        }

        
        // easier to reprefent constant patterns with a 0xXY
        template<unsigned char Value>
        struct uchar_ : 
                byte_<
                        #define UCHAR_AUX(z,n,data) \
                                BOOST_PP_COMMA_IF(n) typename detail::placeholder_cast<( Value & ( 1 << BOOST_PP_SUB(7,n) ) ) != 0>::type
                        BOOST_PP_REPEAT(8,UCHAR_AUX,~)
                        #undef UCHAR_AUX
                >
        {};


}
