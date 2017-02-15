#include "schema.h"
#include "cli.h"
#include "optmizer.h"

namespace cli{

        template<>
        struct serialization_target_with_policy
        <gpack::schema::policy>
                : serialization_target
        {
                using policy_t     = gpack::schema::policy;
                using serializer_t = gpack::vt_serializer<policy_t,backend_t>;

                template<class... Args>
                explicit serialization_target_with_policy(Args&&... args):
                        serialization_target(std::forward<Args>(args)...)
                {}
                void do_preprocess_()final{
                        auto opt_root = gpack::optimize( get_root() );
                        get_root() = opt_root;
                }
                void do_serialize_() final{
                        policy_t p;
                        serializer_t aux(p,get_backend());
                        boost::apply_visitor( aux, get_root() );
                }
        };
}

namespace{
        int _ = ( cli::policy_factory::get().register_policy<gpack::schema::policy>("schema"), 0 );
}

