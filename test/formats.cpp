#include "formats.h"
#include "byte.h"
#include "backend.h"

#include <gtest/gtest.h>

using namespace gpack;

TEST( formats, t){
	vector_backend backend;

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
