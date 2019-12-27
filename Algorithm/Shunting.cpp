#pragma once
#ifndef WILDCAT_ALGORITHM_SHUNTING_CPP
#define WILDCAT_ALGORITHM_SHUNTING_CPP

/* Wildcat: Algorithm/Shunting.cpp
   #include <Algorithm/Shunting.cpp>
   
   By Ryan Babij (github.com/RyanBabij)
   
   Implementation of Edsger Dijkstra's shunting-yard algorithm.
   This algorithm converts infix mathematical notation into
   postfix notation, which can be processed in linear order, and
   therefore is much easier to process algorithmically. However
   postfix notation requires a delimiter to identify multiple
   adjacent values.
   
   May have some bugs because I am not good at math.
   I'm not sure how negative numbers are handled in this system,
   but basic testing seems to be giving usable results.
   
   Custom operators can be added, however left and right parentheses
   are special cases which are always processed. Custom operators
   only have a single non-digit char as a symbol, therefore system
   operators like ABS() will need to be replaced with a single char
   before processing.
   
   Sample input:
   
   3+4*2/(1-5)^2^3      -> 2 3 * 3 4 * + 
   (2*3+3*4)            -> 2 3 * 3 4 * +
   20-30/3+4*2^3        -> 20 30 3 / - 4 2 3 ^ * +
   (-1+1+(-1*2))*(-2*1) -> 1 - 1 + 1 2 * - + 2 1 * - *
   

   Credit to Takayuki MATSUOKA's implementation at
   https://gist.github.com/t-mat/b9f681b7591cdae712f6 and
   https://ideone.com/DYX5CW
   
   Todo:
   * Figure out how negative numbers work.
   * Test custom operator support, for example ABS(-10).
   * Create evaluation function.

*/

//#define SHUNTING_ENABLE_OUTPUT //cout result

// can store either a value or an operator. Operators must be a single
// non-digit char. virtual function operate() defines functionality
class Shunting_Token
{
   public:
   
   unsigned char symbol; // if set to 0, then token is a value.
   bool rightAssociative;
   unsigned short int precedence; // higher = higher precedence
   long int value; // only used for non-operator token
   
   Shunting_Token(int _value, unsigned char _symbol = 0, unsigned short int _precedence = 0, bool _rightAssociative = false)
   {
      value = _value;
      symbol = _symbol;
      precedence = _precedence;
      rightAssociative = _rightAssociative;
   }
   virtual long int operate(long int lv, long int rv) // custom operators can be defined here, for example an ABS function.
   {
      return 0;
   }
   
   bool isToken()
   {
      return symbol != 0;
   }
   
   std::string toString()
   {
      if (symbol!=0)
      {
         return std::string(1,symbol);
      }
      return DataTools::toString(value);
   }
};

//     DEFAULT OPERATOR SET

class Shunting_Token_Add: public Shunting_Token
{
   public:
   
   Shunting_Token_Add(): Shunting_Token(0, '+', 2, false)
   {}
   
   long int operate(long int lv, long int rv)
   { return lv+rv; }
};

class Shunting_Token_Subtract: public Shunting_Token
{
   public:
   
   Shunting_Token_Subtract(): Shunting_Token(0, '-', 2, false)
   {}
   
   long int operate(long int lv, long int rv)
   { return lv-rv; }
};

class Shunting_Token_Multiply: public Shunting_Token
{
   public:
   
   Shunting_Token_Multiply(): Shunting_Token(0, '*', 3, false)
   {}
   
   long int operate(long int lv, long int rv)
   { return lv*rv; }
};

class Shunting_Token_Divide: public Shunting_Token
{
   public:
   
   Shunting_Token_Divide(): Shunting_Token(0, '/', 3, false)
   {}
   
   long int operate(long int lv, long int rv)
   { return lv/rv; }
};

class Shunting_Token_Power: public Shunting_Token
{
   public:
   
   Shunting_Token_Power(): Shunting_Token(0, '^', 4, true)
   {}
   
   long int operate(long int lv, long int rv)
   { return pow(lv,rv); }
};

class Shunting_Token_LeftParen: public Shunting_Token
{
   public:
   
   Shunting_Token_LeftParen(): Shunting_Token(0, '(', 0, false)
   {}
   
   long int operate(long int lv, long int rv)
   { return 0; }
};

class Shunting_Token_RightParen: public Shunting_Token
{
   public:
   
   Shunting_Token_RightParen(): Shunting_Token(0, ')', 0, false)
   {}
   
   long int operate(long int lv, long int rv)
   { return 0; }
};

   // example custom operator
   // in this case only an rvalue is processed,
   // a dummy lvalue should be provided like so: 0 A -10
class Shunting_Token_Absolute: public Shunting_Token
{
   public:
   
   Shunting_Token_Absolute(): Shunting_Token(0, 'A', 7, false)
   {}
   
   long int operate(long int lv, long int rv)
   { return abs(lv); }
};

// Main driver class. Initialise and create operators, or call
// buildDefaults() to use default set. Call shunt() and pass
// infix string. It will return vector of tokens in postfix.
// No whitespace, only pass digits and operators.
class Shunting
{
   public:

   Vector <Shunting_Token> vTokenList; // reference list of all tokens
   
   std::deque<Shunting_Token> outputQueue2; // output queue

   std::vector<Shunting_Token> stack2; // operator stack
   
   
   // Class will build the default operator set unless asked
   Shunting(bool bDefaults = true)
   {
      if (bDefaults)
      {
         buildDefaults();
      }
   }
   ~Shunting()
   { }
   
   // Add operator to reference list.
   // Don't add duplicate symbols.
   void addOperator2(Shunting_Token _toke)
   {
      // check for duplicates
      for (int i=0;i<vTokenList.size();++i)
      {
         if ( vTokenList(i).symbol == _toke.symbol )
         {
            // duplicate symbol, do not add it.
            return;
         }
      }
      
      vTokenList.push(_toke);
   }
   
   Shunting_Token * getToken(unsigned char _symbol)
   {
      for (int i=0;i<vTokenList.size();++i)
      {
         if ( vTokenList(i).symbol == _symbol )
         {
            return &vTokenList(i);
         }
      }
      return 0;
   }
   
   bool isOperator(unsigned char _op)
   {
      for (int i=0;i<vTokenList.size();++i)
      {
         if (vTokenList(i).symbol == _op)
         {
            return true;
         }
      }
      return false;
   }
   
   
   std::deque <Shunting_Token> shunt(std::string expression)
   {
      #ifdef SHUNTING_ENABLE_OUTPUT
         std::cout<<"Input: "<<expression<<"\n";
      #endif

      outputQueue2.clear();
      
      std::string _strNumber = "";
      std::string _strOperator = "";
      
      for (unsigned int i=0;i<expression.size();++i)
      {
         if ( std::isdigit(expression[i]) )
         {
            _strNumber+=expression[i];
         }
         else if ( isOperator(expression[i]))
         {
            if ( _strNumber.size() > 0 )
            {
               // push a value token to the output queue
               outputQueue2.push_back(Shunting_Token(DataTools::toInt(_strNumber)));
               
               _strNumber="";
            }
            
            Shunting_Token * currentToken = getToken(expression[i]);
            if ( currentToken == 0 )
            {
               //something bad happened, return an empty output vector.
               outputQueue2.clear();
               return outputQueue2;
            }

            //push left paren onto stack
            if ( expression[i] == '(' )
            {
               stack2.push_back(*currentToken);
            }
            else if (expression[i] == ')')
            {
                bool match = false;

                // Until the token at the top of the stack
                // is a left parenthesis,
                while(! stack2.empty() && stack2.back().symbol != '(')
                {
                    // pop operators off the stack
                    // onto the output queue.
                    outputQueue2.push_back(stack2.back());
                    stack2.pop_back();
                    
                }
                if (!stack2.empty() && stack2.back().symbol == '(')
                {
                  match = true;
                }

                // Pop the left parenthesis from the stack,
                // but not onto the output queue.
                stack2.pop_back();

                if(!match && stack2.empty())
                {
                  // too many right parentheses. Return empty output vector.
                    outputQueue2.clear();
                    return outputQueue2;
                }
            }
            else
            {
               // push operator
               // If the token is operator, o1, then:

               // while there is an operator token,
               while(!stack2.empty())
               {
                  // o2, at the top of stack, and
                  const Shunting_Token o2 = stack2.back();

                  // either o1 is left-associative and its precedence is
                  // *less than or equal* to that of o2,
                  // or o1 if right associative, and has precedence
                  // *less than* that of o2,
                  if ( (! currentToken->rightAssociative && currentToken->precedence <= o2.precedence)
                  ||   (  currentToken->rightAssociative && currentToken->precedence <  o2.precedence) )
                  {
                     // then pop o2 off the stack,
                     stack2.pop_back();
                     // onto the output queue;
                     outputQueue2.push_back(o2);
                     continue;
                  }
                  //otherwise exit
                  break;
               }
               //push current operator onto stack
               stack2.push_back(*currentToken);
            }
         }
         else
         {
            // invalid expression, return empty output vector
            outputQueue2.clear();
            return outputQueue2;
         }
         
      }
      
      //final case: Push any remaining number
      if (_strNumber.size() > 0)
      {
         outputQueue2.push_back(Shunting_Token(DataTools::toInt(_strNumber)));
         _strNumber="";
      }
      
      //final case: No more tokens to read, so push the remaining stack
      while(! stack2.empty())
      {
         // If the operator token on the top of the stack is a parenthesis,
         // then there are mismatched parentheses.
         if(stack2.back().symbol == '(')
         {
            // too many left parentheses, return empty output vector.
            outputQueue2.clear();
            return outputQueue2;
         }

         // Pop the operator onto the output queue.
         outputQueue2.push_back(stack2.back());
         stack2.pop_back();
      }

      
      #ifdef SHUNTING_ENABLE_OUTPUT
         std::cout<<"Output: ";
         for (unsigned int i=0;i<outputQueue2.size();++i)
         {
            std::cout<<outputQueue2.at(i).toString()<<" ";
         } std::cout<<"\n";
      #endif
      
      return outputQueue2;
   }
   
   // Todo: Add evaluation (maybe should be separate class).
   long int evaluate()
   {
      return 0;
   }
   
   // add the standard set of non-function operators: +-*/^()
   // this runs by default in constructor
   void buildDefaults()
   {
      addOperator2(Shunting_Token_Add());
      addOperator2(Shunting_Token_Subtract());
      addOperator2(Shunting_Token_Multiply());
      addOperator2(Shunting_Token_Divide());
      addOperator2(Shunting_Token_Power());
      addOperator2(Shunting_Token_LeftParen());
      addOperator2(Shunting_Token_RightParen());
   }
   
   // runs a bunch of test-cases to demonstrate it works.
   // see documentation for expected output.
   void test()
   {
      buildDefaults();
      
      Vector <std::string> vTestCase = { "3+4*2/(1-5)^2^3", "(2*3+3*4)", "20-30/3+4*2^3", "(-1+1+(-1*2))*(-2*1)" };

      for (int i=0;i<vTestCase.size();++i)
      {
         std::deque <Shunting_Token> vOutput = shunt(vTestCase(i));
         
         std::cout<<"Test input: "<<vTestCase(i)<<"\n";
         
         std::cout<<"Output: ";
         for (unsigned int i2=0;i2<vOutput.size();++i2)
         {
            std::cout<<vOutput.at(i2).toString()<<" ";
            
         } std::cout<<"\n";
         
      }
   }
  
};

#endif