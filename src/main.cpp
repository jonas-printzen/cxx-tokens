#include <toks.hpp>

#include <iostream>

int main(int,char**) {
  std::cout << "Reading 'lorem.txt' ..." << std::endl;
  Input txt("lorem.txt");
  Output out;

  WarmUp( txt, out );

  OnePass(txt,out);
  BlockWize(txt,out);
  return 0;
}
