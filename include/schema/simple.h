#pragma once

#include "tags.h"
#include "formats.h"
#include "dsl.h"
#include "decoder.h"

namespace gpack{
namespace schema{
namespace simple_types{

        struct nil      : decl_< tags::nil_     , byte_<_1,_1,_0,_0,_0,_0,_0,_0> , formats::t               , std::nullptr_t>{};
        struct false_   : decl_< tags::bool_    , uchar_<0xC2>                   , formats::t               , mpl::false_>   {};
        struct true_    : decl_< tags::bool_    , uchar_<0xC3>                   , formats::t               , mpl::true_>    {};
        struct float32_ : decl_< tags::float32_ , uchar_<0xCA>                   , formats::t_v             , float32_t>     {};
        struct float64_ : decl_< tags::float64_ , uchar_<0xCB>                   , formats::t_v             , float64_t>     {};
        struct int64_   : decl_< tags::int_     , uchar_<0XD3>                   , formats::t_v             , std::int64_t>  {};
        struct str16_   : decl_< tags::string_  , uchar_<0xDA>                   , formats::t_s_vargs       , std::uint16_t >{};
        struct array32  : decl_< tags::array_   , uchar_<0xDD>                   , formats::t_s             , std::uint32_t >{};
        struct map32    : decl_< tags::map_     , uchar_<0xDF>                   , formats::t_s             , std::uint32_t >{};

        // declaritve static construction policy
        struct simple :
                // type switch
                switch_<
                        // first this type
                        case_<tags::int_,
                                when_<
                                        always,
                                        int64_
                                >
                        >,
                        case_<tags::string_,
                                when_< 
                                        always,
                                        str16_
                                >
                        >,
                        case_<tags::bool_,
                                when_<
                                        equal_to<mpl::true_>,
                                        true_
                                >, when_<
                                        equal_to<mpl::false_>,
                                        false_
                                >
                        >,
                        case_<tags::array_,
                                when_<
                                        always,
                                        array32
                                >
                        >,
                        case_<tags::map_,
                                when_<
                                        always,
                                        map32
                                >
                        >,
                        case_<tags::float32_, when_<always, float32_> >,
                        case_<tags::float64_, when_<always, float64_> >,
                        case_<tags::nil_, when_< always, nil> >
                >
        {};
        
} // simple_types
} // schema
} // gpack

namespace gpack{
namespace schema{
        using simple = simple_types::simple;
} // schema
} // gpack
