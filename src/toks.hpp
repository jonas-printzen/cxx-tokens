#ifndef TOKS_HDR
#define TOKS_HDR

#include <stdint.h>
#include <stddef.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <tuple>

#include <string.h>

using usecs_t=uint64_t;

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

struct txtfrag {
  const char *txt=nullptr;
  size_t      len=0;
};

/** @brief The input data */
class Input : public txtfrag {
public:
  Input( const std::string &fname );
  ~Input();

  void drop();

  inline txtfrag &operator * () { return *this; }
  inline size_t size() const { return len; }
};

/** @brief The output */
inline constexpr size_t BLOCK_SIZE=1000;
using Output = std::array<hash32_t,BLOCK_SIZE>;

extern void OnePass( Input&txt, Output &out );
extern void BlockWize( Input&txt, Output &out );
extern void WarmUp( Input &txt, Output &out );

#endif // TOKS_HDR
