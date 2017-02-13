#pragma once

#include <functional>
#include <vector>
#include <string>
#include <map>
#include <fstream>

#include <boost/core/null_deleter.hpp>
#include <boost/noncopyable.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/variant.hpp>
#include <boost/exception/all.hpp>

#include "decoder.h"
#include "equal_to.h"
#include "dsl.h"

#include "vt_context.h"
#include "vt_serializer.h"
#include "vt_to_string.h"

/*
 * this file is to write the code for the cli interface helpers
 
 */


// command line interface
namespace cli{
        
        struct serialization_target : private boost::noncopyable
        {
                using backend_t          = gpack::dispatch_backend;
                using variant_t          = gpack::variant_t;
                using nullary_function_t = std::function<void()>;

                explicit serialization_target(variant_t const&  root):root_(root){}
                explicit serialization_target(variant_t      && root):root_(std::move(root)){}

                virtual ~serialization_target()=default;

                void run()
                {
                        do_preprocess_();

                        boost::for_each( 
                                pre_hooks_ 
                                , [](auto&& hook){ 
                                        try{
                                                hook();
                                        } catch( std::exception const& e){
                                                std::cerr << "Error running pre hook: "
                                                        << boost::diagnostic_information(e)
                                                ;
                                        }
                        });

                        do_serialize_();

                        boost::for_each( 
                                post_hooks_ 
                                , [](auto&& hook){ 
                                        try{
                                                hook();
                                        } catch( std::exception const& e){
                                                std::cerr << "Error running post hook: "
                                                        << boost::diagnostic_information(e)
                                                ;
                                        }
                        });
                        
                }
                decltype(auto) get_backend(){ return (backend_);}
                decltype(auto) get_backend()const{ return (backend_);}
                decltype(auto) get_root(){ return (root_);}
                decltype(auto) get_root()const{ return (root_);}

                void push_post_hook(nullary_function_t f){
                        post_hooks_.emplace_back(std::move(f));
                }
                void push_pre_hook(nullary_function_t f){
                        post_hooks_.emplace_back(std::move(f));
                }

                ///////////////////////////////////////////////
                // this is what is important
                ///////////////////////////////////////////////
                virtual void do_serialize_()=0; 
                virtual void do_preprocess_(){}
                ///////////////////////////////////////////////

        private:
                variant_t                     root_;
                gpack::dispatch_backend      backend_;
                std::vector<nullary_function_t> pre_hooks_;
                std::vector<nullary_function_t> post_hooks_;
        };
        

        struct post_hook{
                virtual ~post_hook()=default;
                virtual void start(serialization_target&)=0;
        };


        template<class Policy>
        struct checker : 
                public std::enable_shared_from_this<checker<Policy> >
                , public post_hook
        {

                using policy_t     = Policy;
                using parser_vec   = typename Policy::parser_vec;
                using decoder_t    = gpack::decoder_kernel<parser_vec>;
                using context_t    = gpack::vt_context;
                using subscriber_t = gpack::decoder_subscriber<decoder_t,gpack::vt_context>;
                using variant_t    = gpack::variant_t;

                void start( serialization_target& target ) final {
                        std::make_shared<subscriber_t>( decoder_, ctx_ )
                            ->connect( target.get_backend() );
                        auto self =
                            std::enable_shared_from_this<checker<Policy>>::shared_from_this();
                        target.push_post_hook( [this, self, &target]() {
                                variant_t root_cpy = ctx_.make();
                                bool checker_result = boost::apply_visitor(
                                    gpack::equal_to_(), target.get_root(), root_cpy );
                                // TODO policy?
                                if ( !checker_result ) {
                                        display( root_cpy );
                                        BOOST_THROW_EXCEPTION( std::domain_error(
                                            "injective optimized serialzation failed" ) );
                                }
                        } );
                }
        private:
                decoder_t decoder_;
                context_t ctx_;
        };
        
        struct displayer :
                public std::enable_shared_from_this<displayer>
                , public post_hook
        {
                void start( serialization_target& target)final{
                        target.push_pre_hook( [&target,self=shared_from_this()](){
                                display( target.get_root() );
                        });
                }
        };

        struct profiler :
                public std::enable_shared_from_this<profiler>
                , public post_hook
        {
                void start( serialization_target& target)final{
                        target.push_post_hook( [self=shared_from_this()](){
                                gpack::detail::dsl_dirty_profiler::get().print();
                        });
                }
        };
        
        struct sink_stream : 
                public std::enable_shared_from_this<sink_stream>
                , public post_hook
        {

                using variant_t    = gpack::variant_t;

                explicit sink_stream(std::shared_ptr<std::ostream> ostr):ostr_(ostr){}

                void start(serialization_target& target)final{
                        using subscriber_t = gpack::generic_stream_subscriber<std::ostream>;
                        std::make_shared<subscriber_t>(*ostr_)
                                ->connect( target.get_backend() );

                        // stay alive
                        target.push_post_hook( [self=shared_from_this()](){});

                }
        private:
                std::shared_ptr<std::ostream> ostr_;
        };

        template<class Policy>
        struct serialization_target_with_policy
                : serialization_target
        {
                using policy_t     = Policy;
                using serializer_t = gpack::vt_serializer<policy_t,backend_t>;

                template<class... Args>
                explicit serialization_target_with_policy(Args&&... args):
                        serialization_target(std::forward<Args>(args)...)
                {}
                void do_serialize_() final{
                        policy_t p;
                        serializer_t aux(p,get_backend());

                        boost::apply_visitor( aux, get_root() );
                }
        };

        struct policy_factory : boost::noncopyable{

                // TODO change this to shared_ptr
                using variant_t        = gpack::variant_t;
                using st_handle_t      = std::shared_ptr<serialization_target>;

                static policy_factory& get(){
                        // I'm not sure why but I've been told to do this
                        static std::unique_ptr<policy_factory> mem_(new policy_factory);
                        return *mem_;
                }
                
                struct proxy{
                        virtual ~proxy()=default;
                        virtual st_handle_t make(variant_t const& root)const=0;
                        virtual std::shared_ptr<post_hook> make_hook(std::string const& token)const=0;
                        virtual std::shared_ptr<post_hook> make_hook(std::string const& token, std::string const& arg)const=0;
                };

                struct maker{
                        explicit maker(std::shared_ptr<proxy const> impl):impl_(std::move(impl)){}
                        st_handle_t make(variant_t const& root)const{
                                return impl_->make(root);
                        }
                        std::shared_ptr<post_hook> make_hook(std::string const& token)const{
                                return impl_->make_hook(token);
                        }
                        std::shared_ptr<post_hook> make_hook(std::string const& token, std::string const& arg)const{
                                return impl_->make_hook(token,arg);
                        }
                private:
                        std::shared_ptr<proxy const> impl_;
                };

                template<class Policy>
                void register_policy(std::string const& token){
                        
                        assert( makers_.count(token) == 0 && "unexpected token clash!");

                        struct policy_proxy : proxy{
                                st_handle_t make(variant_t const& root)const final{
                                        using st_t = serialization_target_with_policy<Policy>;
                                        return std::make_shared<st_t>(root);
                                }
                                std::shared_ptr<post_hook> make_hook(std::string const& token)const final{
                                        if( token == "checker"){
                                                return std::make_shared<checker<Policy>>();
                                        } else if( token == "profiler"){
                                                return std::make_shared<profiler>();
                                        } else if( token == "display"){
                                                return std::make_shared<displayer>();
                                        } else if ( token == "stdout-sink" ){
                                                std::shared_ptr<std::ostream> aux(
                                                        &std::cout,
                                                        boost::null_deleter()
                                                );
                                                return std::make_shared<sink_stream>(aux);
                                        } else{
                                                BOOST_THROW_EXCEPTION(std::domain_error("unknown hook " + token));
                                        }
                                        __builtin_unreachable();
                                }
                                std::shared_ptr<post_hook> make_hook( std::string const& token, 
                                                                      std::string const& arg) const final
                                {
                                        if( token == "file-sink"){
                                                auto ofstr = std::make_shared<std::ofstream>(
                                                        arg 
                                                        , std::ofstream::binary
                                                );
                                                if( ! ofstr->is_open() ){
                                                        BOOST_THROW_EXCEPTION(std::domain_error("unable to open file to writing " + arg ));
                                                }
                                                return std::make_shared<sink_stream>(ofstr);
                                        } else{
                                                BOOST_THROW_EXCEPTION(std::domain_error("unknown hook " + token));
                                        }
                                        __builtin_unreachable();
                                }
                        };

                        makers_.insert(
                                std::make_pair(
                                        token,
                                        std::make_shared<policy_proxy>()
                                )
                        );
                }

                

                maker with_policy(std::string const& policy_s)const{
                        if( makers_.count( policy_s ) == 0 )
                                BOOST_THROW_EXCEPTION(std::domain_error("unknown policy " + policy_s));
                        return maker(makers_.find(policy_s)->second);
                }
                // easists way to allow obverving the policy strings
                template<class F>
                void for_each_policy__(F&& f)const{
                        boost::for_each( makers_, [&](auto&& p){ f(p.first); } );
                }
        private:
                policy_factory()=default;
        private:
                std::map<
                        std::string,
                        std::shared_ptr<proxy>
                > makers_;
        };
}

