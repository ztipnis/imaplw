//test_miniz
#include <miniz.h>
#include <string>
#include <sstream>
#include <boost/log/trivial.hpp>
#include <iostream>
#define ifThenElse(a,b,c) (a ? b : c)
#define min(a,b) ( a < b ? a : b)
#define max(a,b) ( a > b ? a : b)
#define Z_BUF_SIZE (1024*1024)
const std::string deflate(const std::string& data, const int level){
  z_stream strm = {0};
  std::string outbuf(Z_BUF_SIZE, 0);
  std::string inbuf(min(Z_BUF_SIZE, data.length()), 0);
  std::stringstream buf;
  strm.next_out = reinterpret_cast<unsigned char*>(&outbuf[0]);
  strm.avail_out = Z_BUF_SIZE;
  strm.next_in = reinterpret_cast<unsigned char*>(&inbuf[0]);
  strm.avail_in = 0;

  uint remaining = data.length();

  if(deflateInit(&strm, level) != Z_OK){
    BOOST_LOG_TRIVIAL(error) << "Unable to init deflate";
    return data;
  }
  while(1){
    if(!strm.avail_in){
      uint bytesToRead = min(Z_BUF_SIZE, remaining);
      inbuf = data.substr(data.length() - remaining, bytesToRead);
      strm.next_in = reinterpret_cast<unsigned char*>(&inbuf[0]);
      strm.avail_in = bytesToRead;
      remaining -= bytesToRead;
    }
    int status = deflate(&strm, ifThenElse(remaining, Z_NO_FLUSH, Z_FINISH));
    if(status == Z_STREAM_END || (!strm.avail_out)){
      // Output buffer is full, or compression is done.
      uint n = Z_BUF_SIZE - strm.avail_out;
      buf << outbuf.substr(0,n);
      std::fill(outbuf.begin(), outbuf.end(), 0);
      strm.next_out = reinterpret_cast<unsigned char*>(&outbuf[0]);
      strm.avail_out = Z_BUF_SIZE;
    }
    if(status == Z_STREAM_END)
      break;
    else if(status != Z_OK)
      BOOST_LOG_TRIVIAL(error) << "Deflate status not 'OK'";
      return data;
  }
  if(deflateEnd(&strm) != Z_OK){
    BOOST_LOG_TRIVIAL(warning) << "zlib unable to cleanup deflate stream";
  }
  std::string ret = buf.str();
  if(ret.length() != strm.total_out){
    BOOST_LOG_TRIVIAL(error) << "Output size mismatch - Expected: " << strm.total_out << " Got: " << ret.length(); 
  }
  // BOOST_LOG_TRIVIAL(trace) << "DEFLATE: " << ((ret.length() * 100) / data.length()) << "%" << ret;
  return ret;
}

const std::string inflate(const std::string& data){
  z_stream strm = {0};
  std::string outbuf(Z_BUF_SIZE, 0);
  std::string inbuf(Z_BUF_SIZE, 0);
  std::stringstream buf;
  strm.next_out = reinterpret_cast<unsigned char*>(&outbuf[0]);
  strm.avail_out = Z_BUF_SIZE;
  strm.next_in = reinterpret_cast<unsigned char*>(&inbuf[0]);
  strm.avail_in = 0;
  uint remaining = data.length();
  if(inflateInit(&strm) != Z_OK){
    throw std::runtime_error("Unable to inflate command data: Init failed.\n");
  }
  while(1){
    if(!strm.avail_in){
      uint bytesToRead = min(Z_BUF_SIZE, remaining);
      inbuf = data.substr(data.length() - remaining, bytesToRead);
      strm.next_in = reinterpret_cast<unsigned char*>(&inbuf[0]);
      strm.avail_in = bytesToRead;
      remaining -= bytesToRead;
    }
    int status = inflate(&strm, Z_SYNC_FLUSH);
    if ((status == Z_STREAM_END) || (!strm.avail_out)){
        // Output buffer is full, or decompression is done
      uint n = Z_BUF_SIZE - strm.avail_out;
      buf << outbuf.substr(0,n);
      std::fill(outbuf.begin(), outbuf.end(), 0);
      strm.next_out = reinterpret_cast<unsigned char*>(&outbuf[0]);
      strm.avail_out = Z_BUF_SIZE;
    }
    if(status == Z_STREAM_END)
      break;
    else if(status != Z_OK)
      throw std::runtime_error("Unable to inflate");
  }
  if(inflateEnd(&strm) != Z_OK){
    BOOST_LOG_TRIVIAL(warning) << "zlib unable to cleanup inflate stream";
  }
  std::string ret = buf.str();
  if(ret.length() != strm.total_out){
    BOOST_LOG_TRIVIAL(error) << "Output size mismatch - Expected: " << strm.total_out << " Got: " << ret.length(); 
  }
  return ret;
}

int main(int argc, char** argv){
	std::string data = "A001 CAPABILITY\nA001 CAPABILITY\nA001 CAPABILITY\nA001 CAPABILITY\nA001 CAPABILITY\n";
	std::cout << "Data:" <<data << std::endl;
	std::string cdata = deflate(data, 4);
	std::cout << "Deflated:" <<cdata << std::endl;
	std::string udata = inflate(cdata);
	std::cout << "Inflated:" <<udata << std::endl;
	return 0;
}
