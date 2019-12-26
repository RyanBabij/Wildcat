#pragma once
#ifndef WILDCAT_ALGORITHM_SHUNTING_CPP
#define WILDCAT_ALGORITHM_SHUNTING_CPP

/* Wildcat: Algorithm/Shunting.cpp
   #include "Algorithm/Shunting.cpp"
   
   Implementation of Edsger Dijkstra's shunting-yard algorithm.
   
*/

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
   
   
   std::deque<std::string> outputQueue;
   std::vector<Shunting_Operator> stack;
   
   
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
   
   Shunting_Operator* getOperator(unsigned char _symbol)
   {
      Shunting_Operator* retOperator = 0;
      for (int i=0;i<vOperator.size();++i)
      {
         if ( vOperator(i).symbol == _symbol )
         {
            return &vOperator(i);
         }
      }
      return 0;
   }
   
   bool isOperator(unsigned char _op)
   {
      for (int i=0;i<vOperator.size();++i)
      {
         if (vOperator(i).symbol == _op)
         {
            return true;
         }
      }
      return false;
   }
   
   
   std::string shunt(std::string expression)
   {
      std::cout<<"Pre-shunt: "<<expression<<".\n";
      
      outputQueue.clear();
      
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
               _strNumber="";
            }
            
            //unsigned char _operator = expression[i];
            
            Shunting_Operator * currentOperator = getOperator(expression[i]);
            if ( currentOperator == 0 )
            {
               //something bad happened
               std::cout<<"ERROR\n";
               return "";
            }
            
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

            // @@ otherwise, exit.
            //break;
         }
         else
         {
            // invalid expression
         }
         
         //final case: Push any remaining number
         if (_strNumber.size() > 0)
         {
            outputQueue.push_back(_strNumber);
            _strNumber="";
         }
         
         

         
      }
      
         //final case: No more tokens to read, so push the remaining stack
    // When there are no more tokens to read:
    //   While there are still operator tokens in the stack:
    while(! stack.empty()) {
        // If the operator token on the top of the stack is a parenthesis,
        // then there are mismatched parentheses.
        // if(stack.back().type == Token::Type::LeftParen) {
            // printf("Mismatched parentheses error\n");
            // return {};
        // }

        // Pop the operator onto the output queue.
        //outputQueue.push_back(stack.back().symbol);
        outputQueue.push_back(std::string(1,stack.back().symbol));
        stack.pop_back();
    }
         
      
      
      std::cout<<"Post-shunt: ";
      
      for(unsigned int i=0;i<outputQueue.size();++i)
      {
         std::cout<<outputQueue.at(i);
      } std::cout<<"\n";
      
      return "";
   }
   
   void buildDefaults()
   {
      addOperator(Shunting_Operator('^',4,true));
      addOperator(Shunting_Operator('*',3,false));
      addOperator(Shunting_Operator('/',3,false));
      addOperator(Shunting_Operator('+',2,false));
      addOperator(Shunting_Operator('-',2,false));
   }
  
};

void Shunting_Build_Defaults(Shunting& _shunting)
{
   
   
   
   
            // case '(':   t = Token::Type::LeftParen;     break;
            // case ')':   t = Token::Type::RightParen;    break;
            // case '^':   t = Token::Type::Operator;      pr = 4; ra = true;  break;
            // case '*':   t = Token::Type::Operator;      pr = 3; break;
            // case '/':   t = Token::Type::Operator;      pr = 3; break;
            // case '+':   t = Token::Type::Operator;      pr = 2; break;
            // case '-':   t = Token::Type::Operator;      pr = 2; break;
   
   
}

#endif