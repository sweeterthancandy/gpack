#include "backend.h"
#include "byte.h"
#include "decoder.h"
#include "dsl.h"
#include "formats.h"
#include "tags.h"
#include "vt_context.h"
#include "vt_serializer.h"
#include "vt_to_string.h"
#include "json_parser/json_parser.h"

#include "schema/simple.h"

#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/any_cast.hpp>
#include <boost/type_erasure/builtin.hpp>
#include <boost/type_erasure/operators.hpp>
#include <boost/type_erasure/member.hpp>
#include <boost/type_erasure/free.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/core/demangle.hpp>
#include <boost/type_index.hpp>
#include <gtest/gtest.h>

#include <boost/any.hpp>

#include "tags.h"

namespace gpack{

struct nil      : decl_< tags::nil_     , byte_<_1,_1,_0,_0,_0,_0,_0,_0> , formats::t               , std::nullptr_t>{};
struct false_   : decl_< tags::bool_    , uchar_<0xC2>                   , formats::t               , mpl::false_>   {};
struct true_    : decl_< tags::bool_    , uchar_<0xC3>                   , formats::t               , mpl::true_>    {};
struct float32_ : decl_< tags::float32_ , uchar_<0xCA>                   , formats::t_v             , float32_t>     {};
struct float64_ : decl_< tags::float64_ , uchar_<0xCB>                   , formats::t_v             , float64_t>     {};
struct int64_   : decl_< tags::int_     , uchar_<0XD3>                   , formats::t_v             , std::int64_t>  {};
struct str16_   : decl_< tags::string_  , uchar_<0xDA>                   , formats::t_s_vargs       , std::uint16_t >{};

struct array32  : decl_< tags::array_   , uchar_<0xDD>                   , formats::t_s             , std::uint32_t >{};
struct map32    : decl_< tags::map_     , uchar_<0xDF>                   , formats::t_s             , std::uint32_t >{};

namespace bte = boost::type_erasure;

namespace detail{
        template<int i>
        struct precedence_device : precedence_device<i-1>{};
        template<>
        struct precedence_device<0>{};
        using precedence_last = precedence_device<0>;

        template<class T>
        auto maybe_promote_impl(precedence_last const& p, T&& t){
                return t;
        }
        template<class T,
                class _ = std::enable_if_t< std::is_same<std::remove_cv_t<std::remove_reference_t<T>>, char*>::value >
        >
        auto maybe_promote_impl(precedence_device<5> const& p, T&& t){
                return std::string{t};
        }
        template<class T,
                class _ = std::enable_if_t< std::is_integral<std::remove_cv_t<std::remove_reference_t<T>>>::value >
        >
        auto maybe_promote_impl(precedence_device<4> const& p, T&& t){
                return static_cast<int>(t);
        }
        template<class T>
        auto maybe_promote(T&& t){
                std::cout << boost::core::demangle( typeid(t).name() ) << "\n";
                return maybe_promote_impl( precedence_device<10>{}, t);
        }
} // detail

struct deserialize_item{
        using tag_type = 
                bte::any<
                        boost::mpl::vector<
                                bte::copy_constructible<>,
                                bte::typeid_<>,
                                bte::ostreamable<>
                        >
                >;
        using te_type = 
                bte::any<
                        boost::mpl::vector<
                                bte::copy_constructible<>,
                                bte::typeid_<>,
                                bte::ostreamable<>,
                                bte::equality_comparable<>, 
                                bte::relaxed
                        >
                >;
        template<class Tag, class... Args>
        explicit deserialize_item(Tag&& tag, Args&&... args)
                :tag_{tag}
        {
                int aux[] = {0,(args_.emplace_back(detail::maybe_promote(args)), 0)...};
        }
        virtual ~deserialize_item()=default;
        
        auto const& GetTag()const{ return tag_; }
        auto const& Arg0()const{ return args_.at(0); }
        auto const& Arg1()const{ return args_.at(1); }

        friend std::ostream& operator<<(std::ostream& ostr, deserialize_item const& self){
                ostr << "tag=" << self.GetTag() << ", args={";
                const char* comma = "";
                for(auto const& arg : self.args_){
                        ostr << comma
                                << 
                                "<" << boost::core::demangle( typeid_of( arg ).name() ) << ", "
                                << arg;
                        comma = ", ";
                }
                ostr << "}";
                return ostr;
        }
private:
        tags::tag_base tag_;
        std::vector<te_type> args_;
};

struct deserialize_context{
        template<class Tag, class... Args>
        std::enable_if_t< ! std::is_same<Tag, tags::nil_>::value >  operator()(Tag&& tag, Args&&... args){
                items_.emplace_back( tag, args...);
        }
        template<class Tag, class Dummy>
        std::enable_if_t<   std::is_same<Tag, tags::nil_>::value > operator()(Tag&& nil,Dummy&&){
                items_.emplace_back( nil );
        }
        void display(){
                for(auto const& _ : items_){
                        std::cout << _ << "\n";
                }
        }
        auto begin()const{ return items_.begin(); }
        auto end()const{ return items_.end(); }
private:
        std::vector<deserialize_item> items_;
};


TEST(serialization, primitive)
{	
        vector_backend backend;
        deserialize_context dsctx;
        char dummy;
        
        std::string hw =  "hello world";

        nil()     .serialize(backend, dummy);
        false_()  .serialize(backend, dummy);
        true_()   .serialize(backend, dummy);
        float32_().serialize(backend, 123.456f);
        float64_().serialize(backend, 123.456);
        int64_()  .serialize(backend, 654);
        str16_()  .serialize(backend, hw);

        std::vector<char> serial = backend.get();
        auto iter = &serial[0];
        auto end = &serial[0] + serial.size();

        iter = nil()     .deserialize(dsctx, iter, end);
        iter = false_()  .deserialize(dsctx, iter, end);
        iter = true_()   .deserialize(dsctx, iter, end);
        iter = float32_().deserialize(dsctx, iter, end);
        iter = float64_().deserialize(dsctx, iter, end);
        iter = int64_()  .deserialize(dsctx, iter, end);
        iter = str16_()  .deserialize(dsctx, iter, end);

        EXPECT_EQ(iter, end);

        dsctx.display();

        auto ri = dsctx.begin();
        auto re = dsctx.end();
        
        EXPECT_EQ( Id_Nil , ri->GetTag() );

        ++ri;
        EXPECT_EQ( Id_Bool, ri->GetTag() );
        EXPECT_EQ( mpl::false_()  , ri->Arg0() );

        ++ri;
        EXPECT_EQ( Id_Bool, ri->GetTag() );
        EXPECT_EQ( mpl::true_()     , ri->Arg0() );

        ++ri;
        EXPECT_EQ( Id_Float32, ri->GetTag() );
        EXPECT_EQ( 123.456f  , ri->Arg0() );
        
        ++ri;
        EXPECT_EQ( Id_Float64, ri->GetTag() );
        EXPECT_EQ( 123.456   , ri->Arg0() );
        
        ++ri;
        EXPECT_EQ( Id_Int, ri->GetTag() );
        EXPECT_EQ( 654   , ri->Arg0() );
        
        ++ri;
        EXPECT_EQ( Id_String, ri->GetTag() );
        EXPECT_EQ( static_cast<int>(hw.size()), ri->Arg0());
        //EXPECT_EQ( hw       , ri->Arg1());

        ++ri;
        EXPECT_EQ(re, ri);

}

static std::string json_sample_text = R"(
{
  "firstName": "John",
  "lastName": "Smith",
  "age": 25,
  "address": {
    "streetAddress": "21 2nd Street",
    "city": "New York",
    "state": "NY",
    "postalCode": "10021"
  },
  "phoneNumber": [
    {
      "type": "home",
      "number": "212 555-1234"
    },
    {
      "type": "fax",
      "number": "646 555-4567"
    }
  ],
  "gender": {
    "type": "male"
  }
}
)";


TEST(serialization, serialize_and_then_deserialize){
        using backend_t    = vector_backend;
        using policy_t     = schema::simple;
        using serializer_t = vt_serializer<policy_t,backend_t>;
        using parser_vec   = policy_t::parser_vec;
        using decoder_t    = decoder_kernel<parser_vec>;
        using context_t    = vt_context;
        vt_builder m;

        ASSERT_TRUE( json_parser::try_parse(m, json_sample_text.begin(), json_sample_text.end()) );
        variant_t root(m.make());


        backend_t   backend;
        policy_t p;
        serializer_t aux(p, backend);
        boost::apply_visitor( aux, root );

        std::vector<char> serial = backend.get();
        decoder_t dec;
        context_t ctx;
        dec.parse(ctx, &serial[0], &serial[0] + serial.size());
        auto result = ctx.make();
        display(result);
}

} // gpack
