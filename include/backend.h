#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <bitset>
#include <iostream>

#include <boost/range/algorithm.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/format.hpp>

namespace gpack{ 

	// simple back, nice for debugging
        struct vector_backend{
                using char_vec_t = std::vector<char>;

                void put(char c){
			buffer_.emplace_back(c);
                }
                void write(const char* ptr, size_t n){
			std::copy( ptr, ptr+n, std::back_inserter(buffer_));
                }
                void write(std::string const& str){
			boost::copy( str, std::back_inserter(buffer_));
                }
                void up(){}
                void down(){}
                size_t bytes()const{ return buffer_.size(); }
                void reset(){}

		decltype(auto) get()const{ return buffer_; }
        private:
                std::vector<char> buffer_;
        };






        // this is what the binary stream is written to
	// 
	// It's going to be important that I don't buffer several GB of data
	// at once, so this needs to be continously dispatched (w.r.t. to 
	// some buffering policy later)
        struct dispatch_backend{
                using char_vec_t = std::vector<char>;
                using signal_t = boost::signals2::signal<void(char_vec_t)>;

                template<class Callback_Type>
                auto connect( Callback_Type&& callback){
                        return on_write_.connect(callback);
                }

                void put(char c){
                        push_(std::vector<char>{c});
                }
                void write(const char* ptr, size_t n){
                        std::vector<char> buffer(ptr,ptr+n);
                        push_(buffer);
                }
                void write(std::string const& str){
                        std::vector<char> buffer(str.begin(),str.end());
                        push_(buffer);
                }

                void up(){++sem_;}
                void down(){
                        assert(sem_ != 0); 
                        if( --sem_ == 0 ){
                                if( buffer_.size() ){
                                        on_write_(buffer_);
                                        buffer_.clear();
                                }
                        }
                }
                size_t bytes()const{return count_;}
                void reset(){count_=0;}
        private:
                void push_( std::vector<char> const& buf){
                        count_ += buf.size();
                        if( sem_ == 0){
                                assert( buffer_.empty());
                                on_write_(buf);
                        } else{
                                std::copy( buf.begin(), buf.end(), std::back_inserter(buffer_));
                        }
                }
                std::vector<char> buffer_;
                std::size_t sem_{0};
                signal_t on_write_;
                size_t count_{0};
        };




	// These are object which can connect ot to the dispatch_backend
        struct cout_subscriber : std::enable_shared_from_this<cout_subscriber>
        {
                void connect( dispatch_backend& that){
                        auto self = shared_from_this();
                        con_ = that.connect( [self](auto&& data){ 
                                std::stringstream hex;
                                std::stringstream binary;
                                auto first = data.cbegin(), last = data.cend();
                                for(size_t i=0;first!=last;++i,++first){
                                        if( i != 0 ){ hex << ","; binary << ",";}
                                        hex << "0x" << std::hex << static_cast<unsigned>(static_cast<unsigned char>(*first));
                                        binary << std::bitset<8>(*first).to_string();
                                }
                                std::cout << boost::format("%-30s|%s\n") % hex.str() % binary.str();
                        });
                }
        private:
                boost::signals2::connection con_;
        };

        template<class Stream>
        struct generic_stream_subscriber : std::enable_shared_from_this<generic_stream_subscriber<Stream> >
        {
                explicit generic_stream_subscriber(Stream& stream):stream_(&stream){}
                void connect( dispatch_backend& that){
                        auto self = std::enable_shared_from_this<generic_stream_subscriber>::shared_from_this();
                        con_ = that.connect( [this,self](auto&& data){ 
                                stream_->write( reinterpret_cast<const char*>(data.data()), data.size() );
                        });
                }
        private:
                boost::signals2::connection con_;
                Stream* stream_;
        };
}
