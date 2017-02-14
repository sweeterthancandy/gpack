#pragma once

#include <list>

#include "vt.h"
#include <boost/range/algorithm.hpp>

namespace gpack{ namespace detail{
        
        /* example 
         *
         * S : a b b a c -> (a,2),(b,2),(c,1)
         *
         *      a -> ref(a)
         *      b -> ref(b)
         *      c -> c
         *
         * S : a b b a c -> ref(a) ref(b) ref(b) ref(a) c
         *               -> insert_(a) ...  [a]
         *               -> insert_(a) insert_(b) ...  [a,b]
         *               -> insert_(a) insert_(b) remove_(1) ...  [a]
         *               -> insert_(a) insert_(b) remove_(1) remove_(0) c  [a]
         *
         *
         * */

        /*
         * first go though to compile data structure
         * of duplicated strings, then choose strings
         * make indirect
         *
         */

        // note that you could make a reference to one string 
        struct optimizer_context : boost::noncopyable{
                void inc(std::string const& value){
                        ++str_count_[value];
                }
                variant_t map(std::string const& value){
                        auto s_count = str_count_.find(value);
                        assert( s_count != str_count_.end() && "precondition failed");
                        assert( s_count->second > 0 && "invariant failed");
                        
                        auto pred = [](std::string const& , size_t count)->bool{
                                return count > 1;
                        };

                        auto iter = mapping_.find(value);
                        if( iter == mapping_.end()){
                                if( pred( s_count->first, s_count->second ) ){
                                        assert( s_count->second >= 2 && "precondition failed (simplifies implenentation)");
                                        // CASE 1: need to insert a new reference (insert_)
                                        size_t idx = impl_.size();
                                        impl_.emplace_back(idx,s_count->second-1);
                                        mapping_.insert(std::make_pair(value,std::prev(impl_.end())));
                                        return insert_(value,s_count->second);
                                } else{
                                        // CASE 2: dont reference
                                        return value;
                                }
                        } else{
                                auto idx_rc_iter = iter->second; //  index ref-count iter
                                // CASE 3: return reference
                                ref_ ret(idx_rc_iter->first);
                                if( --idx_rc_iter->second == 0 ){
                                        std::for_each( 
                                                std::next(idx_rc_iter), 
                                                impl_.end(), 
                                                [last=idx_rc_iter->first](auto& p)mutable{
                                                        assert( last + 1 == p.first && "invariant failed");
                                                        --p.first;
                                                        ++last;
                                                }
                                        );
                                        impl_.erase(idx_rc_iter);
                                        mapping_.erase(iter);
                                }
                                return ret;
                        }
                }
                void debug()const{
                        for( auto const& p : str_count_ )
                                std::cout << boost::format("%-3i => %s\n") % p.second % p.first;
                        std::vector<std::tuple<std::string,size_t,size_t> > buf; //  (value,index,count)
                        for( auto const& p : mapping_ ){
                                auto idx = p.second->first;
                                auto rc = p.second->second;
                                buf.emplace_back( p.first, idx, rc );
                        }
                        std::sort( 
                                buf.begin(), buf.end(), 
                                [](auto&& l, auto&& r)->bool{
                                        using std::get;
                                        return get<1>(l) < get<1>(r);
                                }
                        );
                        boost::for_each( buf, [](auto&& e){
                                using std::get;
                                std::cout << boost::format("%s -> (%i,%i)\n") 
                                        % get<0>(e) % get<1>(e) % get<2>(e)
                                ;
                        });
                        std::cout << "-------------------------\n";
                }
        private:
                // NOTE that this is only a map for easty prototyping,
                // the point of a map is O( log(n) ) insertion, whislt
                // mainting O( n log(n) ) lookup, I should use a vector
                // then sort just before needed
                std::map<std::string,size_t> str_count_;
                
                using impl_t = std::list<std::pair<size_t,size_t> >;
                impl_t impl_; // (index,ref_count)
                std::map<std::string, impl_t::iterator > mapping_; // mapping: string -> (index,ref_count) 

        };
        struct optimizer_visitor : boost::static_visitor<void>{
                explicit optimizer_visitor(optimizer_context& ctx):ctx_(&ctx){}
                void operator()(float32_t ){
                }
                void operator()(float64_t ){
                }
                void operator()(std::int64_t ){
                }
                void operator()(bool ){
                }
                void operator()(std::nullptr_t ){
                }
                void operator()(binary_ const& ){
                }
                void operator()(ref_ const& ){
                }
                void operator()(insert_ const& ){
                }
                void operator()(std::string const& value){
                        ctx_->inc(value);
                }
                void operator()(array_ const& array){
                        for( auto const& element : array){
                                boost::apply_visitor(*this,element);
                        }
                }
                void operator()(map_ const& map){
                        for( auto const& p : map){
                                boost::apply_visitor(*this,p.first);
                                boost::apply_visitor(*this,p.second);
                        }
                }
        private:
                optimizer_context* ctx_;
        };

        struct all_ref_policy : boost::static_visitor<variant_t>{
                explicit all_ref_policy(optimizer_context& ctx):ctx_(&ctx){}
                variant_t operator()(float32_t value){
                        return value;
                }
                variant_t operator()(float64_t value){
                        return value;
                }
                variant_t operator()(std::int64_t value){
                        return value;
                }
                variant_t operator()(bool value){
                        return value;
                }
                variant_t operator()(std::nullptr_t value){
                        return value;
                }
                variant_t operator()(std::string const& value){
                        return ctx_->map(value);
                }
                variant_t operator()(binary_ const& value){
                        return value;
                }
                variant_t operator()(ref_ const& value){
                        return value;
                }
                variant_t operator()(insert_ const& value){
                        return value;
                }
                variant_t operator()(array_ array){
                        for( auto& element : array){
                                element = boost::apply_visitor(*this,element);
                        }
                        return array;
                }
                variant_t operator()(map_ map){
                        for( auto& p : map){
                                p.first  = boost::apply_visitor(*this,p.first);
                                p.second = boost::apply_visitor(*this,p.second);
                        }
                        return map;
                }
        private:
                optimizer_context* ctx_;
        };

        
} }

namespace gpack{
        inline
        variant_t optimize(variant_t const& root){
                detail::optimizer_context ctx;

                detail::optimizer_visitor obs(ctx);
                detail::all_ref_policy trans(ctx);

                // collect statistics
                boost::apply_visitor(obs,root);
                // tranform
                return boost::apply_visitor(trans,root);
        }
}
