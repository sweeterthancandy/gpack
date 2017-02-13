#pragma once

#include "tags.h"
#include "formats.h"
#include "dsl.h"
#include "decoder.h"

namespace gpack{ namespace schema{



        struct pfixint  : decl_< tags::int_     , byte_<_0,X,X,X,X,X,X,X>        , formats::tv              , std::uint8_t>  {}; 
        struct fixmap   : decl_< tags::map_     , byte_<_1,_0,_0,_0,X,X,X,X>     , formats::ts              , std::uint8_t>  {};
        //struct fixarray : decl_< tags::array_   , byte_<_1,_0,_0,_1,X,X,X,X>     , formats::ts              , std::uint8_t>  {};
        struct fixref   : decl_< tags::ref_     , byte_<_1,_0,_0,_1,X,X,X,X>     , formats::ts              , std::uint8_t>  {};
        struct fixstr   : decl_< tags::string_  , byte_<_1,_0,_1, X,X,X,X,X>     , formats::ts_vargs        , std::uint8_t>  {};

        struct nil      : decl_< tags::nil_     , byte_<_1,_1,_0,_0,_0,_0,_0,_0> , formats::t               , std::nullptr_t>{};
        
        struct false_   : decl_< tags::bool_    , uchar_<0xC2>                   , formats::t               , mpl::false_>   {};
        struct true_    : decl_< tags::bool_    , uchar_<0xC3>                   , formats::t               , mpl::true_>    {};

        //struct bin8_    : decl_< tags::binary_  , uchar_<0xC4>                   , formats::t_s_vargs       , std::uint8_t>  {};
        //struct bin16_   : decl_< tags::binary_  , uchar_<0xC5>                   , formats::t_s_vargs       , std::uint16_t> {};
        //struct bin32_   : decl_< tags::binary_  , uchar_<0xC6>                   , formats::t_s_vargs       , std::uint32_t> {};
        
        struct insert8_    : decl_< tags::insert_  , uchar_<0xC4>                   , formats::t_v             , std::uint8_t>  {};
        struct insert16_   : decl_< tags::insert_  , uchar_<0xC5>                   , formats::t_v             , std::uint16_t> {};
        struct insert32_   : decl_< tags::insert_  , uchar_<0xC6>                   , formats::t_v             , std::uint32_t> {};
        
        struct ref8_    : decl_< tags::ref_  , uchar_<0xC7>                   , formats::t_v             , std::uint8_t>  {};
        struct ref16_   : decl_< tags::ref_  , uchar_<0xC8>                   , formats::t_v             , std::uint16_t> {};
        struct ref32_   : decl_< tags::ref_  , uchar_<0xC9>                   , formats::t_v             , std::uint32_t> {};

        //struct ext8{};
        //struct ext16{};
        //struct ext32{};

        struct float32_ : decl_< tags::float32_ , uchar_<0xCA>                   , formats::t_v             , float32_t>     {};
        struct float64_ : decl_< tags::float64_ , uchar_<0xCB>                   , formats::t_v             , float64_t>     {};

        struct uint8_   : decl_< tags::int_     , uchar_<0xCC>                   , formats::t_v             , std::uint8_t>  {};
        struct uint16_  : decl_< tags::int_     , uchar_<0xCD>                   , formats::t_v             , std::uint16_t> {};
        struct uint32_  : decl_< tags::int_     , uchar_<0xCE>                   , formats::t_v             , std::uint32_t> {};
        struct uint64_  : decl_< tags::int_     , uchar_<0xCF>                   , formats::t_v             , std::uint64_t> {};
        
        struct int8_    : decl_< tags::int_     , uchar_<0XD0>                   , formats::t_v             , std::int8_t>   {};
        struct int16_   : decl_< tags::int_     , uchar_<0XD1>                   , formats::t_v             , std::int16_t>  {};
        struct int32_   : decl_< tags::int_     , uchar_<0XD2>                   , formats::t_v             , std::int32_t>  {};
        struct int64_   : decl_< tags::int_     , uchar_<0XD3>                   , formats::t_v             , std::int64_t>  {};

        struct fixext1{};
        struct fixext2{};
        struct fixext4{};
        struct fixext8{};
        struct fixext16{};

        struct str8_    : decl_< tags::string_  , uchar_<0xD9>                   , formats::t_s_vargs       , std::uint8_t > {};
        struct str16_   : decl_< tags::string_  , uchar_<0xDA>                   , formats::t_s_vargs       , std::uint16_t >{};
        struct str32_   : decl_< tags::string_  , uchar_<0xDB>                   , formats::t_s_vargs       , std::uint32_t >{};

        struct array16  : decl_< tags::array_   , uchar_<0xDC>                   , formats::t_s             , std::uint16_t >{};
        struct array32  : decl_< tags::array_   , uchar_<0xDD>                   , formats::t_s             , std::uint32_t >{};

        struct map16    : decl_< tags::map_     , uchar_<0xDE>                   , formats::t_s             , std::uint16_t >{};
        struct map32    : decl_< tags::map_     , uchar_<0xDF>                   , formats::t_s             , std::uint32_t >{};

        struct nfixint  : decl_< tags::int_     , byte_<_1,_1,_1,X,X,X,X,X>      , formats::tv1             , std::int8_t>  {}; 


        // declaritve static construction policy
        struct policy :
                // type switch
                switch_<
                        // first this type
                        case_<tags::int_,
                                when_< 
                                        range<0,(1ull<< 7)>, 
                                        pfixint
                                >,
                                when_< 
                                        range<0,(1ull<< 8)>, 
                                        uint8_
                                >,
                                when_< 
                                        range<0,(1ull<< 16)>, 
                                        uint16_
                                >,
                                when_< 
                                        range<0,(1ull<< 32)>, 
                                        uint32_
                                >,
                                when_< 
                                        range<-(1ll << 5),0>, 
                                        nfixint
                                >,
                                when_<
                                        range<-(1ll << 7),(1ll << 7)-1>,
                                        int8_
                                >,
                                when_<
                                        range<-(1ll << 15),(1ll << 15)-1>,
                                        int16_
                                >,
                                when_<
                                        range<-(1ll << 31),(1ll << 31)-1>,
                                        int32_
                                >,
                                when_<
                                        always,
                                        int64_
                                >
                        >,
                        case_<tags::string_,
                                when_< 
                                        range<0,(1<<5)>, 
                                        fixstr
                                >,
                                when_< 
                                        range<0,(1<<8)>, 
                                        str8_
                                >,
                                when_< 
                                        range<0,(1<<16)>, 
                                        str16_
                                >
                        >,
                        case_<tags::insert_,
                                when_< 
                                        range<0,(1<<8)>, 
                                        insert8_
                                >,
                                when_< 
                                        range<0,(1<<16)>, 
                                        insert16_
                                >,
                                when_< 
                                        always,
                                        insert32_
                                >
                        >,
                        case_<tags::ref_,
                                when_< 
                                        range<0,(1<<4)>, 
                                        fixref
                                >,
                                when_< 
                                        range<0,(1<<8)>, 
                                        ref8_
                                >,
                                when_< 
                                        range<0,(1<<16)>, 
                                        ref16_
                                >,
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
                                //when_<
                                        //range<0,(1<<4)>,
                                        //fixarray
                                //>,
                                when_<
                                        range<0,(1<<16)>,
                                        array16
                                >,
                                when_<
                                        range<0,(1ull<<32)>,
                                        array32
                                >
                        >,
                        case_<tags::map_,
                                when_<
                                        range<0,(1<<4)>,
                                        fixmap
                                >,
                                when_<
                                        range<0,(1<<16)>,
                                        map16
                                >,
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
        
} }
