#pragma once

#include "tags.h"
#include "formats.h"
#include "dsl.h"
#include "decoder.h"

namespace gpack{
namespace schema{
namespace simple_detail{

        struct nil      : decl_< tags::nil_     , byte_<_1,_1,_0,_0,_0,_0,_0,_0> , formats::t               , std::nullptr_t>{};
        struct false_   : decl_< tags::bool_    , uchar_<0xC2>                   , formats::t               , mpl::false_>   {};
        struct true_    : decl_< tags::bool_    , uchar_<0xC3>                   , formats::t               , mpl::true_>    {};
        struct insert32_: decl_< tags::insert_  , uchar_<0xC6>                   , formats::t_v             , std::uint32_t> {};
        struct ref32_   : decl_< tags::ref_     , uchar_<0xC9>                   , formats::t_v             , std::uint32_t> {};
        struct float32_ : decl_< tags::float32_ , uchar_<0xCA>                   , formats::t_v             , float32_t>     {};
        struct float64_ : decl_< tags::float64_ , uchar_<0xCB>                   , formats::t_v             , float64_t>     {};
        struct int64_   : decl_< tags::int_     , uchar_<0XD3>                   , formats::t_v             , std::int64_t>  {};
        struct str16_   : decl_< tags::string_  , uchar_<0xDA>                   , formats::t_s_vargs       , std::uint16_t >{};
        struct array32  : decl_< tags::array_   , uchar_<0xDD>                   , formats::t_s             , std::uint32_t >{};
        struct map32    : decl_< tags::map_     , uchar_<0xDF>                   , formats::t_s             , std::uint32_t >{};

        // declaritve static construction policy
        struct policy :
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
                                        range<0,(1<<16)>, 
                                        str16_
                                >
                        >,
                        case_<tags::insert_,
                                when_< 
                                        always,
                                        insert32_
                                >
                        >,
                        case_<tags::ref_,
                                when_< 
                                        always,
                                        ref32_
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
                                        range<0,(1ull<<32)>,
                                        array32
                                >
                        >,
                        case_<tags::map_,
                                when_<
                                        range<0,(1ull<<32)>,
                                        map32
                                >
                        >,
                        case_<tags::float32_, when_<always, float32_> >,
                        case_<tags::float64_, when_<always, float64_> >,
                        case_<tags::nil_, when_< always, nil> >
                >
        {};
        
} // simple_detail
} // schema
} // gpack

namespace gpack{
namespace schema{
        using simple = simple_detail::policy;
} // schema
} // gpack
