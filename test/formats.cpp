#include "formats.h"
#include "byte.h"
#include "backend.h"

#include <gtest/gtest.h>

#include <boost/any.hpp>

#include "tags.h"

using namespace gpack;

enum Type{
        Type_String,
        Type_Binary,
        Type_Int,
        Type_Bool,
        Type_Float32,
        Type_Float64,
        Type_Nil,
        Type_Map,
        Type_Array,
        Type_Insert,
        Type_Ref,
};

struct deserialize_item{
        template<class... Args>
        explicit deserialize_item(Type type, Args&&... args)
                :type_{type}
        {
                int aux[] = {0,(args_.emplace_back(args), 0)...};
        }
        virtual ~deserialize_item()=default;

        auto GetType()const{ return type_; }
private:
        Type type_;
        std::vector<boost::any> args_;
};
struct deserialize_context{
        template<class Size_Type>
        void operator()(tags::string_, Size_Type n, const char* data){
                items_.emplace_back( Type_String, n, data);
        }
        template<class Size_Type>
        void operator()(tags::binary_, Size_Type , const char* ){
        }
        template<class Int_Type>
        void operator()(tags::int_, Int_Type value){
                items_.emplace_back( Type_Int, value);
        }
        void operator()(tags::bool_, bool value){
                items_.emplace_back( Type_Bool, value);
        }
        template<class Float_Type>
        void operator()(tags::float32_, Float_Type value){
        }
        template<class Float_Type>
        void operator()(tags::float64_, Float_Type value){
        }
        template<class Dummy>
        void operator()(tags::nil_,Dummy&&){
        }
        template<class Size_Type>
        void operator()(tags::map_, Size_Type n){
        }
        template<class Size_Type>
        void operator()(tags::array_, Size_Type n){
        }
        template<class Size_Type>
        void operator()(tags::insert_, Size_Type count){
        }
        template<class Size_Type>
        void operator()(tags::ref_, Size_Type idx){
        }
private:
        std::vector<deserialize_item> items_;
};

TEST( formats, t){
	vector_backend backend;
        deserialize_context dsctx;

        char dummy;

	formats::t fmt;
	fmt.serialize<char>( backend, uchar_<0xff>(), 0 /*dummy*/ );
	fmt.serialize<char>( backend, uchar_<0xab>(), 0 /*dummy*/ );
	fmt.serialize<char>( backend, uchar_<0x11>(), 0 /*dummy*/ );

        std::vector<char> serial{ backend.get() };


	EXPECT_EQ( 3, backend.get().size() );
	EXPECT_EQ( '\xff', backend.get()[0] );
	EXPECT_EQ( '\xab', backend.get()[1] );
	EXPECT_EQ( '\x11', backend.get()[2] );
	EXPECT_TRUE( boost::equal( serial, std::vector<char>{'\xff', '\xab', '\x11'} ) );
        
        #if 0
        auto iter = serial.cbegin();
        auto end = serial.cend();

        iter = fmt.deserialize<tags::int_, char>( dsctx, iter, end, dummy);
        iter = fmt.deserialize<tags::int_, char>( dsctx, iter, end, dummy);
        iter = fmt.deserialize<tags::int_, char>( dsctx, iter, end, dummy);

        EXPECT_EQ( end, iter);
        #endif

} 
