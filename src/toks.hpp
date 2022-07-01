#ifndef TOKS_HDR
#define TOKS_HDR

#include <stdint.h>
#include <stddef.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <tuple>

using usecs_t=uint64_t;

// struct txtfrag { const char*txt; size_t len; };
using txtfrag=std::tuple<const char*,size_t>;

using hash32_t=uint32_t;

inline hash32_t Hash32(const char *txt, size_t len) {
    hash32_t hash = 0x811c9dc5;
    const hash32_t prime = 0x01000193;

    while (len--) {
        hash ^= hash32_t(*txt);
        hash *= prime;
        ++txt;
    }

    return hash;
}


inline usecs_t Now() {
  auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::microseconds>(now).count();
}


/** @brief The input data */
class Input {
public:
  Input( const std::string &fname );
  ~Input();

  void drop();

  inline txtfrag operator * () const { return data; }

  inline size_t size() const { return std::get<1>(data); }

private:
  txtfrag data;
};


/** @brief The output */
using Output = std::array<hash32_t,1000>;

extern void OnePass( Input&txt, Output &out );
extern void BlockWize( Input&txt, Output &out );
extern void WarmUp( Input &txt, Output &out );

#endif // TOKS_HDR
