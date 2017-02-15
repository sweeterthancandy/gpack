#include "formats.h"
#include "byte.h"
#include "backend.h"

#include <gtest/gtest.h>

using namespace gpack;


TEST( formats, t){
	vector_backend backend;

	formats::t fmt;
	fmt.serialize<char>( backend, uchar_<0xff>(), 0 /*dummy*/ );
	fmt.serialize<char>( backend, uchar_<0xab>(), 0 /*dummy*/ );
	fmt.serialize<char>( backend, uchar_<0x11>(), 0 /*dummy*/ );

	EXPECT_EQ( 3, backend.get().size() );
	EXPECT_EQ( '\xff', backend.get()[0] );
	EXPECT_EQ( '\xab', backend.get()[1] );
	EXPECT_EQ( '\x11', backend.get()[2] );
	EXPECT_TRUE( boost::equal( backend.get(), std::vector<char>{'\xff', '\xab', '\x11'} ) );

	auto iter{ backend.get().begin()}, end{ backend.get().end() };

	for(; iter != end; ++iter){
	}
} 
