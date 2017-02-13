#pragma once

#include <boost/preprocessor.hpp>

#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>

#include <boost/xpressive/xpressive.hpp>

namespace json_parser{ namespace detail{ 

        namespace xpr = boost::xpressive;

        #define TOKEN_TYPES/**/\
                (left_curl)\
                (right_curl)\
                (left_br)\
                (right_br)\
                (comma)\
                (colon)\
                (string_)\
                (int_)\
                (float_)\
                (true_)\
                (false_)\
                (null_)\
                (id_)\
                (dummy)\

        #define TOKEN_ENUM_AUX(r,data,i,elem) BOOST_PP_COMMA_IF(i) elem
        #define TOKEN_OSTREAM_AUX(r,data,elem)\
                case token_type::elem:\
                        return ostr << "token_type::" BOOST_PP_STRINGIZE(elem);
        enum class token_type{
                BOOST_PP_SEQ_FOR_EACH_I(TOKEN_ENUM_AUX,~,TOKEN_TYPES)
        };
        std::ostream& operator<<(std::ostream& ostr, token_type type){
                switch(type){
                        BOOST_PP_SEQ_FOR_EACH(TOKEN_OSTREAM_AUX,~,TOKEN_TYPES)
                        default:
                                __builtin_unreachable();
                }
        }
        #undef TOKEN_OSTREAM_AUX
        #undef TOKEN_ENUM_AUX
        #undef TOKEN_TYPES

        #define TOKENIZER_ERROR( MSG )                                                 \
                do {                                                                   \
                        BOOST_THROW_EXCEPTION(                                         \
                            std::domain_error( str( boost::format( "%s (left=%s)" ) %  \
                                                    ( MSG ) % whats_left() ) ) );      \
                } while ( 0 )


        struct token{
                token(token_type _type, std::string const& _value)
                        :type_(_type),value_(_value)
                {}
                token_type type()const{return type_;}
                std::string const& value()const{return value_;}

                friend std::ostream& operator<<(std::ostream& ostr, token const& tok){
                        return ostr << "(" << tok.type() << ",\"" << tok.value() << "\")";
                }
        private:
                token_type type_;
                std::string value_;
        };

        template<class Iter>
        struct tokenizer{
                struct state_t{
                        Iter first_, last_;
                        boost::optional<token> peak_;
                };

                tokenizer(Iter first, Iter last){
                        state_.first_ = first;
                        state_.last_ = last;

                        next();
                }
                bool eos()const{return state_.first_ == state_.last_;}
                boost::optional<token> peak(){return state_.peak_;}
                boost::optional<token> next(){
                        state_.peak_ = next_();
                        //PRINT(std::distance(state_.first_,state_.last_));
                        return state_.peak_;
                }

                state_t save_state_please()const{
                        return state_;
                }
                void restore_this_state_if_you_would_please(state_t proto){
                        state_ = proto;
                }
                std::string whats_left()const{
                        return std::string(
                                state_.first_,
                                std::next(
                                        state_.first_,
                                        std::min<size_t>(
                                                30,
                                                std::distance(state_.first_,state_.last_)
                                        )
                                )
                        );
                }
        private:
                boost::optional<token> next_(){

                        // eat whitespace
                        for(;state_.first_!=state_.last_;++state_.first_)
                                if( ! std::isspace(*state_.first_))
                                        break;
                        if( state_.first_ == state_.last_)
                                return boost::none;

                        switch(*state_.first_){
                                case '{': ++state_.first_; return token(token_type::left_curl ,"{");
                                case '}': ++state_.first_; return token(token_type::right_curl,"}");
                                case '[': ++state_.first_; return token(token_type::left_br   ,"[");
                                case ']': ++state_.first_; return token(token_type::right_br  ,"]");
                                case ',': ++state_.first_; return token(token_type::comma     ,",");
                                case ':': ++state_.first_; return token(token_type::colon     ,":");
                                case '"':{
                                        auto iter = state_.first_;
                                        ++iter;
                                        for(;*iter != '"';++iter){
                                                if( iter == state_.last_)
                                                        TOKENIZER_ERROR("unterminated string");
                                        }
                                        assert( *iter == '"');
                                        token tmp(token_type::string_,
                                                std::string(
                                                        std::next(state_.first_),
                                                        iter));
                                        state_.first_ = std::next(iter);
                                        return tmp;
                                }
                                default:
                                        break;
                        }

                        // is it an int or float literals?
                        if( std::isdigit(*state_.first_) || *state_.first_ == '+' || *state_.first_ == '-'){
                                auto iter = state_.first_;
                                ++iter;

                                switch(*state_.first_ ){
                                        case '+':
                                        case '-':
                                                // must be followed by digit
                                                if( iter == state_.last_  || ! std::isdigit( *iter ) )
                                                        TOKENIZER_ERROR("+/- not followed by digit");
                                                break;
                                        default:
                                                break;
                                }

                                for(; iter != state_.last_ && std::isdigit(*iter);++iter);
                                if( *iter == '.'){
                                        ++iter;
                                        if( ! std::isdigit(*iter) ){
                                                TOKENIZER_ERROR("not a valid numeric literal");
                                        }
                                        for(; iter != state_.last_ && std::isdigit(*iter);++iter);
                                        auto tmp = token(
                                                token_type::float_, 
                                                std::string(
                                                        state_.first_,
                                                        iter));
                                        state_.first_ = iter;
                                        return std::move(tmp);
                                } else{
                                        auto tmp = token(
                                                token_type::int_, 
                                                std::string(
                                                        state_.first_,
                                                        iter));
                                        state_.first_ = iter;
                                        return std::move(tmp);
                                }
                        }  else if( std::isalpha(*state_.first_) || *state_.first_ == '_' ){
                                auto iter = state_.first_;
                                for(;std::isalnum(*iter) || *iter == '_';++iter){
                                        if( iter == state_.last_){
                                                break;
                                        }
                                }
                                token tmp(token_type::string_,
                                        std::string(
                                                state_.first_,
                                                iter));
                                state_.first_ = iter;
                                return tmp;
                        } else{
                                std::stringstream sstr;
                                sstr << "unregognized sequence of chars (";
                                sstr << std::string(state_.first_,
                                        std::next(state_.first_,
                                                std::min<size_t>/**/(
                                                        20,
                                                        std::distance(state_.first_,state_.last_)
                                                )
                                        ));
                                sstr << ")";
                                BOOST_THROW_EXCEPTION(std::domain_error(sstr.str()));
                        }
                }
        private:
                state_t state_;
        };

        inline
        bool operator^(token const& l_param, token_type r_param)noexcept{
                return l_param.type() == r_param;
        }
        #undef TOKENIZER_ERROR
} }