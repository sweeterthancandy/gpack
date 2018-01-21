#pragma once

#include <ostream>

namespace gpack{
        enum Id{
                Id_String,
                Id_Binary,
                Id_Int,
                Id_Bool,
                Id_Float32,
                Id_Float64,
                Id_Nil,
                Id_Map,
                Id_Array,
                Id_Insert,
                Id_Ref,
        };
        inline std::string Id_to_string(Id e) {
                switch (e) {
                case Id_String:
                        return "String";
                case Id_Binary:
                        return "Binary";
                case Id_Int:
                        return "Int";
                case Id_Bool:
                        return "Bool";
                case Id_Float32:
                        return "Float32";
                case Id_Float64:
                        return "Float64";
                case Id_Nil:
                        return "Nil";
                case Id_Map:
                        return "Map";
                case Id_Array:
                        return "Array";
                case Id_Insert:
                        return "Insert";
                case Id_Ref:
                        return "Ref";
                default:
                        return "(unknown)";
                }
        }
} // gpack

namespace gpack{
namespace tags{


        struct tag_base{
                explicit tag_base(Id id):id_{id}, name_{Id_to_string(id)}{}
                std::string const& name()const{ return name_; }
                Id id()const{ return id_; }
                operator Id()const{ return id(); }
                friend std::ostream& operator<<( std::ostream& ostr, tag_base const& self){
                        return ostr << self.name_;
                }
        private:
                Id id_;
                std::string name_;
        };

        template<Id _id>
        struct decl_tag : tag_base{
                decl_tag():tag_base(_id){}
        };

        struct bool_    : decl_tag<Id_Bool>{};
        struct string_  : decl_tag<Id_String>{};
        struct binary_  : decl_tag<Id_Binary>{};
        struct int_     : decl_tag<Id_Int>{};
        struct float32_ : decl_tag<Id_Float32>{};
        struct float64_ : decl_tag<Id_Float64>{};
        struct nil_     : decl_tag<Id_Nil>{};
        struct map_     : decl_tag<Id_Map>{};
        struct array_   : decl_tag<Id_Array>{};
        struct insert_  : decl_tag<Id_Insert>{};
        struct ret_     : decl_tag<Id_Ref>{};
        
} // tags 
} // gpack
