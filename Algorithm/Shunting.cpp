#pragma once
#ifndef WILDCAT_ALGORITHM_SHUNTING_CPP
#define WILDCAT_ALGORITHM_SHUNTING_CPP

/* Wildcat: Algorithm/Shunting.cpp
   #include "Algorithm/Shunting.cpp"
   
   Implementation of Edsger Dijkstra's shunting-yard algorithm.
   
*/

class Shunting_Token
{
   public:
   
   unsigned char symbol; // if set to 0, then token is a value.
   bool rightAssociative;
   unsigned short int precedence;
   long int value;
   
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

// Custom operators can be defined using this interface.
// Special cases are right and left parenthesis.
class Shunting_Operator
{
   public:
   char symbol;
   bool rightAssociative;
   int precedence;
   
   Shunting_Operator(char _symbol, int _precedence, bool _rightAssociative = false)
   {
      symbol=_symbol;
      rightAssociative = _rightAssociative;
      precedence = _precedence;
      
   }
   
   // define custom operation here.
   virtual int operation (int lhs, int rhs)
   {
      return 0;
   }
   
   
   // for vector contains() call
   // void operator == (const Shunting_Operator &rhs )
   // { 
      // return symbol == rhs.symbol;
   // }
      
};

class Shunting
{
   public:
   
   // define operators here
   Vector <Shunting_Operator> vOperator;
   
   Vector <Shunting_Token> vToken;
   Vector <Shunting_Token> vTokenList;
   
   
   std::deque<std::string> outputQueue;
   std::deque<Shunting_Token> outputQueue2;
   
   std::vector<Shunting_Operator> stack;
   std::vector<Shunting_Token> stack2;
   
   
   Shunting()
   {
   }
   ~Shunting()
   {
   }
   
   // Only pure expressions should be passed here. No variables, strings or whitespace.
   // Functions will need to be custom, probably replaced by a char.
   // Expressions to test:
   // (1+2+3)
   // (1+2)/2*2
   // 3+4*2/(1-5)^2^3
   // 20-30/3+4*2^3
   
   void addOperator(Shunting_Operator _op)
   {
      vOperator.push(_op);
   }
   void addOperator2(Shunting_Token _toke)
   {
      vTokenList.push(_toke);
   }
   
   Shunting_Operator* getOperator(unsigned char _symbol)
   {
      //Shunting_Operator* retOperator = 0;
      for (int i=0;i<vOperator.size();++i)
      {
         if ( vOperator(i).symbol == _symbol )
         {
            return &vOperator(i);
         }
      }
      return 0;
   }
   
   Shunting_Token * getToken(unsigned char _symbol)
   {
      //Shunting_Operator* retOperator = 0;
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
      
      // for (int i=0;i<vOperator.size();++i)
      // {
         // if (vOperator(i).symbol == _op)
         // {
            // return true;
         // }
      // }
      return false;
   }
   
   
   std::deque <Shunting_Token> shunt(std::string expression)
   {
      std::cout<<"Pre-shunt: "<<expression<<".\n";
      
      outputQueue.clear();
      outputQueue2.clear();
      
      std::string _strNumber = "";
      std::string _strOperator = "";
      
      for (unsigned int i=0;i<expression.size();++i)
      {
         if ( std::isdigit(expression[i]) )
         {
            std::cout<<"digit\n";
            _strNumber+=expression[i];
         }
         else if ( isOperator(expression[i]))
         {
            std::cout<<"operator\n";
            if ( _strNumber.size() > 0 )
            {
               // push number to the output queue
               outputQueue.push_back(_strNumber);
               
               //2: push a value token to the output queue
               //Shunting_Token st (DataTools::toInt(_strNumber));
               outputQueue2.push_back(Shunting_Token(DataTools::toInt(_strNumber)));
               
               _strNumber="";
            }
            
            Shunting_Token * st = getToken(expression[i]);
            
            if ( st == 0 )
            {
               std::cout<<"ERROR: Invalid symbol.\n";
               
                  //return "";
            }
            std::cout<<"Current token symbol: "<<st->symbol<<".\n";
            
            
            //2: process basic operators
            // push operator
            // If the token is operator, o1, then:
            //const auto o1 = token;

            // while there is an operator token,
            while(!stack2.empty())
            {
               // o2, at the top of stack, and
               const Shunting_Token o2 = stack2.back();

               // // either o1 is left-associative and its precedence is
               // // *less than or equal* to that of o2,
               // // or o1 if right associative, and has precedence
               // // *less than* that of o2,
               if ( (! st->rightAssociative && st->precedence <= o2.precedence)
               ||   (  st->rightAssociative && st->precedence <  o2.precedence) )
               {
                  // // // then pop o2 off the stack,
                  stack2.pop_back();
                  // // // onto the output queue;
                  // //std::string strQ (1,o2.symbol);
                  // //std::string strQ = 'a';
                  //outputQueue2.push_back(std::string(1,o2.symbol));
                  outputQueue2.push_back(o2);

                  continue;
               }
               //otherwise exit
               break;
            }
            //push current operator onto stack
            stack2.push_back(*st);
            
            
            
            

            Shunting_Operator * currentOperator = getOperator(expression[i]);
            if ( currentOperator == 0 )
            {
               //something bad happened
               std::cout<<"ERROR\n";
                  //return "";
            }
            std::cout<<"Current operator symbol: "<<currentOperator->symbol<<".\n";
            
            //push left paren onto stack
            if ( expression[i] == '(' )
            {
               std::cout<<"Pushing left paren\n";
               //Shunting_Operator sh ('(',5,false);
               stack.push_back(*currentOperator);
            }
            else if (expression[i] == ')')
            {
               std::cout<<"Right paren\n";
               
                bool match = false;

                // Until the token at the top of the stack
                // is a left parenthesis,
                while(! stack.empty() && stack.back().symbol != '(')
                {
                    // pop operators off the stack
                    // onto the output queue.
                    outputQueue.push_back(std::string(1,stack.back().symbol));
                    stack.pop_back();
                    
                }
                if (!stack.empty() && stack.back().symbol == '(')
                {
                  match = true;
                }

                // Pop the left parenthesis from the stack,
                // but not onto the output queue.
                std::cout<<"Popping: "<<(unsigned char)stack.back().symbol<<".\n";
                stack.pop_back();

                if(!match && stack.empty())
                {
                    // If the stack runs out without finding a left parenthesis,
                    // then there are mismatched parentheses.
                    std::cout<<"ERROR: Too many right paren.\n";
                     //return "";
                }
               
            }
            else
            {
               
               
               // push operator
               // If the token is operator, o1, then:
               //const auto o1 = token;

               // while there is an operator token,
               while(!stack.empty())
               {
                  // o2, at the top of stack, and
                  const Shunting_Operator o2 = stack.back();

                  // either o1 is left-associative and its precedence is
                  // *less than or equal* to that of o2,
                  // or o1 if right associative, and has precedence
                  // *less than* that of o2,
                  if ( (! currentOperator->rightAssociative && currentOperator->precedence <= o2.precedence)
                  ||   (  currentOperator->rightAssociative && currentOperator->precedence <  o2.precedence) )
                  {
                     // // then pop o2 off the stack,
                     stack.pop_back();
                     // // onto the output queue;
                     //std::string strQ (1,o2.symbol);
                     //std::string strQ = 'a';
                     outputQueue.push_back(std::string(1,o2.symbol));

                     continue;
                  }
                  //otherwise exit
                  break;
               }
               //push current operator onto stack
               stack.push_back(*currentOperator);
               
               
            }
         }
         else
         {
            // invalid expression
         }
         
      }
      
      //final case: Push any remaining number
      if (_strNumber.size() > 0)
      {
         outputQueue.push_back(_strNumber);
         outputQueue2.push_back(Shunting_Token(DataTools::toInt(_strNumber)));
         _strNumber="";
      }
      
         //final case: No more tokens to read, so push the remaining stack
    // When there are no more tokens to read:
    //   While there are still operator tokens in the stack:
      while(! stack.empty())
      {
         // If the operator token on the top of the stack is a parenthesis,
         // then there are mismatched parentheses.
         if(stack.back().symbol == '(')
         {
            std::cout<<"ERROR: Parenthesis mismatch.\n";
               //return "";
         }

         // Pop the operator onto the output queue.
         //outputQueue.push_back(stack.back().symbol);
         outputQueue.push_back(std::string(1,stack.back().symbol));
         outputQueue2.push_back(stack2.back());
         
         stack2.pop_back();
         stack.pop_back();
      }

      
      
      std::cout<<"Post-shunt: ";
      
      for(unsigned int i=0;i<outputQueue.size();++i)
      {
         std::cout<<outputQueue.at(i);
      } std::cout<<"\n";
      
      std::cout<<"New shunt: ";
      
      for (unsigned int i=0;i<outputQueue2.size();++i)
      {
         std::cout<<outputQueue2.at(i).toString();
      } std::cout<<"\n";
      
      return outputQueue2;
   }
   
   void buildDefaults()
   {
      addOperator(Shunting_Operator('^',4,true));
      addOperator(Shunting_Operator('*',3,false));
      addOperator(Shunting_Operator('/',3,false));
      addOperator(Shunting_Operator('+',2,false));
      addOperator(Shunting_Operator('-',2,false));
      addOperator(Shunting_Operator('(',0,false));
      addOperator(Shunting_Operator(')',0,false));
      
      
      addOperator2(Shunting_Token_Add());
      addOperator2(Shunting_Token_Subtract());
      addOperator2(Shunting_Token_Multiply());
      addOperator2(Shunting_Token_Divide());
      addOperator2(Shunting_Token_Power());
      addOperator2(Shunting_Token_LeftParen());
      addOperator2(Shunting_Token_RightParen());
   }
  
};

#endif