#pragma once
#ifndef WILDCAT_GAME_EASI_CPP
#define WILDCAT_GAME_EASI_CPP

/* Terminal: EASI.cpp
   #include <Game/EASI/EASI.cpp>
   
   Implementation of the EASI language interpreter.
   
*/

#include <Game/EASI/EASI.hpp>

#include <queue> // shunting algorithm

EASI::EASI()
{
   currentLine=0;
   terminated=false;
   isWaitingInput=0;
   input = "";
   vLine=0;
   
   vInput.clear();
   vInputVar.clear();
}

#define EASI_LOAD_VERBOSE

std::string EASI::load(std::string _code)
{
   //EASI CODE MUST ALWAYS BE IN  U P P E R C A S E
   _code = DataTools::toUpper(_code);
   
	#ifdef EASI_LOAD_VERBOSE
   std::cout<<"EASI::load(~)\n\n";
	std::cout<<"Raw input:\n\n";
	
	std::string _strFormatted = "\t" + _code;
	DataTools::findAndReplace(&_strFormatted,"\n","\n\t");
	
   std::cout<<_strFormatted<<"\n***\n\n";
	#endif
   
   terminated=false;
   
   if ( vLine != 0 )
   {
      vLine->clear();
      delete vLine;
      vLine=0;
   }

   for (int i=0;i<vCodeLine.size();++i)
   {
      delete vCodeLine(i);
      vCodeLine(i)=0;
   } vCodeLine.clear();
   
   // Tokenise by newline
   vLine = Tokenize::tokenize(_code,"\n\r");
   
   if ( vLine==0 )
   {
      currentLine=0;
      return "";
   }
	
	std::cout<<"Stripped input:\n\n";

   // Load up each line.
   for (int i=0;i<vLine->size();++i)
   {
		if ( (*vLine)(i) != "\n" && (*vLine)(i) != "\r" )
		{
			vCodeLine.push(new CodeLine((*vLine)(i)));
			std::cout<<"\t"<<(*vLine)(i)<<"\n";
		}
		else
		{
			std::cout<<"REJECT\n";
		}
   }
   
   // Debug output of each CodeLine
   #define EASI_OUTPUT_CODE
   #ifdef EASI_OUTPUT_CODE
   for (int i=0;i<vCodeLine.size();++i)
   {
      std::cout<<"\nLine "<<i<<": "<<vCodeLine(i)->strLine<<" -> "<<vCodeLine(i)->strLineStripped<<"\n";
      std::cout<<"           Label:  "<<vCodeLine(i)->label<<"\n";
      std::cout<<"       Linelabel:  "<<vCodeLine(i)->lineLabel<<"\n";
      std::cout<<"         Keyword:  "<<vCodeLine(i)->keyword<<"\n";
      std::cout<<"  Assignment var:  "<<vCodeLine(i)->assignmentVar<<"\n";
      std::cout<<"   Array indices:  ";
      for (int i2=0;i2<vCodeLine(i)->vAssignmentIndices.size();++i2)
      {
         std::cout<<vCodeLine(i)->vAssignmentIndices(i2)<<",";
      }std::cout<<"\n";
      std::cout<<"      Expression:  "<<vCodeLine(i)->expression<<"\n";
      std::cout<<"             Arg:  "<<vCodeLine(i)->arg<<"\n";
      std::cout<<"      Arg vector:  ";
      for (int i2=0;i2<vCodeLine(i)->vArg.size();++i2)
      {
         std::cout<<vCodeLine(i)->vArg(i2)<<",";
      }std::cout<<"\n";
      
      std::cout<<"Expression tokens: ";
      for (int i2=0; i2<vCodeLine(i)->vExpressionToken.size(); ++i2)
      {
         std::cout<<vCodeLine(i)->vExpressionToken(i2)<<" ";
      } std::cout<<"\n";
      
   }
   #endif
   return "";
}

std::string EASI::cycle()
{
   if (terminated) { return ""; }
   // Waiting for input, continue waiting
   if (isWaitingInput > 0) { return ""; }
   
   // Inputs have been recieved. Process and clear.
   if (isWaitingInput == 0 && vInput.size() > 0 )
   {
      // load each input into corresponding variable
      for(int i=0;i<vInput.size();++i)
      {
         if ( vCodeLine.isSafe(currentLine-1) && vCodeLine(currentLine-1)->vExpressionToken.isSafe(i+1) )
         {
            varTable.set(vCodeLine(currentLine-1)->vExpressionToken(i+1),vInput(i));
         }
         else
         {
            std::cout<<"Error loading var table.\n";
         }
         

      }

      std::string rString = "Inputs recieved:\n";
      for(int i=0;i<vInput.size();++i)
      {
         rString+=vInput(i)+"\n";
      }
      vInput.clear();
      return rString;
   }
   
   // Run the current line of code
   if (vCodeLine.isSafe(currentLine)==false)
   {
      std::cout<<"EASI: End of program\n";
      terminated=true;
      currentLine=0;
      return "END OF PROGRAM\n";
   }
   
   int oldLine = currentLine;
   
   std::string strEval = evaluate (vCodeLine(currentLine));
   
   if ( currentLine == oldLine )
   { // only increment linenumber if it hasn't been modified by the code
      ++currentLine;
   }

   return strEval;
}

std::string EASI::evaluate(CodeLine* _codeLine)
{   
   if ( _codeLine == 0 )
   {
      return "ERROR: Null CodeLine ptr\n";
   }

   std::cout<<"Evaluating line: "<<_codeLine->strLineStripped<<"\n";

   // 1: Sub all variables into expressions, replace uninitialized vars with 0 or null string.
   // 2: Evaluate expression, assign if necessary.
   //    There's no need to worry about combining strings and expressions except with PRINT,
   //    as mixing of var types is forbidden.
   //    todo: Account for positive/negative prefixes, which can be found either at the
   //    start of an expression, or following other operators.
   // 3: Evaluate and execute keywords.

   // sub all variables using token vector
   //  should be moved into a function
   
   // only sub existing vars
   // note that BASIC assumes any non-system keyword is a var,
   // so maybe we will change this later.
   Vector <std::string> vSubbedToken = _codeLine->vExpressionToken;
   for (int i=0;i<vSubbedToken.size();++i)
   {
		std::cout<<"Checking sub: "<<vSubbedToken(i)<<"\n";
      if ( varTable.hasVar(vSubbedToken(i)) )
      {
         if ( varTable.isRealVar(vSubbedToken(i)) ) // get variable as-is
         {
            vSubbedToken(i) = varTable.get(vSubbedToken(i));
         }
         else if ( varTable.isStringVar(vSubbedToken(i)) ) // add quotes to sub so we know it's a string
         {
            vSubbedToken(i) = "\"" + varTable.get(vSubbedToken(i)) + "\"";
         }
      }
   }
   
   // Assignment expression
   // Checking for "FOR" is not good. In future we should insert "LET"
   // into raw expressions.
   if ( _codeLine->assignmentVar != "" && _codeLine->keyword != "FOR")
   {
      if ( _codeLine->vAssignmentIndices.size() > 0 )
      {
         // this is an array assignment
         // each dim must be evaluated
         Vector <unsigned short int> vArrayIndex;
         
         // evaluate each index and push result to index vector
         for (int i=0;i<_codeLine->vAssignmentIndices.size();++i)
         {
            shunt.shunt(_codeLine->vAssignmentIndices(i));
            long int index = shunt.evaluate();
            if ( index > 256 )
            {
               std::cout<<"ARRAY OUT OF BOUNDS\n";
               return "";
            }
            vArrayIndex.push(index);
         }
         
         varTable.set(_codeLine->assignmentVar,vArrayIndex,"TEST");
         
         return "";
         
         std::string strEvalExpression = "";
         for (int i=1;i<vSubbedToken.size();++i)
         {
            if (vSubbedToken(i)==",")
            {
               //evaluate index and keep going
               shunt.shunt(strEvalExpression);
               strEvalExpression="";
               long int index = shunt.evaluate();
               if ( index > 256 )
               {
                  std::cout<<"ARRAY OUT OF BOUNDS\n";
                  return "";
               }
               vArrayIndex.push(index);
               continue;
            }
            if ( vSubbedToken(i) == "=" )
            {
            }
            strEvalExpression += vSubbedToken(i);
         }
         
         //evaluate final index
         shunt.shunt(strEvalExpression);
         strEvalExpression="";
         long int index = shunt.evaluate();
         if ( index > 256 )
         {
            std::cout<<"ARRAY OUT OF BOUNDS\n";
            return "";
         }
         vArrayIndex.push(index);
         
         // assign array value
         std::cout<<"Assigning array var\n";
         varTable.set(_codeLine->assignmentVar,"1");
         
         
         return "";
      }
      else
      {
      }
      
      // rebuild subbed expression into string
      std::string strEvalExpression = "";
      for (int i=0;i<vSubbedToken.size();++i)
      {
         strEvalExpression += vSubbedToken(i);
      }
      
      // real variable evaluation
      if ( varTable.isRealVar(_codeLine->assignmentVar) )
      {
         shunt.shunt(strEvalExpression);
         std::string strResult = DataTools::toString(shunt.evaluate());
         varTable.set(_codeLine->assignmentVar,DataTools::toString(strResult));
      }
      // string variable evaluation
      else if (varTable.isStringVar(_codeLine->assignmentVar) )
      {
         // assigning string vars is simpler because there's only the + operator
         bool isString = false;
         std::string strVarAssignment = "";
         
         for (unsigned int i=0;i<strEvalExpression.size();++i)
         {
            if ( strEvalExpression[i] == '"' )
            {
               isString = !isString;
               continue;
            }
            
            if ( isString )
            {
               strVarAssignment += strEvalExpression[i];
            }
         }
         
         varTable.set(_codeLine->assignmentVar,strVarAssignment);
      }

   //std::cout<<varTable.toString();
   }
   // IF <expression> THEN <linenumber>
   // or:
   // IF <expression> THEN <command>
   else if ( _codeLine->keyword == "IF" )
   {
      // evaluate everything between IF and THEN.
      std::cout<<"EVALUATING IF\n";
      
      // sub in vars
      // sub all variables using token vector
      // vSubbedToken.clear();
      // vSubbedToken = _codeLine->vExpressionToken;
      // for (int i=0;i<vSubbedToken.size();++i)
      // {
         // if ( varTable.isRealVar(vSubbedToken(i)) ) // get variable as-is
         // {
            // vSubbedToken(i) = varTable.get(vSubbedToken(i));
         // }
         // else if ( varTable.isStringVar(vSubbedToken(i)) ) // add quotes to sub so we know it's a string
         // {
            // vSubbedToken(i) = "\"" + varTable.get(vSubbedToken(i)) + "\"";
         // }
      // }
      // rebuild subbed expression into string
      std::string strEvalExpression = "";
      for (int i=0;i<vSubbedToken.size();++i)
      {
         strEvalExpression += vSubbedToken(i);
      }
      
      shunt.shunt(strEvalExpression);
      std::string strResult = DataTools::toString(shunt.evaluate());
      //varTable.set(_codeLine->assignmentVar,DataTools::toString(strResult));
      
      std::cout<<"RESULT: "<<strResult<<"\n";
      
      // -1 is true, execute arg.
      if (strResult == "-1")
      {
         std::cout<<"TRUE, EXECUTING ARG: "<<_codeLine->arg<<"\n";
         // if arg is a number, jump to that label
         
         if (DataTools::isNumeric(_codeLine->arg))
         {
            std::cout<<"JUMP TO LINE : "<<_codeLine->arg<<"\n";
            jumpToLabel(_codeLine->arg);
         }
         
         // if arg is not a number, execute the line like a normal code line.
      }
      else
      {
         std::cout<<"FALSE, CONTINUING\n";
      }

      // 0 is false, continue next line.
      
   }
   else if ( _codeLine->keyword == "DIM" )
   {
      // token 2 must be a var.
      // token 3 must be an opening paren
      // remaining tokens are array dimension expressions, comma delimited
      // final token is closing paren
      
      std::string strEvalExpression = "";
      for (int i=0;i<vSubbedToken.size();++i)
      {
         strEvalExpression += vSubbedToken(i);
      }
      //std::cout<<"EVAL: "<<strEvalExpression<<"\n";
      
      if ( vSubbedToken.size()>2)
      {
         if ( vSubbedToken(1) == "(" )
         {
            if (varTable.isValid(vSubbedToken(0)))
            {
            }
            else
            {
               std::cout<<"Error: Invalid var name.\n";
               return "";
            }
            //std::cout<<"Valid DIM\n";
            
            // build sub-expressions and push results to array dim vector
            Vector <unsigned short int> vArrayDim;
            std::string currentExpression = "";
            for (int i=2;i<vSubbedToken.size();++i)
            {
               if ( vSubbedToken(i) == "," )
               {
                  // evaluate expr and continue
                  if (currentExpression.size() == 0 )
                  { // something went wrong
                     std::cout<<"Error building array\n";
                     return "";
                  }
                  
                  //std::cout<<"evaluating sub expressoin: "<<currentExpression<<"\n";
                  Shunting sh;
                  sh.shunt(currentExpression);
                  vArrayDim.push(sh.evaluate());
                  currentExpression="";
                  
                  
               }
               else if (vSubbedToken(i) == ")")
               {
                  // evaluate final expressoin and finished.
                  // evaluate expr and continue
                  if (currentExpression.size() == 0 )
                  { // something went wrong
                     std::cout<<"Error building array\n";
                     return "";
                  }
                  
                  //std::cout<<"evaluating sub expressoin: "<<currentExpression<<"\n";
                  Shunting sh;
                  sh.shunt(currentExpression);
                  vArrayDim.push(sh.evaluate());
                  currentExpression="";
                  
                  varTable.addArray(vSubbedToken(0),vArrayDim);
                  
                  //std::cout<<"finished building array\n";
                  
                  //std::cout<<"Vartable:\n"<<varTable.toString()<<"\n";
               }
               else
               {
                  currentExpression+=vSubbedToken(i);
               }
            }
            // std::cout<<"DIMs are: ";
            // for (int i=0;i<vArrayDim.size();++i)
            // { std::cout<<vArrayDim(i)<<", ";
            // } std::cout<<"\n";
            
            
            
            return "";
         }
         else
         {
            std::cout<<"INVALID DIM\n";
            return "";
         }
      }
      else
      {
         std::cout<<"INVALID DIM\n";
         return "";
      }
   }
   else if ( _codeLine->keyword == "GOTO" )
   {
      jumpToLabel(_codeLine->expression);
   }
   else if ( _codeLine->keyword == "INPUT" )
   {
      //Execution freezes until user presses enter.
      // INPUT can contain string output, followed by ;.
      // Multiple vars can be listed, separated by commas.
      // Example: INPUT "PLEASE ENTER 2 NUMBERS"; A
      // program will repeat input if a string is entered into a number var
      // numbers can be entered as strings.
      isWaitingInput=_codeLine->vExpressionToken.size()-1;
      if ( isWaitingInput < 1)
      {
         std::cout<<"Error with INPUT command\n";
         isWaitingInput=0;
      }
      else
      {
         std::cout<<"Waiting for input\n";
      }
      
      return _codeLine->vExpressionToken(0);
   }
   else if ( _codeLine->keyword == "NEXT" )
   {
      //scan up until we find a FOR keyword.
      forCycle();
      return "";
   }
   else if ( _codeLine->keyword == "PRINT" )
   {
      // PRINT can return strings, evaluate an expression, or a combination of both.
      // Strings can be string var or text enclosed in quotes.
      // adjacent strings are automatically appended, even without a + operator
      // Any operations between strings and non-strings are invalid.
      // PRINT will also evaluate normal expressions.
      
      // rebuild subbed expression into string
      std::string strEvalExpression = "";
      
      std::string expressionToEvaluate="";
      
      //bool lastWasString = false;
      bool appendingString = false;
      
      for (int i=0;i<vSubbedToken.size();++i)
      {
         // replace all instances of "\\n" with '\n'.
         size_t index = 0;
         while (true) //https://stackoverflow.com/questions/4643512/replace-substring-with-another-substring-c
         {
              /* Locate the substring to replace. */
              index = vSubbedToken(i).find("\\N", index);
              if (index == std::string::npos) break;

              /* Make the replacement. */
              vSubbedToken(i).replace(index, 2, "\n");

              /* Advance index forward so the next iteration doesn't pick it up as well. */
              index += 2;
         }
         
         //number variable
         if (DataTools::isNumeric(vSubbedToken(i)))
         {
            //if last was string, then invalid operation.
            if (appendingString) //invalid
            {
               return "ERROR\n";
            }
            else
            {
               //strEvalExpression+=vSubbedToken(i);
               expressionToEvaluate+=vSubbedToken(i);
            }
         }
         else if (vSubbedToken(i).size()>1 && vSubbedToken(i)[0] == '"' && vSubbedToken(i).back()=='"')
         { // is a string
            //this is a string, only appending is possible.
            
            
            //EVALUATE EXPRESSION
            if (expressionToEvaluate.size() > 0)
            {
               //EVALUATE
               shunt.shunt(expressionToEvaluate);
               std::string strResult = DataTools::toString(shunt.evaluate());
               strEvalExpression+=strResult;
               expressionToEvaluate="";
            }
            
            // remove first and last char
            strEvalExpression+=vSubbedToken(i).substr(1, vSubbedToken(i).size() - 2);
            
            //Strings can only be followed by + and another string, another string, or a number.
            //lastWasString=true;
            appendingString=false;
         }
         else // operator or invalid.
         {
            expressionToEvaluate+=vSubbedToken(i);
         }
      }
      
      //FINAL CASE EVALUATE EXPRESSION
      if (expressionToEvaluate.size() > 0)
      {
            //EVALUATE
            shunt.shunt(expressionToEvaluate);
            std::string strResult = DataTools::toString(shunt.evaluate());
            strEvalExpression+=strResult;
            expressionToEvaluate="";
      }
      return strEvalExpression;
   }
   else if ( _codeLine->keyword == "END" )
   { // Get off Mr Bones' Wild Ride
      terminated=true;
      currentLine=0;
   }
   
   
   return "";

   
   

   std::string _str = "";
   std::string _strSubExpression = "";
   
   Vector <std::string> _vString;
   Vector <std::string> _vSubExpression;
   
   bool isString=false;
   // we need to sub all variables here to prevent confusion with string variables in expressions.
   // variables are alphabetical, so we just scan for any alphabetical chars.
   // in future we must make sure we aren't reading in keywords or functions
   // BASIC and EASI set unknown variables to 0, this includes throwing /0 errors.
   std::string _strSubbedCode = "";
   std::string _strVar = "";
   
         // need to account for assignment expressions here.
         // assignment can have a single lvalue, but multiple rvalues.
   std::string _strAssignment = ""; // Variable to assign if required.
   //bool isLvalue=true; // only true for first element.
   
   std::string _strExpression = _codeLine->expression;
   
   
   // step 1: determine if this is an assignment expression.
   // This can only happen if there is no keyword, or the keyword is LET
   
   Vector <std::string> expressionTokens; // vector of tokens in expression
   


   
   
   //Step 0: figure out if this is an assignment expression, for example:
   // A = B or
   // VAR = 123+456*2
   // Assignment expressions may only use a single =.
   // Invalid assignments still run in BASIC. Assignment appears to become a comparator,
   // with true returning -1 (11111111 11111111) TRUE
   // and false returning 0 (0000000 00000000) FALSE
   // TRUE appears to be a special case which converts to value -1. (10000000 00000001).
   // however this is unlikely to appear in normal code.

   
   // Step 1: Sub in all variable values (except strings and assignment variable).
   for (unsigned int i=0;i<_strExpression.size();++i)
   {
      if ( _strExpression[i] == '\"' ) // flip between expression and string
      {
         isString = !isString;
         _strSubbedCode += '\"';
         continue;
      }
         
      if ( isString ) // ignore
      {
         _strSubbedCode += _strExpression[i];
      }
      else // scan for variables
      {
         // check is alpha
         //check is dollarsign
         
         if ( DataTools::isAlpha(_strExpression[i]) )
         {
            _strVar+=_strExpression[i];
         }
         else if (_strExpression[i] == '$')
         {
            //STRING VAR
            // pull from vartable
            //std::cout<<"Stringvar\n";
            _strVar+='$';
            _strSubbedCode += varTable.get(_strVar);
            _strVar="";
         }
         else
         {
            // variable terminates.
            if (_strVar != "")
            {
               // pull from vartable.
               _strSubbedCode += varTable.get(_strVar);
               _strVar="";
            }
            _strSubbedCode += _strExpression[i];
         }

      }

   }
   //end case: final var
   if ( _strVar.size() > 0 )
   {
      _strSubbedCode += varTable.get(_strVar);
   }
   
   //create sub expression and string vector.
   
   isString=false;
   for (unsigned int i=0;i<_strExpression.size();++i)
   {
      if ( _strExpression[i] == '\"' ) // flip between expression and string
      {
         isString = !isString;
         
         if ( isString ) // push the expression
         {
            _vSubExpression.push(_strSubExpression);
            _strSubExpression="";
         }
         else // push the string
         {
            _vString.push(_str);
            _str="";
         }
      }
      else if (isString)
      {
         _str += _strExpression[i];
      }
      else
      {
         _strSubExpression += _strExpression[i];
      }
   }
   _vSubExpression.push(_strSubExpression);
   
   std::cout<<"Sub expressions\n";
   for (int i=0;i<_vSubExpression.size();++i)
   {
      std::cout<<_vSubExpression(i)<<"\n";
      //convert each sub expression with shunting
      
      std::stack <std::string> stkValue;
      std::stack <std::string> stkOperator;
      
      std::queue <std::string> qVal;
      
      
   }
   
   std::cout<<"Sub strings\n";
   for (int i=0;i<_vString.size();++i)
   {
      std::cout<<_vString(i)<<"\n";
   }

   // EVALUATE SUBEXPRESSIONS

   // std::cout<<"Sub expressions\n";
   // for (int i=0;i<_vSubExpression.size();++i)
   // {
      // std::cout<<_vSubExpression(i)<<"\n";
      // std::string shunted = shunt(_vSubExpression(i));
      // std::cout<<"Shunted to: "<<shunted<<"\n";
   // }
   

   return _strExpression+"\n"+_strSubbedCode+"\n";

}


void EASI::jumpToLabel(std::string _label)
{
   for (int i=0;i<vCodeLine.size();++i)
   {
      if ( vCodeLine(i)->label == _label || vCodeLine(i)->lineLabel == _label )
      {
         currentLine = i;
      }
   }
}

void EASI::forCycle()
{ // move up until we find a FOR loop. Set the current line to that for loop
   // we should also increment the variable
   // Also we need the vars from the NEXT line

   // save the NEXT line in case we need to exit the loop.
   long int nextLine = currentLine;
   
   if ( vCodeLine(currentLine)->hasKeyword("NEXT") )
   {
      // get all the vars to STEP
      Vector <std::string> vStep;
      for (int i=1;i<vCodeLine(currentLine)->vArg.size();++i)
      {
         vStep.push(vCodeLine(currentLine)->vArg(i));
      }
      
      while (currentLine >= 0 )
      {
         if ( vCodeLine(currentLine)->vArg.size() > 3 && vCodeLine(currentLine)->vArg(0) == "FOR" )
         { // Found matching FOR
            // Get target var name
            std::string targetVarName = vCodeLine(currentLine)->vArg(1);
            // Push the target var to the vector if it isn't already.
            vStep.pushUnique(targetVarName);
            
            long int targetVarValue = DataTools::toInt(vCodeLine(currentLine)->vArg(3));

            // step the variables.
            long int stepAmount = DataTools::toInt(vCodeLine(currentLine)->vArg(4));

            for (int i=0;i<vStep.size();++i)
            {
               long int var = DataTools::toInt(varTable.get(vStep(i)));
               var+=stepAmount;
               varTable.set(vStep(i),DataTools::toString(var));
            }
            // check if the target var has incremented to or past its target.
            long int var = DataTools::toInt(varTable.get(targetVarName));
            if ( var > targetVarValue ) // Note that loops use inclusive ranges.
            {
               currentLine=nextLine;
               return;
            }

            return;
         }
         --currentLine;
      }
      if (currentLine<0)
      {
         std::cout<<"Error: Couldn't find matching FOR for NEXT\n";
      }
      return;
   }
   else
   {
      std::cout<<"ERROR: No NEXT here.\n";
      std::cout<<"Line: "<<vCodeLine(currentLine)->strLine<<"\n";
      return;
   }
   

}

#include <list> 
#include <iterator> 

//#include <Algorithm/Shunting.cpp>

// std::string EASI::shunt(std::string _input)
// {
   // //Shunting shunt;
   // shunt.shunt(_input);
   
   // std::cout<<"EASI: shunted: "<<shunt.toString()<<".\n";
   // return shunt.toString();

// }

#endif
