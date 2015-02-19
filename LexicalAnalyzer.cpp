// File: LexicalAnalyzer.cpp
// Authored by Chris Sturtevant
// chris@xris.us
// Sourced from Computer Systems, Fourth Edition
// Figure 7.50
// A multiple-token parser

#include <iostream>
#include <iomanip>
#include <cctype> // isalpha, isdigit
#include <string> // string, getline
using namespace std;

#include "inBuffer.hpp" // InBuffer
InBuffer inBuffer;

enum Token {eT_EMPTY, eT_INVALID, eT_INTEGER1, eT_INTEGER2, eT_HEXADECIMAL2, eT_IDENTIFIER,
            eT_DOTCOMMAND2, eT_ADDRESS2 };

class AToken {
public:
   virtual Token tokenType () = 0;
   virtual void printToken () = 0;
};

class TEmpty : public AToken {
public:
   Token tokenType () { return eT_EMPTY; }
   void printToken () { cout << "Empty token" << endl; }
}; //Final State

class TInvalid : public AToken {
public:
   Token tokenType () { return eT_INVALID; }
   void printToken () { cout << "Syntax error" << endl; }
}; //Final State

class TInteger1 : public AToken {
private:
   int intValue;
public:
   TInteger1(int i) { intValue = i; }
   Token tokenType () { return eT_INTEGER1; }
   void printToken () { cout << "Integer1   = " << intValue << endl; }
};  //Final State

class TInteger2 : public AToken {
private:
   int intValue;
public:
   TInteger2(int i) { intValue = i; }
   Token tokenType () { return eT_INTEGER2; }
   void printToken () { cout << "Integer2   = " << intValue << endl; }
}; //Final State

class TIdentifier : public AToken {
private:
   string identValue;
public:
   TIdentifier (string str) { identValue = str; }
   Token tokenType () { return eT_IDENTIFIER; }
   void printToken () { cout << "Identifier = " << identValue << endl; }
}; //Final State



class THexadecimal2 : public AToken {
private:
   int hexValue;
public:
   THexadecimal2(int i) { hexValue = i; }
   Token tokenType () { return eT_HEXADECIMAL2; }
   void printToken () { cout << "Hexadecimal   = " << hexValue << endl; }
};  //Final State

class TDotCommand : public AToken {
private:
   string dotCmdValue;
public:
   TDotCommand (string str) { dotCmdValue = str; }
   Token tokenType () { return eT_DOTCOMMAND2; }
   void printToken () { cout << "Dot Command = " << dotCmdValue << endl; }
}; //Final State

class TAddress : public AToken {
private:
   string addressValue;
public:
   TAddress (string str) { addressValue = str; }
   Token tokenType () { return eT_ADDRESS2; }
   void printToken () { cout << "Address Mode = " << addressValue << endl; }
}; //Final State

enum State {eS_START, eS_STOP, eS_SIGN, eS_INTEGER1, eS_INTEGER2, eS_HEX1, eS_HEX2,
            eS_IDENTIFIER, eS_ADD, eS_DOT2, eS_ADDRESS1, eS_ADDRESS2};

void getToken (AToken*& pAT) {
   // Pre: pAT is set to a token object.
   // Post: pAT is set to a token object that corresponds to
   // the next token in the input buffer.
   State state;
   char nextChar;
   int sign;
   int localIntValue;
   string localIdentValue;
   string localDotCmdValue;
   string localAddressValue;
   string localHexString = "0x";
   delete pAT;
   pAT = new TEmpty;
   state = eS_START;
   do {
      inBuffer.advanceInput (nextChar);
      switch (state) {
      case eS_START:
         if (isalpha(nextChar)) {
            localIdentValue = nextChar;
            state = eS_IDENTIFIER;
         }
         else if (nextChar == '-') {
            sign = -1;
            state = eS_SIGN;
         }
         else if (nextChar == '+') {
            sign = +1;
            state = eS_SIGN;
         }
         else if (nextChar == '0') {
            localIntValue = nextChar - '0';
            sign = +1;
            state = eS_INTEGER1;
         }
         else if (isdigit(nextChar)) {
            localIntValue = nextChar - '0';
            sign = +1;
            state = eS_INTEGER2;
         }
         else if (nextChar == '.') {
            localDotCmdValue = nextChar;
            state = eS_ADD;
         }
         else if (nextChar == ',') {
            localAddressValue = nextChar;
            state = eS_ADDRESS1;
         }
         else if (nextChar == '\n') {
            state = eS_STOP;
         }
         else if (nextChar != ' ') {
            delete pAT;
            pAT = new TInvalid;
         }
         break;
      case eS_IDENTIFIER:
         if (isalpha (nextChar) || isdigit (nextChar)) {
            localIdentValue.push_back (nextChar);
         }
         else {
            inBuffer.backUpInput ();
            delete pAT;
            pAT = new TIdentifier (localIdentValue);
            state = eS_STOP;
         }
         break;//End of Identifier
      case eS_SIGN:
         if (isdigit (nextChar)) {
            localIntValue = nextChar - '0';
            state = eS_INTEGER2;
         }
         else {
            delete pAT;
            pAT = new TInvalid;
         }//End of Sign, also use for invalid
         break;
         case eS_INTEGER1:
            if (isdigit (nextChar)) {
               localIntValue = 10 * localIntValue + nextChar - '0';
               state = eS_INTEGER2;
            }
            else if (nextChar == 'x'){
               state = eS_HEX1;
            }//Changes the state to Hex 1 if it sees an x after a 0
            else {
               inBuffer.backUpInput ();
               delete pAT;
               pAT = new TInteger1(sign * localIntValue);
               state = eS_STOP;
            }
              break; //End eS_INTEGER1
      case eS_INTEGER2:
         if (isdigit (nextChar)) {
            localIntValue = 10 * localIntValue + nextChar - '0';
         }
         else {
            inBuffer.backUpInput ();
            delete pAT;
            pAT = new TInteger2(sign * localIntValue);
            state = eS_STOP;
         }
         break; //End eS_INTEGER2
         case eS_HEX1:
            if (isalpha (nextChar) || isdigit (nextChar)) {
               localHexString.push_back(nextChar);
               state = eS_HEX2;
            }
            else {
               delete pAT;
               pAT = new TInvalid;
            }//Invalid entry after 0x
              break; //End eS_HEX1
         case eS_HEX2:
            if (isalpha (nextChar) || isdigit (nextChar)) {
               localHexString.push_back(nextChar);
            }
            else {
               //The line below changes the string to an int and converts the Hexadecimal to a decimal.
               localIntValue = (int) strtol(localHexString.c_str(), NULL, 16);
               inBuffer.backUpInput();
               delete pAT;
               pAT = new THexadecimal2 (localIntValue);
               state = eS_STOP;
            }//Valid Hex entry
              break; //End eS_HEX2
         case eS_ADD:
            if (isalpha (nextChar)) {
               localDotCmdValue.push_back (nextChar);
               state = eS_DOT2;
            }
            else {
               delete pAT;
               pAT = new TInvalid;
            }
              break;//End of Dot Command 1
         case eS_DOT2:
            if (isalpha (nextChar) || isdigit (nextChar)) {
               localDotCmdValue.push_back (nextChar);
            }
            else {
               inBuffer.backUpInput ();
               delete pAT;
               pAT = new TDotCommand (localDotCmdValue);
               state = eS_STOP;
            }
              break;//End of Dot Command 2
         case eS_ADDRESS1:
            if (isalpha (nextChar)) {
               localAddressValue.push_back (nextChar);
               state = eS_ADDRESS2;
            }
            else {
               delete pAT;
               pAT = new TInvalid;
            }
              break;//End of Adress Mode 1
         case eS_ADDRESS2:
            if (isalpha (nextChar) || isdigit (nextChar)) {
               localAddressValue.push_back (nextChar);
            }
            else {
               inBuffer.backUpInput ();
               delete pAT;
               pAT = new TAddress (localAddressValue);
               state = eS_STOP;
            }
              break;//End of Address Mode 2
      }
   }
   while ((state != eS_STOP) && (pAT->tokenType () != eT_INVALID));
}

int main () {
   AToken* pAToken = new TEmpty;
   inBuffer.getLine ();
   while (!cin.eof ()) {
      do {
         getToken (pAToken);
         pAToken->printToken ();
      }
      while ((pAToken->tokenType () != eT_EMPTY)
          && (pAToken->tokenType () != eT_INVALID));
      inBuffer.getLine ();
   }
   delete pAToken;
   return 0;
}
