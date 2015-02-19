// File: inBuffer.hpp
// Computer Systems, Fourth Edition
// Figure 7.27
// Input buffer class

#include <string> // string, getline

class InBuffer {
private:
   string line;
   int lineIndex;

public:
   void getLine () {
      getline (cin, line);
      line.push_back ('\n');
      lineIndex = 0;
   }

   void advanceInput (char& ch) {
      ch = line[lineIndex++];
   }

   void backUpInput () {
      lineIndex--;
   }
};
