#include "byte.h"

#include <gtest/gtest.h>

using namespace gpack;

using _00000000 = byte_<_0,_0,_0,_0,_0,_0,_0,_0>;
using _00000001 = byte_<_0,_0,_0,_0,_0,_0,_0,_1>;
using _00000010 = byte_<_0,_0,_0,_0,_0,_0,_1,_0>;
using _10000001 = byte_<_1,_0,_0,_0,_0,_0,_0,_1>;


static_assert( _00000000::mask  == 0xff , "" );
static_assert( _00000000::value == 0b00000000 , "" );
static_assert( _00000000::constant_pattern , "" );

static_assert( _00000001::mask  == 0xff , "" );
static_assert( _00000001::value == 0b00000001 , "" );
static_assert( _00000001::constant_pattern , "" );

static_assert( _00000010::mask  == 0xff , "" );
static_assert( _00000010::value == 0b00000010 , "" );
static_assert( _00000010::constant_pattern , "" );

static_assert( _10000001::mask  == 0xff , "" );
static_assert( _10000001::value == 0b10000001 , "" );
static_assert( _10000001::constant_pattern , "" );

using _11110XXX = byte_<_1,_1,_1,_1,_0,X,X,X>;
using _11100XXX = byte_<_1,_1,_1,_0,_0,X,X,X>;

static_assert( _11110XXX::mask  == 0b11111000 , "" );
static_assert( _11110XXX::value == 0b11110000 , "" );
static_assert( _11110XXX::constant_pattern == false , "" );

static_assert( _11100XXX::mask  == 0b11111000 , "" );
static_assert( _11100XXX::value == 0b11100000 , "" );
static_assert( _11100XXX::constant_pattern == false , "" );

using _0xff = uchar_<0xff>;
using _0xab = uchar_<0xab>;
using _0x3c = uchar_<0x3c>;

static_assert( _0xff::mask  == 0xff , "" );
static_assert( _0xff::value == 0xff , "" );
static_assert( _0xff::constant_pattern , "" );

static_assert( _0xab::mask  == 0xff , "" );
static_assert( _0xab::value == 0xab , "" );
static_assert( _0xab::constant_pattern , "" );

static_assert( _0x3c::mask  == 0xff , "" );
static_assert( _0x3c::value == 0x3c , "" );
static_assert( _0x3c::constant_pattern , "" );

TEST( byte, to_string ){

	EXPECT_EQ( "10000001", _10000001::to_string() );
	EXPECT_EQ( "00000001", _00000001::to_string() );
	EXPECT_EQ( "11110XXX", _11110XXX::to_string() );
}
