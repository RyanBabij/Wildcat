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
   
   Custom operators can be added, however left and right parentheses
   are special cases which are always processed. Custom operators
   only have a single non-digit char as a symbol, therefore system
   operators like ABS() will need to be replaced with a single char
   before processing.
   
   Sample input:
   
   3+4*2/(1-5)^2^3      -> 2 3 * 3 4 * + 
   (2*3+3*4)            -> 2 3 * 3 4 * +
   20-30/3+4*2^3        -> 20 30 3 / - 4 2 3 ^ * +
   (-1+1+(-1*2))*(-2*1) -> -1 1 + -1 2 * + -2 1 * *
   

   Credit to Takayuki MATSUOKA's implementation at
   https://gist.github.com/t-mat/b9f681b7591cdae712f6 and
   https://ideone.com/DYX5CW
   
   Todo:
   * Add multi-char operators
   * Add function support such as ABS, SIN, LOG, etc.

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
   
   Shunting_Token(int _value=0, unsigned char _symbol = 0, unsigned short int _precedence = 0, bool _rightAssociative = false)
   {
      value = _value;
      symbol = _symbol;
      precedence = _precedence;
      rightAssociative = _rightAssociative;
   }
   
   virtual ~Shunting_Token()
   { }
   
   virtual Shunting_Token * operate(Shunting_Token * lv, Shunting_Token * rv) // custom operators can be defined here, for example an ABS function.
   {
      //Shunting_Token * ret = new Shunting_Token;
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
   
   // add operator overloads here
};

//     DEFAULT OPERATOR SET

class Shunting_Token_Add: public Shunting_Token
{
   public:
   
   Shunting_Token_Add(): Shunting_Token(0, '+', 2, false)
   { }
   
   Shunting_Token* operate(Shunting_Token * lv, Shunting_Token * rv) override // custom operators can be defined here, for example an ABS function.
   {
      if (lv==0 || rv==0)
      {
         std::cout<<"Error: Add recieved null pointer.\n";
         return 0;
      }

      lv->value = lv->value + rv->value;
      return lv;
   }
};

class Shunting_Token_Subtract: public Shunting_Token
{
   public:
   
   Shunting_Token_Subtract(): Shunting_Token(0, '-', 2, false)
   {}
   
   Shunting_Token* operate(Shunting_Token * lv, Shunting_Token * rv) override // custom operators can be defined here, for example an ABS function.
   {
      if (lv==0 || rv==0)
      {
         std::cout<<"Error: Subtract recieved null pointer.\n";
         return 0;
      }

      lv->value = lv->value - rv->value;
      return lv;
   }
};

class Shunting_Token_Multiply: public Shunting_Token
{
   public:
   
   Shunting_Token_Multiply(): Shunting_Token(0, '*', 3, false)
   {}
   
   Shunting_Token* operate(Shunting_Token * lv, Shunting_Token * rv) override // custom operators can be defined here, for example an ABS function.
   {
      if (lv==0 || rv==0)
      {
         std::cout<<"Error: Multiply recieved null pointer.\n";
         return 0;
      }

      lv->value = lv->value * rv->value;
      return lv;
   }
};

class Shunting_Token_Divide: public Shunting_Token
{
   public:
   
   Shunting_Token_Divide(): Shunting_Token(0, '/', 3, false)
   {}
   
   Shunting_Token* operate(Shunting_Token * lv, Shunting_Token * rv) override // custom operators can be defined here, for example an ABS function.
   {
      if (lv==0 || rv==0)
      {
         std::cout<<"Error: Multiply recieved null pointer.\n";
         return 0;
      }
      else if (rv->value==0)
      {
         std::cout<<"ERROR: Divide by 0.\n";
         return 0;
      }

      lv->value = lv->value / rv->value;
      return lv;
   }
};

class Shunting_Token_Power: public Shunting_Token
{
   public:
   
   Shunting_Token_Power(): Shunting_Token(0, '^', 4, true)
   {}
   
   Shunting_Token* operate(Shunting_Token * lv, Shunting_Token * rv) override // custom operators can be defined here, for example an ABS function.
   {
      if (lv==0 || rv==0)
      {
         std::cout<<"Error: Multiply recieved null pointer.\n";
         return 0;
      }
      lv->value = pow(lv->value, rv->value);
      return lv;
   }
};

class Shunting_Token_LeftParen: public Shunting_Token
{
   public:
   
   Shunting_Token_LeftParen(): Shunting_Token(0, '(', 0, false)
   {}
   
   // long int operate(long int lv, long int rv)
   // { return 0; }
};

class Shunting_Token_RightParen: public Shunting_Token
{
   public:
   
   Shunting_Token_RightParen(): Shunting_Token(0, ')', 0, false)
   {}
   
   // long int operate(long int lv, long int rv)
   // { return 0; }
};

class Shunting_Token_Equals: public Shunting_Token
{
   public:
   
   Shunting_Token_Equals(): Shunting_Token(0, '=', 5, false)
   {}
   
   Shunting_Token* operate(Shunting_Token * lv, Shunting_Token * rv) override
   {
      if (lv==0 || rv==0)
      {
         std::cout<<"Null ptr error\n";
         return 0;
      }
      
      if ( lv->value == rv->value )
      {
         lv->value = -1; // true
      }
      else
      {
         lv->value = 0; // false
      }
      return lv;
   }
};

class Shunting_Token_LessThan: public Shunting_Token
{
   public:
   
   Shunting_Token_LessThan(): Shunting_Token(0, '<', 5, false)
   {}
   
   Shunting_Token* operate(Shunting_Token * lv, Shunting_Token * rv) override
   {
      if (lv==0 || rv==0)
      {
         std::cout<<"Null ptr error\n";
         return 0;
      }
      
      if ( lv->value < rv->value )
      {
         lv->value = -1; // true
      }
      else
      {
         lv->value = 0; // false
      }
      return lv;
   }
};

class Shunting_Token_GreaterThan: public Shunting_Token
{
   public:
   
   Shunting_Token_GreaterThan(): Shunting_Token(0, '>', 5, false)
   {}
   
   Shunting_Token* operate(Shunting_Token * lv, Shunting_Token * rv) override
   {
      if (lv==0 || rv==0)
      {
         std::cout<<"Null ptr error\n";
         return 0;
      }
      
      if ( lv->value > rv->value )
      {
         lv->value = -1; // true
      }
      else
      {
         lv->value = 0; // false
      }
      return lv;
   }
};

   // example custom operator
   // in this case only an rvalue is processed,
   // a dummy lvalue should be provided like so: 0 A -10
class Shunting_Token_Absolute: public Shunting_Token
{
   public:
   
   Shunting_Token_Absolute(): Shunting_Token(0, 'A', 7, false)
   {}
   
   // long int operate(long int lv, long int rv)
   // { return abs(lv); }
};

// Main driver class. Initialise and create operators, or call
// buildDefaults() to use default set. Call shunt() and pass
// infix string. It will return vector of tokens in postfix.
// No whitespace, only pass digits and operators.
class Shunting
{
   public:

   Vector <Shunting_Token*> vTokenList; // reference list of all tokens
   
   std::deque<Shunting_Token*> outputQueue2; // output queue

   std::vector<Shunting_Token*> stack2; // operator stack
   
   
   // Class will build the default operator set unless asked
   Shunting(bool bDefaults = true)
   {
      if (bDefaults)
      {
         buildDefaults();
      }
   }
   ~Shunting()
   {
      clearMem();
   }
   
   void clearMem()
   {
      for (unsigned int i=0;i<outputQueue2.size();++i)
      {
         // delete all values from output queue
         if (outputQueue2.at(i)->symbol==0)
         {
            delete outputQueue2.at(i);
         }
      }
      // delete all operators
      vTokenList.deleteAll();
   }
   
   // Add operator to reference list.
   // Don't add duplicate symbols.
   void addOperator2(Shunting_Token * _toke)
   {
      if ( _toke == 0 )
      { return; }
      
      // check for duplicates
      for (int i=0;i<vTokenList.size();++i)
      {
         if ( vTokenList(i)->symbol == _toke->symbol )
         {
            // duplicate symbol, do not add it.
            delete _toke;
            return;
         }
      }
      
      vTokenList.push(_toke);
   }
   
   Shunting_Token * getToken(unsigned char _symbol)
   {
      for (int i=0;i<vTokenList.size();++i)
      {
         if ( vTokenList(i)->symbol == _symbol )
         {
            return vTokenList(i);
         }
      }
      return 0;
   }
   
   bool isOperator(unsigned char _op)
   {
      for (int i=0;i<vTokenList.size();++i)
      {
         if (vTokenList(i)->symbol == _op)
         {
            return true;
         }
      }
      return false;
   }
   
   
   void shunt(std::string expression)
   {
      #ifdef SHUNTING_ENABLE_OUTPUT
         std::cout<<"Input: "<<expression<<"\n";
      #endif
      
      if (expression.size() == 0 )
      {
         //do nothing
         return;
      }

      for (unsigned int i=0;i<outputQueue2.size();++i)
      {
         // delete all values from output queue
         if (outputQueue2.at(i)->symbol==0)
         {
            delete outputQueue2.at(i);
         }
      }
      outputQueue2.clear();
      
      stack2.clear();
      
      std::string _strNumber = "";
      std::string _strOperator = "";
      
      // Handling +- signs makes life more difficult than it needs to be.
      // BASIC allows combination of many sign operators, for example A=----1 is valid.
      // For some reason basic allows -(1+1) but not 5(1+1).
      // First we combine +- combinations into a single sign. For example --- becomes -.
      // Then we add a 0 in front of an expression that starts with - as a special case.
      // Then we scan for any negative numbers and mark them for pushing onto the token vector,
      // and we remove + signs which aren't addition.
      
      // In BASIC, A=--------1 is technically a valid expression.

      // combine multiple signs eg --1 becomes +1.
      int nSigns = 0;
      int currentSign = 1;
      std::string strCombinedSigns = "";
      for (unsigned int i=0;i<expression.size();++i)
      {
         if ( expression[i] == '-' )
         {
            ++nSigns;
            currentSign = currentSign * -1;
         }
         else if (expression[i] == '+')
         {
            ++nSigns;
         }
         else
         {
            if (nSigns>0)
            {
               if ( currentSign==1 )
               {
                  strCombinedSigns+='+';
               }
               else
               {
                  strCombinedSigns+='-';
               }
               // reset counter
               nSigns=0;
               currentSign=1;
            }
            strCombinedSigns+=expression[i];
         }
      }
      expression=strCombinedSigns;
      
      // insert a 0 at the start of a - expression
      if (expression[0] == '-')
      {
         expression.insert(expression.begin(),'0');
      }
      
      // remove + prefixes, and convert - prefixes into negative values.
      // convert -() into -1*()
      std::string fixedStr = "";
      fixedStr+=expression[0];
      for (unsigned int i=1;i<expression.size()-1;++i)
      {
         if ( std::isdigit(expression[i-1]) == false )
         {
            if ( expression[i] == '+' && expression[i-1] != ')' && expression[i-1] != '(' )
            {
               // we can remove this positive sign, as it is not addition
               continue;
            }
            else if (expression[i] == '-')
            {
               if ( expression[i+1] == '(')
               {
                  // replace negative brackets with M1*()
                  fixedStr+="M1*";
                  continue;
               }
               else
               {
                  // this is a negative value instead of a minus.
                  // Put a M to signal for parsing as a negative value
                  fixedStr+='M';
                  continue;
               }
            }
         }
         fixedStr+=expression[i];
      } // final case
      fixedStr+=expression[expression.size()-1];
      
      expression = fixedStr;

      for (unsigned int i=0;i<expression.size();++i)
      {
         if ( std::isdigit(expression[i]) )
         {
            _strNumber+=expression[i];
         }
         else if (expression[i] == 'M')
         {
            // parse M as a negative value.
            _strNumber+='-';
         }
         else if ( isOperator(expression[i]))
         {
            if ( _strNumber.size() > 0 )
            {
               // push a value token to the output queue
               outputQueue2.push_back(new Shunting_Token(DataTools::toInt(_strNumber)));
               _strNumber="";
            }
            
            Shunting_Token * currentToken = getToken(expression[i]);
            if ( currentToken == 0 )
            {
               //something bad happened, return an empty output vector.
               outputQueue2.clear();
            }

            //push left paren onto stack
            if ( expression[i] == '(' )
            {
               stack2.push_back(currentToken);
            }
            else if (expression[i] == ')')
            {
                bool match = false;

                // Until the token at the top of the stack
                // is a left parenthesis,
                while(! stack2.empty() && stack2.back()->symbol != '(')
                {
                    // pop operators off the stack
                    // onto the output queue.
                    outputQueue2.push_back(stack2.back());
                    stack2.pop_back();
                    
                }
                if (!stack2.empty() && stack2.back()->symbol == '(')
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
                    //return outputQueue2;
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
                  Shunting_Token* o2 = stack2.back();

                  // either o1 is left-associative and its precedence is
                  // *less than or equal* to that of o2,
                  // or o1 if right associative, and has precedence
                  // *less than* that of o2,
                  if ( (! currentToken->rightAssociative && currentToken->precedence <= o2->precedence)
                  ||   (  currentToken->rightAssociative && currentToken->precedence <  o2->precedence) )
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
               stack2.push_back(currentToken);
            }
         }
         else
         {
            // invalid expression, return empty output vector
            outputQueue2.clear();
         }
         
      }
      
      //final case: Push any remaining number
      if (_strNumber.size() > 0)
      {
         outputQueue2.push_back(new Shunting_Token(DataTools::toInt(_strNumber)));
         _strNumber="";
      }
      
      //final case: No more tokens to read, so push the remaining stack
      while(! stack2.empty())
      {
         // If the operator token on the top of the stack is a parenthesis,
         // then there are mismatched parentheses.
         if(stack2.back()->symbol == '(')
         {
            // too many left parentheses, return empty output vector.
            outputQueue2.clear();
            //return outputQueue2;
         }

         // Pop the operator onto the output queue.
         outputQueue2.push_back(stack2.back());
         stack2.pop_back();
      }

      
      #ifdef SHUNTING_ENABLE_OUTPUT
         std::cout<<"Output: ";
         for (unsigned int i=0;i<outputQueue2.size();++i)
         {
            std::cout<<outputQueue2.at(i)->toString()<<" ";
         } std::cout<<"\n";
      #endif
      
   }
   
   std::string toString()
   {
      std::string retStr = "";
      for (unsigned int i2=0;i2<outputQueue2.size();++i2)
      {
         retStr += outputQueue2.at(i2)->toString();
         retStr += " ";
      }
      return retStr;
   }
   
   // Evaluation function should be included otherwise we will need to pass
   // all operator tokens anyways.
   long int evaluate()
   {
      std::stack <Shunting_Token*> stack; // stack to store values from left to right.
      
      for (unsigned int i=0;i<outputQueue2.size();++i)
      {
         if ( outputQueue2.at(i)->symbol == 0 )
         {
            //push value to stack
            stack.push(outputQueue2.at(i));
         }
         else
         {
            //evaluate this operator, and last 2 values in stack.
            if (stack.size() > 1)
            {
               Shunting_Token* oper = outputQueue2.at(i);
               Shunting_Token* opRight = stack.top();
               stack.pop();
               Shunting_Token* opLeft = stack.top();
               stack.pop();

               Shunting_Token* result = oper->operate(opLeft,opRight);
               if ( result==0 )
               {
                  std::cout<<"Error: operation returned null ptr\n";
                  return 0;
               }
               stack.push(result);
            }
            else
            {
               std::cout<<"SYNTAX ERROR\n";
               return 0;
            }
            
         }
      }
      if (stack.size() == 1)
      {
         return stack.top()->value;
      }
      std::cout<<"SYNTAX ERROR 2\n";
      return 0;
   }
   
   std::string shuntEvaluate(std::string _input)
   {
      //automatically shunt and evaluate the string, and return string output.
      return "";
   }
   
   // add the standard set of non-function operators: +-*/^()
   // this runs by default in constructor
   void buildDefaults()
   {
      addOperator2(new Shunting_Token_Add());
      addOperator2(new Shunting_Token_Subtract());
      addOperator2(new Shunting_Token_Multiply());
      addOperator2(new Shunting_Token_Divide());
      addOperator2(new Shunting_Token_Power());
      addOperator2(new Shunting_Token_LeftParen());
      addOperator2(new Shunting_Token_RightParen());
      addOperator2(new Shunting_Token_Equals());
      addOperator2(new Shunting_Token_LessThan());
      addOperator2(new Shunting_Token_GreaterThan());
   }
   
   // runs a bunch of test-cases to demonstrate it works.
   // see documentation for expected output.
   // void test()
   // {
      // buildDefaults();
      
      // Vector <std::string> vTestCase = { "1/0", "1+1", "1+1+1", "2*2*2", "2-1", "12-2*5", "3+4*2/(1-5)^2^3", "(2*3+3*4)", "20-30/3+4*2^3", "(-1+1+(-1*2))*(-2*1)", "2^2^2", "1+1+2^2-1*2", "66" };

      // for (int i=0;i<vTestCase.size();++i)
      // {
         // //std::deque <Shunting_Token*> vOutput = shunt(vTestCase(i));
         // shunt(vTestCase(i));
         
         // std::cout<<"Test input: "<<vTestCase(i)<<"\n";
         
         // std::cout<<"Output: ";
         // for (unsigned int i2=0;i2<vOutput.size();++i2)
         // {
            // std::cout<<vOutput.at(i2)->toString()<<" ";
            
         // } std::cout<<"\n";
         
         // evaluate();
         
      // }
   // }
  
};

#endif