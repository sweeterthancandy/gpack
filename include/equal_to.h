#pragma once

#include <boost/type_index.hpp>

#include "vt.h"

namespace gpack{


        struct equal_to_ : boost::static_visitor<bool> {

                using trivially_comparable =
                    mpl::push_back<primitive_vec, binary_>::type;

                template <class T, class U>
                bool operator()( T&& , U&&  ) const {
                        return false;
                }
                template <class T>
                std::enable_if_t<
                    mpl::contains<trivially_comparable, std::decay_t<T>>::value, bool>
                operator()( T&& left, T&& right ) const {
                        return left == right;
                }
                template <class T>
                std::enable_if_t<std::is_same<map_, std::decay_t<T>>::value, bool>
                operator()( T&& left, T&& right ) const {
                        return boost::equal(
                            left, right, [&]( auto&& lp, auto&& rp ) -> bool {
                                    return boost::apply_visitor( *this, lp.first,
                                                                 rp.first ) &&
                                           boost::apply_visitor( *this, lp.second,
                                                                 rp.second );
                            } );
                }
                template <class T>
                std::enable_if_t<std::is_same<array_, std::decay_t<T>>::value, bool>
                operator()( T&& left, T&& right ) const {
                        return boost::equal(
                            left, right, [&]( auto&& lp, auto&& rp ) -> bool {
                                    return boost::apply_visitor( *this, lp, rp );
                            } );
                }
                template <class T>
                std::enable_if_t<std::is_same<insert_, std::decay_t<T>>::value, bool>
                operator()( T&& , T&&  ) const {
                        BOOST_THROW_EXCEPTION( std::domain_error( "unexpected insert" ) );
                }
                template <class T>
                std::enable_if_t<std::is_same<ref_, std::decay_t<T>>::value, bool>
                operator()( T&& , T&&  ) const {
                        BOOST_THROW_EXCEPTION( std::domain_error( "unexpected ref" ) );
                }
        };
}

