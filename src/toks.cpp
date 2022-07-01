#include <toks.hpp>
#include <iostream>
#include <stdlib.h>
#include <solution.icpp>

const usecs_t WARMUP_TIME=1000000;
const usecs_t TOKENIZE_TIME=2000000;

const size_t BLOCK_SIZE=1000;

bool CheckHashes( const Output &out );

void WarmUp( Input &txt, Output &out ) {
  std::cout << "Warming up ..." << std::endl;
  // Timeout one second ahead
  auto timeout = Now() + WARMUP_TIME;
  do {
    TokenizeAll( txt, out );
  } while( Now() < timeout );
}

void OnePass( Input&txt, Output &out ) {
  std::cout << "Starting OnePass ... "; std::cout.flush();
  out[0] = 0;
  int64_t tokens = 0;
  usecs_t t0 = Now();
  usecs_t t1=t0;
  usecs_t timeout = t0 + TOKENIZE_TIME;
  do {
    tokens += TokenizeAll( txt, out );
    t1 = Now();
  } while( t1 < timeout );

  double toks_per_usec = tokens ? static_cast<double>(tokens) / (static_cast<double>(t1)-static_cast<double>(t0)) : 0.;

  std::cout << "=> " << std::setprecision(3) << toks_per_usec << " tokens per µs, "
            "CheckHashes() => " << (CheckHashes(out)?"OK":"Nope!") << std::endl;
}

void BlockWize( Input&txt, Output &out ) {
  std::cout << "Starting BlockWize ... "; std::cout.flush();
  out[0] = 0;
  int64_t tokens = 0;
  usecs_t t0 = Now();
  usecs_t t1=t0;
  usecs_t timeout = t0 + TOKENIZE_TIME;
  do {
    auto [p,len] = *txt;
    size_t full_blocks = len/BLOCK_SIZE;
    size_t reminder = len%BLOCK_SIZE;
    while( full_blocks-- ) {
      tokens += TokenizeBlock({p,BLOCK_SIZE},out);
      p += BLOCK_SIZE;
    }
    if( reminder ) {
      tokens += TokenizeBlock({p,reminder},out);
    }

    t1 = Now();
  } while( t1 < timeout );

  double toks_per_usec = tokens ? static_cast<double>(tokens) / (static_cast<double>(t1)-static_cast<double>(t0)) : 0.;

  std::cout << "=> " << std::setprecision(3) << toks_per_usec << " tokens per µs, "
            "CheckHashes() => " << (CheckHashes(out)?"OK":"Nope!") << std::endl;
}

Input::~Input() {
  drop();
}

Input::Input( const std::string &fname ) {
  std::filesystem::directory_entry afile(fname);
  if( !afile.is_regular_file() ) throw std::runtime_error("File not found!");

  size_t sz = afile.file_size();
  char* p = (char*)::malloc(sz);
  data = txtfrag{p,sz};

  std::ifstream infile(fname);
  infile.read(p,sz);
}

void Input::drop() {
  auto p = std::get<0>(data);
  if( p ) {
    ::free((void*)p);
  }
  data = txtfrag{nullptr,0};
}

std::array<hash32_t,1000> good_hashes = {
0x6aa73340,0x1847f92f,0x80afa4d5,0xc630015f,0xcf1f29aa,0xf0d4bb32,0x17bf76f2,0xd1d823b5,
0x60d95848,0x4bc3578b,0x3c99dbb3,0x37386ae0,0x70c60ef2,0x7e1450e5,0xde3876a0,0x1847f92f,
0xe5d11def,0x9ee2a996,0x57251588,0xfa690868,0x1f1a2168,0xc68830e5,0x74305278,0x5dc4ece3,
0x46c75bd7,0x74305278,0x6b1c24ff,0xee396aa3,0x21439604,0x9042a13a,0xc1e4dac9,0x24913ed0,
0x481a708f,0x97bc3f41,0x6e7bb95f,0xdaac3d48,0x274ae844,0x18b18573,0xcdb085fe,0x58664d9c,
0xfa690868,0x57251588,0xc1e4dac9,0x249f5099,0xe42fd382,0xe5d11def,0xee396aa3,0x911b3cc7,
0x4e25075d,0xf9a500ea,0x7e4b6b57,0x2fb1a5fc,0xf1ab507e,0x804b6e7d,0x5e634b89,0xe5d11def,
0xa8f48db,0xc630015f,0x448761c2,0x1a892261,0xa324a792,0x5466c5cc,0xb321be1,0xcd0fb015,
0x4e25075d,0x609b3649,0x80d3770f,0x481a708f,0x747e8ca5,0xfb5a2037,0xee183ab3,0x5e634b89,
0x327a60c1,0xb8fad3d,0xf9a500ea,0xc630015f,0x448761c2,0xd5c8d239,0x754ab6e3,0xe5d11def,
0x96cf2de6,0x804b6e7d,0x5e634b89,0xec038251,0xf6e2f31d,0x96cf2de6,0x6b3f0409,0x61da1409,
0x21bb7e28,0x481a708f,0xf1ab507e,0x8f80b721,0x18b18573,0x57251588,0x499e6d58,0x8dde57bc,
0x6e7bb95f,0x66285e0c,0x545f6ba9,0x96cf2de6,0x4a878bef,0xba59eb42,0x18b18573,0x37386ae0,
0x169e6e72,0x471a6efc,0xf65a1858,0xa176f4a0,0xee183ab3,0x2838ac77,0x566bd741,0x5164cc6b,
0x9902aeea,0xe40c292c,0xab002fea,0xff248b00,0xc6b8e4f5,0x606febe,0x948fd9af,0x18b18573,
0xb626edd3,0xcdb085fe,0x57251588,0x7899c4a7,0xb321be1,0x45f2d3df,0xf0d4bb32,0x96cf2de6,
0x7ac423bd,0xaa90b20,0x661deaac,0x848c2038,0x274ae844,0xd04e840f,0xc1e4dac9,0x1847f92f,
0x31d91c35,0x41387a9e,0x30e0217f,0xdb2b8324,0xec038251,0x471a6efc,0xf9a500ea,0x70c60ef2,
0x85212c46,0x96b1d760,0x21bb7e28,0x4e25075d,0x861c631f,0x9de56e2a,0x2838ac77,0x70c60ef2,
0x327a60c1,0x3c99b58d,0xf9a500ea,0xa00bb92a,0x5e634b89,0x2fb1a5fc,0xc1e4dac9,0xa022b70f,
0x9902aeea,0x6e7bb95f,0xc630015f,0xcf1f29aa,0xdaac3d48,0xda44d949,0x18b18573,0x4a878bef,
0x669dd15f,0x65a6983d,0x60438345,0x97bc3f41,0x4b785d73,0xa19000d0,0x96cf2de6,0xff208b9f,
0x78ae3d10,0xd3ecb92d,0xd5611452,0x18b18573,0xb626edd3,0x609b3649,0x2fb1a5fc,0xb4d19f02,
0xec038251,0x645f9652,0xd9a953e5,0xe9a8268,0xe42fd382,0xc68830e5,0x74305278,0x63c96454,
0x46c75bd7,0x41182780,0x56a409d9,0x3c99b58d,0x45f2d3df,0x8e78854d,0xcdb085fe,0x499e6d58,
0x669dd15f,0x3c4cffb9,0x57251588,0xe23aed05,0xdb2b8324,0xcdb085fe,0x4a878bef,0x5dc4ece3,
0x40e8c8be,0xff9523d5,0x35a41549,0x3ed8855d,0x471a6efc,0x6e7bb95f,0xbe9c7170,0xf13c62e9,
0xcbf10bbc,0x5a6650c2,0x1e70ec0,0x760bc73f,0xf967bdfa,0x97bc3f41,0x471a6efc,0xe42fd382,
0x6f8ec73,0xaa991652,0x41387a9e,0x7b56a425,0x25c1f16b,0x9694f542,0x60438345,0x60777b60,
0xcdb085fe,0xe40c292c,0x3d0d9691,0x198b487c,0x1847f92f,0x566bd741,0x89272d32,0x24913ed0,
0x471a6efc,0xb321be1,0x609b3649,0x376adc9f,0x2fb1a5fc,0x33a41223,0x25c1f16b,0x481a708f,
0x212f7b02,0x98faa17,0x2838ac77,0x3c99dbb3,0x8778cd56,0x909e3c1c,0x1a892261,0x4743116c,
0x499e6d58,0x4e25075d,0xb065f7fb,0x2838ac77,0x9042a13a,0x6b3f0409,0x3a225e0c,0xe40c292c,
0xff208b9f,0xf6047dc,0x724d29b3,0x774aba09,0x18b18573,0xb172b3c,0xf65a1858,0x5164cc6b,
0x41387a9e,0x66285e0c,0x88380590,0xa176f4a0,0xfb70b4e3,0xbab7f296,0xa127bddc,0xf967bdfa,
0x4b785d73,0x57251588,0x891ae26b,0xb9d01f24,0x70c60ef2,0x4a878bef,0x8f4b0c2a,0x911b3cc7,
0x1c75d20d,0xcf098f45,0x4b1a7548,0x41387a9e,0x79580632,0x354eb1f6,0x4b785d73,0xc139cc97,
0x212f7b02,0x8dde57bc,0x7c144dbf,0x354eb1f6,0x481a708f,0x56a409d9,0x1c5235c5,0x82bb3a0f,
0x6e7bb95f,0x249f5099,0xca31021d,0xd1fd9244,0x2838ac77,0x4b1a7548,0x78c42097,0x661deaac,
0xb626edd3,0x88380590,0x322514fe,0xe5d11def,0x98256b28,0x631063ab,0x21439604,0x724d29b3,
0x4b1a7548,0x651066d1,0x2ad52611,0x4d7cb102,0x327a60c1,0x41210e5a,0xd5611452,0x46f35f8c,
0x4e25d338,0xbdc5cb88,0xf6e2f31d,0xe42fd382,0x513be7a3,0x2ca5d816,0x6b3f0409,0xe40c292c,
0xcd0fb015,0xc630015f,0x448761c2,0x2844c75a,0xb087fce5,0x4e25075d,0xdaac3d48,0x3c226132,
0x909e3c1c,0x481a708f,0xcd438540,0x471a6efc,0xb2a5b2b9,0x1dd4e360,0x4d7cb102,0x4a878bef,
0x2fb1a5fc,0xf54b318c,0xd9a953e5,0x41210e5a,0x4e25075d,0xb4d19f02,0x8dde57bc,0x433b1432,
0xaa90b20,0x80afa4d5,0x7e4b6b57,0x2ad52611,0x481a708f,0x169de566,0xbe9c7170,0x1a892261,
0x37386ae0,0xc7e0875b,0xd9a953e5,0x37386ae0,0x30e0217f,0x3a4cfc93,0x911b3cc7,0xae63eba6,
0xe2001682,0x58664d9c,0x4e25d338,0xf033c2e9,0x4b1a7548,0x545f6ba9,0x302511d8,0x21439604,
0xa8f48db,0x5164cc6b,0x724d29b3,0x3c99b58d,0x5c2e17c3,0xb9d01f24,0xe5d11def,0x1234093,
0x7ac423bd,0xd04e840f,0x1847f92f,0xa19000d0,0x169de566,0xe40c292c,0x88380590,0xff248b00,
0xb321be1,0xbdc5cb88,0x7b56a425,0x18b18573,0x3698f000,0x9be56b04,0xd3cd1239,0x41387a9e,
0x566bd741,0x249f5099,0x873ebbb5,0xe23aed05,0xf54b318c,0x911b3cc7,0x606febe,0xf6e2f31d,
0xb626edd3,0xc1e4dac9,0x1c75d20d,0x6ee1c6dd,0x5f56818c,0x2ad52611,0xe5d11def,0x5fc4f009,
0x37386ae0,0x8be04440,0x1847f92f,0xc139cc97,0x1248e26,0x46c75bd7,0x3ed8855d,0xf78fd0d7,
0xab002fea,0x8dde57bc,0xfb09c09c,0xaa90b20,0xcf098f45,0xe5d11def,0xc1e4dac9,0x471a6efc,
0xee373ed1,0x909e3c1c,0xc630015f,0x448761c2,0xdaac3d48,0xc7e0875b,0x46c75bd7,0x4a878bef,
0xbd26f8d8,0x57251588,0x88380590,0x66285e0c,0x61da1409,0xd04e840f,0x2d7afc8d,0xf6047dc,
0x8dde57bc,0x848c2038,0xa53921b2,0x5ec4db40,0xa8f48db,0x6c57164f,0x364a6574,0x2fb1a5fc,
0x2ad52611,0x566bd741,0x499e6d58,0x2fb1a5fc,0xb97f8035,0x18b18573,0xc139cc97,0x4b785d73,
0xe43a463,0xd04e840f,0x1234093,0x8dde57bc,0x760bc73f,0x41387a9e,0xe87b9f45,0x2838ac77,
0x545f6ba9,0xcf098f45,0x877ee987,0x1e70ec0,0x97bc3f41,0x3698f000,0xc630015f,0x448761c2,
0x70c60ef2,0xac8b352d,0x25c1f16b,0x66285e0c,0xfb5a2037,0x8dde57bc,0xcd438540,0x625f932c,
0x8dde57bc,0xe23aed05,0x5f3dd750,0x41d47283,0x354eb1f6,0x7055ce8b,0xc139cc97,0x78ae3d10,
0x471a6efc,0x43182aa6,0x5f56818c,0x8dde57bc,0x57291d4c,0x5b3bafd5,0x25c1f16b,0x96cf2de6,
0xbd26f8d8,0xb626edd3,0x5c2e17c3,0xf9a500ea,0x89272d32,0x4e25075d,0x8dde57bc,0x65a6983d,
0x5466c5cc,0x9afb48de,0x4f478064,0x873ebbb5,0xee47a7db,0x471a6efc,0x3885ad2c,0x471a6efc,
0xa176f4a0,0x8db18d87,0x4e25075d,0x1f1a2168,0x877ee987,0x5466c5cc,0x1a892261,0x41210e5a,
0x8c788227,0x354eb1f6,0x661deaac,0x376adc9f,0x3a4cfc93,0xbab7f296,0x37386ae0,0x747e8ca5,
0x3a225e0c,0x8dde57bc,0xf9a500ea,0xee396aa3,0x2ca5d816,0x57291d4c,0xdb2b8324,0x3a225e0c,
0x8dde57bc,0xf033c2e9,0x376adc9f,0x724d29b3,0x5be92310,0xd9a953e5,0xb626edd3,0x609b3649,
0x65a6983d,0x46f35f8c,0xf033c2e9,0x499e6d58,0x8dde57bc,0x5c2e17c3,0x57291d4c,0x8d761b74,
0xb087fce5,0x6bba32f3,0x861c631f,0x96cf2de6,0x499e6d58,0xf13c62e9,0x9042a13a,0xb626edd3,
0x96cf2de6,0x7ac423bd,0x46f35f8c,0x6b3f0409,0xdb2b8324,0xc630015f,0x448761c2,0x76eba271,
0x79580632,0x40e8c8be,0xa176f4a0,0x6b1c24ff,0xe34a37c4,0x25c1f16b,0x97bc3f41,0x6b1c24ff,
0x1847f92f,0x4e25075d,0xe34a37c4,0x50f97d9f,0xf24fccf5,0x56a8fb1,0xe4d0b67,0x4724fc58,
0xb2c0799e,0xb0c784b9,0x6b4b8da4,0x634851f6,0xe98d0d58,0x6b4b8da4,0xeb022eec,0xd3f22250,
0x46c75bd7,0x8a497fa1,0x478cb439,0x4743116c,0x606febe,0x8a497fa1,0xf6047dc,0x5f3dd750,
0xf033c2e9,0xa19000d0,0x4e25075d,0x747e8ca5,0xcd0fb015,0xde3876a0,0x3c99b58d,0xf7f48be,
0x1e70ec0,0xf78fd0d7,0x6bba32f3,0x3a4cfc93,0xb087fce5,0x74305278,0x724d29b3,0x224499a5,
0x21439604,0x6bba32f3,0xb4d19f02,0x5fc4f009,0x3c99b58d,0x545f6ba9,0x7899c4a7,0x669dd15f,
0xd5611452,0x909e3c1c,0x1a892261,0x97bc3f41,0xb77f7d0f,0x2fb1a5fc,0x6c57164f,0x7899c4a7,
0x8be04440,0xe703dad6,0x909e3c1c,0x471a6efc,0xe23aed05,0x45d481c2,0x40e8c8be,0x74305278,
0xa8f48db,0x4a878bef,0xa8f48db,0x545f6ba9,0xf4bc718,0xd04e840f,0x78ae3d10,0xb77f7d0f,
0x1234093,0x2fb1a5fc,0xdaac3d48,0x166103f9,0x169de566,0x2fb1a5fc,0x9ce2a670,0x911b3cc7,
0x1a892261,0x6b1c24ff,0x9e7536d6,0x911b3cc7,0x4a878bef,0xf033c2e9,0x5364cf91,0x25c1f16b,
0x9694f542,0x2ca5d816,0x471a6efc,0xbd26f8d8,0x3c99b58d,0x80afa4d5,0xc3709b50,0xc6b8e4f5,
0x471a6efc,0x74305278,0xf54b318c,0xbab7f296,0xe40c292c,0x3c99dbb3,0x471a6efc,0xf6047dc,
0x1dd4e360,0xdaac3d48,0x4e25075d,0x74305278,0x334b827,0x2ca5d816,0x74305278,0x545f6ba9,
0xf767bad4,0x5466c5cc,0x9902aeea,0x481a708f,0x499e6d58,0x37386ae0,0x8d761b74,0x354eb1f6,
0x57251588,0x376adc9f,0x9de56e2a,0x60438345,0xdaac3d48,0xdb2b8324,0x3c99b58d,0x3698f000,
0x704df829,0x57251588,0x1c75d20d,0x861c631f,0xb01044c1,0x60d95848,0x471a6efc,0x169e6e72,
0x41387a9e,0xf6e2f31d,0x6b1c24ff,0x4e25d338,0xe40c292c,0x57251588,0xee396aa3,0x21bb7e28,
0x74305278,0x80afa4d5,0x7b56a425,0xa257d10f,0x873ebbb5,0x861c631f,0x3a4cfc93,0x40e8c8be,
0x57291d4c,0x6e7bb95f,0xb626edd3,0x760bc73f,0x294aeb6a,0x96cf2de6,0x4e25d338,0x861c631f,
0xee183ab3,0x354eb1f6,0xe2001682,0x5c2e17c3,0xe40c292c,0x80afa4d5,0x41182780,0x471a6efc,
0xf0d4bb32,0x7899c4a7,0xdb644ead,0x18b18573,0xcbf10bbc,0xc1e4dac9,0x481a708f,0xf6e2f31d,
0xf78fd0d7,0x43182aa6,0x60438345,0x4a878bef,0xd5c8d239,0x78c42097,0x4a878bef,0xe42fd382,
0x8f4b0c2a,0x5466c5cc,0xf0d4bb32,0x606febe,0x1f1a2168,0xb626edd3,0x59364585,0xbab7f296,
0x5b830a57,0x6e7bb95f,0x66285e0c,0x661deaac,0x8d761b74,0xd9a953e5,0x4743116c,0x339d330c,
0x60e2a18a,0x46f35f8c,0x4743116c,0x257c471f,0xe58a5fc3,0x5466c5cc,0x8dde57bc,0xbd26f8d8,
0xcfe685a,0xe23aed05,0xa8f48db,0xd361112c,0xf9a500ea,0x5364cf91,0x60d95848,0xb321be1,
0x56a409d9,0x593bacaf,0x8dde57bc,0x724d29b3,0xf65a1858,0x5b830a57,0x5be92310,0x5f56818c,
0x4a878bef,0x7899c4a7,0x413b110c,0xc6b8e4f5,0xa127bddc,0xbe9c7170,0x661deaac,0x2e310a1a,
0x21439604,0x6e7bb95f,0x593bacaf,0x41210e5a,0x4e25075d,0xb4d19f02,0xff248b00,0x30e0217f,
0x873ebbb5,0x948fd9af,0xaa90b20,0x97bc3f41,0xf6e2f31d,0x98faa17,0x25c1f16b,0xb4d19f02,
0xa19000d0,0xa176f4a0,0x5b830a57,0xc68830e5,0xd361112c,0x6b1c24ff,0x724d29b3,0xa0a6cf92,
0x911b3cc7,0x74305278,0xc3709b50,0x364a6574,0x2fb1a5fc,0x6b3f0409,0x2e310a1a,0xd04e840f,
0xc1e4dac9,0x1847f92f,0x31d91c35,0x41387a9e,0x30e0217f,0xdb2b8324,0xec038251,0x471a6efc,
0xf9a500ea,0x70c60ef2,0x85212c46,0x96b1d760,0xa257d10f,0x760bc73f,0x6bbee96a,0xca31021d,
0x41387a9e,0xaa991652,0xff248b00,0xe2001682,0xc630015f,0x448761c2,0x5364cf91,0xaa90b20,
0x57251588,0x376adc9f,0xbc967f7c,0x606febe,0x861c631f,0xfd09c3c2,0xf13c62e9,0x52c7a421,
0x354eb1f6,0xbdc5cb88,0xde3876a0,0x471a6efc,0xff9523d5,0x413b110c,0x37386ae0,0x7055ce8b,
0x3c99dbb3,0x4421d3ca,0x909e3c1c,0xec038251,0x257c471f,0x51b02949,0xe40c292c,0xf0d4bb32,
0xff9523d5,0x6bba32f3,0x146100d3,0x60438345,0x96cf2de6,0x57291d4c,0x78c42097,0x74305278,
0xe5d11def,0x948fd9af,0xb087fce5,0x98256b28,0xde3876a0,0x57251588,0x566bd741,0xc139cc97,
0x82c15501,0x46c75bd7,0x2fb1a5fc,0x3ed8855d,0xc7e0875b,0x354eb1f6,0x1234093,0x78ae3d10,
0x8b269bb,0xb626edd3,0xf0d4bb32,0xb2a5b2b9,0x3ed8855d,0xe6a80996,0xbab7f296,0x747e8ca5,
0xaa991652,0x24913ed0,0x4a878bef,0x66285e0c,0x376adc9f,0x339d330c,0xbbd0224a,0xa257d10f,
0x1dd4e360,0xc6b8e4f5,0x7e4b6b57,0x8dde57bc,0xf033c2e9,0x334b827,0x60438345,0x545f6ba9,
0x76eba271,0xedbc81f3,0x18b18573,0xca31021d,0xd3cd1239,0x7956a0ff,0xb626edd3,0xf1ab507e,
0xcd438540,0x3d0d9691,0x1248e26,0x18b18573,0x7899c4a7,0xb859e81c,0x747e8ca5,0xb626edd3,
0x97bc3f41,0xbe48a86d,0xf9a500ea,0x8dde57bc,0x6c5a3b5a,0xd9a953e5,0x7423b3ea,0x478cb439,
0x96cf2de6,0x661deaac,0x4a57d897,0xff9523d5,0xb2a5b2b9,0xff208b9f,0x7956a0ff,0xb626edd3,
0x4e25d338,0x80afa4d5,0x80afa4d5,0x481a708f,0xd1fd9244,0xf638135f,0x4e25075d,0x1dd4e360,
0x384a689a,0x5f56818c,0x89272d32,0x1dd4e360,0xa8f48db,0xb626edd3,0xc2c0348c,0x3c74f167};


bool CheckHashes( const Output &out ) {
  return out == good_hashes;
}
