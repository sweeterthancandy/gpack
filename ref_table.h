#pragma once

#include "vt.h"
#include "vt_to_string.h"

namespace gpack{
        namespace detail{
                struct ref_table : boost::noncopyable{
                        void insert( variant_t const& var, size_t count){
                                //std::cout << boost::format("ref_table::%s(%s,%s)\n") 
                                        //% __FUNCTION__ 
                                        //% boost::apply_visitor(
                                                //variant_t_to_string(),var)
                                        //% count;
                                // this means that there is at least 2 reference to the string
                                // a value of 1 would mean special semantics for one usgae 
                                assert( count >= 2 && "precondition failed");

                                impl_.emplace_back(var,count-1);
                                m_.insert(std::make_pair( impl_.size() -1, std::prev(impl_.end())));
                        }
                        // called on each ref usage
                        void down(size_t index){
                                //std::cout << boost::format("ref_table::%s(%s)\n") % __FUNCTION__ % index;
                                auto iter = m_.find(index);
                                assert( iter != m_.end() && "precondition failed");
                                if( --iter->second->second == 0 ){
                                        decltype(m_) next_m;
                                        std::for_each( m_.begin(), iter, [&](auto&& p){ next_m.insert(p); } );
                                        for(auto i(std::next(iter)),end(m_.end());i!=end;++i){
                                                next_m.insert(std::make_pair(
                                                        i->first - 1,
                                                        i->second ));
                                        }
                                        impl_.erase(iter->second);
                                        m_ = std::move(next_m);
                                }

                        }
                        variant_t lookup(size_t index)const{
                                //std::cout << boost::format("ref_table::%s(%s)\n") % __FUNCTION__ % index;
                                auto iter = m_.find(index);
                                assert( iter != m_.end() && "precondition failed");
                                return iter->second->first;
                        }
                        void debug_()const{
                                std::cout << "--- ref_table debug ----\n";
                                for( auto const& p : m_ ){
                                        std::cout << boost::format("%-2i -> (%s,%s)\n")
                                                % p.first
                                                % to_string(p.second->first)
                                                % p.second->second
                                        ;
                                }
                                std::cout << "------------------------\n";
                        }
                private:
                        using impl_t = std::list<std::pair<variant_t,size_t> >; // (value,ref_count)
                        impl_t impl_;
                        std::map<size_t,impl_t::iterator> m_; // M : index -> (value,ref_count)
                };
        }
}
