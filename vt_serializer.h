#pragma once

#include "tags.h"
#include "vt.h"

namespace gpack{

        /*
         * vt_serializer is to call appropriate functions
         * of the policy to serialize it
         *
         */
        template<class Policy, class Stream>
        struct vt_serializer : boost::static_visitor<>
        {
                vt_serializer(Policy& policy, Stream& stream)
                        :policy_(&policy),
                        stream_(&stream)
                {}
                void operator()(float32_t value)const{
                        policy_->make( tags::float32_(), *stream_, value );
                }
                void operator()(float64_t value)const{
                        policy_->make( tags::float64_(), *stream_, value );
                }
                void operator()(std::int64_t value)const{
                        policy_->make( tags::int_(), *stream_, value );
                }
                void operator()(bool value)const{
                        policy_->make( tags::bool_(), *stream_, value );
                }
                void operator()(std::nullptr_t value)const{
                        policy_->make( tags::nil_(), *stream_, value );
                }
                void operator()(std::string const& value)const{
                        policy_->make( tags::string_(), *stream_, value );
                }
                void operator()(binary_ const& bin)const{
                        policy_->make( tags::binary_(), *stream_, bin);
                }
                void operator()(array_ const& array)const{
                        policy_->make( tags::array_(), *stream_, array.size());
                        for( auto const& element : array){
                                boost::apply_visitor(*this,element);
                        }
                }
                void operator()(map_ const& map)const{
                        policy_->make( tags::map_(), *stream_, map.size());
                        for( auto const& p : map){
                                boost::apply_visitor(*this,p.first);
                                boost::apply_visitor(*this,p.second);
                        }
                }
                void operator()(insert_ const& value)const{
                        policy_->make( tags::insert_() , *stream_,  value.count()); // TODO
                        boost::apply_visitor(*this, value.get() );
                }
                void operator()(ref_ const& value)const{
                        policy_->make( tags::ref_(), *stream_, value.index() );
                }
        private:
                Policy* policy_;
                Stream* stream_;
        };
}
