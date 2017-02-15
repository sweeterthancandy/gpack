#pragma once

#include <cassert>
#include <cstdint>
#include <algorithm>

#include "base.h"
#include "byte.h"

namespace gpack{ namespace formats{

        // This represents a solid byte
        //
        // The primitive should only really be a byte
        struct t{
                template<class Primitive, class Stream, class Byte, class Value>
                void serialize(Stream&& stream, Byte&&, Value&&){
                        //static_assert( sizeof(Primitive) == 1 , "bad serialization");
                        stream.put( Byte::value );
                }
                // NOTE That primitive has special semantics here
                template<class Tag, class Primitive, class Context, class Iter, class Byte>
                Iter deserialize(Context& ctx, Iter first, Iter, Byte&& ){
                        assert( static_cast<uchar>(*first) == Byte::value );
                        ctx( Tag(), Primitive() );
                        return first+1;
                }
        };

        namespace detail{
                template<class Fill>
                struct tv_composer{
                        template<class Byte>
                        static char compose(char value){
                                assert( ( value & Byte::mask ) == ( Fill::value & Byte::mask ) );
                                auto tmp = static_cast<char>(
                                        static_cast<unsigned char>( (  Byte::value & Byte::mask ) |
                                        ( static_cast<unsigned char>(value) & ~Byte::mask )
                                ));
                                return tmp;
                        }
                        template<class Byte, class Primitive>
                        static Primitive decompose(char value){
                                static_assert( sizeof(Primitive) == 1, "expected byte sized primitive");
                                auto tmp = static_cast<Primitive>(( static_cast<unsigned char>(value) & ~Byte::mask )
                                        | ( Fill::value & Byte::mask ) );
                                assert( compose<Byte>( tmp ) == value && "not surjection");
                                return tmp;
                        }
                };
        }

        // bp
        template<class Fill>
        struct basic_tv{
                using composer = detail::tv_composer<Fill>;
                template<class Primitive, class Stream, class Byte, class Value>
                void serialize(Stream&& stream, Byte&& , Value&& value){
                        stream.put( composer::template compose<Byte>(value) );
                }
                template<class Tag, class Primitive, class Context, class Iter, class Byte>
                Iter deserialize(Context& ctx, Iter first, Iter , Byte&& ){
                        ctx( Tag(), composer::template decompose<Byte,Primitive>( *first ) );
                        return first + 1;
                }
        };

        using tv  = basic_tv<uchar_<0x0> >;
        using ts = tv;
        using tv1 = basic_tv<uchar_<0xFF> >;

        // b,p
        struct t_v{
                template<class Primitive, class Stream, class Byte, class Value>
                void serialize(Stream&& stream, Byte&& , Value&& value){
                        Primitive const tmp = static_cast<Primitive>(value);
                        stream.up();
                        stream.put( Byte::value );
                        stream.write( reinterpret_cast<const char*>(&tmp), sizeof(tmp) );
                        stream.down();
                }
                template<class Tag, class Primitive, class Context, class Iter, class Byte>
                Iter deserialize(Context& ctx, Iter first, Iter last, Byte&& ){
                        auto iter = first;
                        ++iter;
                        auto dist = std::distance(iter,last);
                        assert( 0 <= dist );
                        if( static_cast<size_t>(dist) < sizeof(Primitive) ){
                                return first;
                        }
                        Primitive const value = *reinterpret_cast<Primitive const*>(iter);
                        iter += sizeof(Primitive);
                        ctx(Tag(), value );
                        return iter;
                }
        };
        using t_s = t_v;
        // b,p,s
        struct t_s_vargs{
                template<class Primitive, class Stream, class Byte, class Value>
                void serialize(Stream&& stream, Byte&& , Value&& value){
                        Primitive const tmp = static_cast<Primitive>(value.size());
                        stream.up();
                        stream.put( Byte::value );
                        stream.write( reinterpret_cast<const char*>(&tmp), sizeof(tmp) );
                        stream.write( value.data(), value.size() );
                        stream.down();
                }
                template<class Tag, class Primitive, class Context, class Iter, class Byte>
                Iter deserialize(Context& ctx, Iter first, Iter last, Byte&& ){
                        auto iter = first;
                        ++iter;
                        auto dist = std::distance(iter,last);
                        assert( 0 < dist );
                        if( static_cast<size_t>(dist) < sizeof(Primitive) ){
                                // want to return why maybe?
                                return first;
                        }
                        Primitive const size = *reinterpret_cast<Primitive const*>(iter);
                        iter += sizeof(Primitive);
                        auto data_dist = std::distance(iter,last);
                        if( data_dist < size )
                                return first; // TODO
                        ctx(Tag(),size,iter);
                        iter += size;
                        return iter;
                }
        };
        // bp,s
        struct ts_vargs{
                template<class Primitive, class Stream, class Byte, class Value>
                void serialize(Stream&& stream, Byte&& , Value&& value){
                        auto tmp = static_cast<unsigned char>(
                                (  Byte::value & Byte::mask ) |
                                ( static_cast<unsigned char>(value.size()) & ~Byte::mask )
                        );
                        stream.up();
                        stream.put( tmp );
                        stream.write( value.data(), value.size() );
                        stream.down();
                }
                template<class Tag, class Primitive, class Context, class Iter, class Byte>
                Iter deserialize(Context& ctx, Iter first, Iter last, Byte&& ){
                        assert( ( static_cast<unsigned char>(*first) & Byte::mask ) == ( Byte::value & Byte::mask) && "precondition failed");
                        // I know these are the same
                        auto size = static_cast<std::uint8_t>(static_cast<unsigned char>(*first) & ~Byte::mask );
                        auto iter = first;
                        ++iter;
                        auto dist = std::distance(iter,last);
                        assert( 0 < dist );
                        if( static_cast<size_t>(dist) < size){
                                return first;
                        }
                        ctx( Tag(), size, iter );
                        iter += size;
                        return iter;
                }
        };
} }
