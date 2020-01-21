#pragma once
#ifndef WILDCAT_GAME_EASI_HPP
#define WILDCAT_GAME_EASI_HPP

/* Wildcat: EASI.hpp
   #include <Game/EASI/EASI.hpp>
   
   The EASI language interpreter.
   
   EASI is a basic interpreted programming language designed to be similar to BASIC. The ultimate
   goal is for EASI to run any BASIC program, but BASIC may not run any EASI program, as EASI will
   have expanded functionality. The purpose of EASI is for use in programming and hacking games, but
   it could potentially also be used for scripting.
   
   Todo: Shift from named strings to predictable token Vector.
   
*/

#include <stack> // for shunting-yard algorithm

// Simple table to lookup variable names and values
// VarTable is held by EASI.
class VarTable
{
   private:
   class VarTableArray
   {
      public:
      std::string name; // Name of the array
      Vector <unsigned short int> vDim; // list of dimensions
      Vector <int> vValue; // vector of array values, initialised to 0.
      
      VarTableArray()
      {
         name="";
      }
      void init(std::string _name, Vector <unsigned short int> _vDim)
      {
         if (_vDim.size()==0)
         {
            std::cout<<"Error: Constructing null array dims\n";
            return;
         }
         
         name=_name;
         vDim=_vDim;
         
         unsigned long int totalIndex=vDim(0);
         for (int i=1;i<vDim.size();++i)
         {
            totalIndex*=vDim(i);
         }
         for (unsigned int i=0;i<totalIndex;++i)
         {
            vValue.push(0);
         }
      }
      void set(Vector<unsigned short int> vAddress, std::string _value)
      {
         if (vAddress.size() != vDim.size())
         {
            std::cout<<"Incorrect array index\n";
            return;
         }
         for (int i=0;i<vDim.size();++i)
         {
            if (vAddress(i) >= vDim(i))
            {
               std::cout<<"Error: array index out of bounds\n";
               return;
            }
         }
         // we can safely set this value.
         // Set the value using the following algorithm:
         //    [ index(1)*( S2* S3* S4 * ... * Sn) + index(2)*( S3* S4* S5 * ... * Sn)
         //    + ... + index(n-1)*Sn + index(n) ] 
         unsigned long int index=0;
         for (int i=0;i<vDim.size()-1;++i)
         {
            long int totalMult = vAddress(i);
            for (int i2=i+1;i2<vDim.size();++i2)
            {
               totalMult*=vDim(i2);
            }
            index+=totalMult;
         }
         index+=vAddress(vAddress.size()-1);
         vValue(index)=1;
      }
      std::string toString()
      {
         std::string strRet = "";
         for (int i=0;i<vValue.size();++i)
         {
            strRet+=DataTools::toString(vValue(i))+",";
         }
         return strRet;
      }
   };
   
   public:
   Vector <std::string> vVarName;
   Vector <std::string> vVarValue;
   
   Vector <VarTableArray*> vArray;
   
   VarTable()
   {
   }
   
   void addArray(std::string strArrayName, Vector <unsigned short int> vDim)
   {
      //std::cout<<"Vartable: Building array: "<<strArrayName<<"\n";
      VarTableArray* vta = new VarTableArray;
      vta->init(strArrayName,vDim);
      vArray.push(vta);
   }
   
   // return true if var is a valid numeric or string variable name
   bool isValid(std::string _varName)
   {
      if (_varName.size() == 0 )
      { return false; }
      
      for (unsigned int i=0;i<_varName.size()-1;++i)
      {
         if ( _varName[i] < 65 || _varName[i] > 90 )
         {
            return false;
         }
      }
      
      if ( _varName.back() == '$'
        || (_varName.back() > 64 && _varName.back() < 91) )
      { return true; }
 
      
      return false;
   }
   
   // real vars must be uppercase alpha characters only
   bool isRealVar ( std::string _varName )
   {
      if ( _varName.size() == 0 )
      { return false; }
      
      for (unsigned int i=0;i<_varName.size();++i)
      {
         if ( _varName[i] < 65 || _varName[i] > 90 )
         {
            return false;
         }
      }
      
      return true;
   }

   // string vars must be uppercase alpha and end with a $
   bool isStringVar (std::string _varName)
   {
      if (_varName.size() == 0 )
      { return false; }
      
      for (unsigned int i=0;i<_varName.size()-1;++i)
      {
         if ( _varName[i] < 65 || _varName[i] > 90 )
         {
            return false;
         }
      }
      
      if ( _varName.back() != '$' )
      { return false; }
 
      
      return true;
   }
   
   
   // Automatically adds the variable or updates it as required.
   void set(std::string _varName, std::string _varValue)
   {
      //std::cout<<"Set "<<_varName<<" -> "<<_varValue<<".\n";
      
      for (int i=0;i<vVarName.size();++i)
      {
         if ( vVarName(i) == _varName )
         {
            if ( DataTools::isNumeric(_varValue)==false && isRealVar(_varName) )
            {
               // convert invalid values to 0.
               vVarValue(i) = "0";
            }
            else
            { vVarValue(i) = _varValue;
            }
            
            return;
         }
      }
      vVarName.push(_varName);
      vVarValue.push(_varValue);
   }
   // Same but for array
   // If array doesn't exist, it will be automatically initialised with 11 indexes on each dimension.
   void set(std::string _varName, Vector <unsigned short int> vDim, std::string _varValue)
   {
      //std::cout<<"Set array "<<_varName<<" -> "<<_varValue<<".\n";
      
      for (int i=0;i<vArray.size();++i)
      {
         if ( vArray(i)->name == _varName )
         {
            vArray(i)->set(vDim,_varValue);
            return;
         }
      }
      // initialise here
      std::cout<<"Array not found, initialise here\n";
      // vVarName.push(_varName);
      // vVarValue.push(_varValue);
   }
   
   
   std::string get(std::string _varName)
   {
      for (int i=0;i<vVarName.size();++i)
      {
         if (vVarName(i) == _varName)
         {
            return vVarValue(i);
         }
      }
      // BASIC and EASI assume unknown variables are 0.
      return "0";
   }
   bool hasVar(std::string _varName)
   {
      return vVarName.contains(_varName);
   }
   void clear()
   {
      vVarName.clear();
      vVarValue.clear();
   }
   
   std::string toString()
   {
      std::string strRet = "";
      for (int i=0;i<vVarName.size();++i)
      {
         strRet+=" "+vVarName(i)+": "+vVarValue(i)+"\n";
      }
      
      std::cout<<"Arrays:\n";
      
      for (int i=0;i<vArray.size();++i)
      {
         std::cout<<"  "<<vArray(i)->name<<"(";
         for (int i2=0;i2<vArray(i)->vDim.size();++i2)
         {
            std::cout<<vArray(i)->vDim(i2)<<",";
         }std::cout<<")\n";
         
         std::cout<<"Array content: "<<vArray(i)->toString()<<"\n";
      }
      
      
      return strRet;
   }
};

// CodeLine reads and parses lines of EASI code, but does not execute them.
// Technically I think the C64 executes lines at it reads them.
class CodeLine
{
   public:
   std::string label; // The label of the line if applicable -> should be moved to keyword and vector.
   std::string lineLabel; //Optional line label
   std::string keyword;
   std::string expression;
   std::string arg; // Commands or sub-code to be executed, should probably be a vector.
   // vector of expression tokens: +-*/^ strings, values, variables.
   // useful because some operators change if they are in a different
   // position, for example = can be comparator or assignment.
   Vector <std::string> vExpressionToken;

   // Note: I'm planning to remove all strings except line number
   // and pushing the parsed args to a vector.
   // For example a for loop will be : "FOR",<ASSIGNMENTVAR>,"TO",<EXPR>,"STEP",<VALUE>
   // It should be easy to evaluate because everything will be in a predictable order.
   // We can even strip the keywords, so the for loop will become something like:
   // FOR,<ASSIGNMENTVAR>,<EXPR>,<STEP VALUE>
   // In future we can also push the line number and have it implicit and always on slot 0.
   // The only issue is how to handle sub-commands like:
   // IF <EXPR> THEN PRINT A+B+C
   // But I already have code for that so I should be fine.
   Vector <std::string> vArg;

   std::string strLine; // full code line
   std::string strLineStripped; // above but with non-string whitespace removed.
   std::string errorMessage;
   std::string assignmentVar; // Only if expression is assignment.
   Vector <std::string> vAssignmentIndices; // Vector of array indices for assignment var.
   
   
   // Load and parse line of code.
   CodeLine(std::string _strLine)
   {
      //std::cout<<"Loading line: "<<_strLine<<"\n";
      
      label="";
      lineLabel="";
      keyword="";
      expression="";
      arg="";
      strLine = _strLine;
      errorMessage="";
      assignmentVar="";
      vArg.clear();
      
      // Evaluate the line
      bool isLineLabel = true;
      bool isKeyword = false;
      bool isExpression = false;
      
      // A line that starts with numbers  has a line number label
      // A line that starts with characters is either a keyword or expression
      // (an expression must start with a variable or keyword, and neither can contain numbers)
      // If the line does not have a valid keyword, it will be evaluated as an expression
      // If the expression is invalid, the code line is invalid.
      // These rules mean that EASI and BASIC are whitespace independent,
      // and you can strip all whitespace (except strings) and assume every char is significant
      //_strLine = DataTools::strip(_strLine,"\n\r");
      
      // The main catch is that a colon indicates a new line of code, but this is easy to deal with.
      // Simply replace every colon (not in a string) with a newline
      
      std::string _strNew = "";
      bool isString=false;
      // Strip all spaces which aren't between quotes
      for (unsigned int i=0;i<_strLine.size();++i)
      {
         if ( _strLine[i] == '\"' )
         {
            isString=!isString;
            _strNew+=_strLine[i];
         }
         else if (isString==true || _strLine[i] != ' ' )
         {
            _strNew+=_strLine[i];
         }
      }
      _strLine=_strNew;
      strLineStripped=_strNew;
      
      for (unsigned int i=0;i<_strLine.size();++i)
      {
         if (isLineLabel) // line labels can only be at the beginning of a string.
         {
            if ( DataTools::isAlpha(_strLine[i]))
            {
               // switch to keyword mode.
               isLineLabel=false;
               isKeyword=true;
            }
            else if (DataTools::isNumeric(_strLine[i]))
            {
               lineLabel+=_strLine[i];
            }
            else //invalid char, or maybe expression. Go back to start and try evaluate as expression.
            {
               lineLabel="";
               i=0;
               isLineLabel=false;
               isExpression=true;
            }
         }
         if (isKeyword) // keywords can only be alpha
         {
               if (_strLine.rfind("PRINT",i,5) == i)
               {
                  keyword = "PRINT";
                  i+=5;
               }
               else if (_strLine.rfind("IF",i,2) == i)
               {
                  std::size_t found = _strLine.find("THEN",i+2);
                  
                  // IF is only valid if followed by a THEN
                  // Code between IF and THEN becomes an expression
                  // Code after THEN becomes an arg.
                  // THEN may be followed by a number, in which case EASI
                  // jumps to that line if true, or continues if false.
                  // THEN may instead be followed by a command like PRINT,
                  // which executes if true, and does not execute if false.
                  if ( found != std::string::npos )
                  {
                     std::cout<<" IF CONTAINS A THEN\n";
                     keyword = "IF";
                     i+=2;
                     expression = _strLine.substr(i,found-i);
                     std::cout<<"EXPRESSOIN: "<<expression<<"\n";
                     i=found+4;
                     
                     arg = _strLine.substr(i);
                     
                     break;
                  }
                  else
                  {
                     std::cout<<" IF BUT NO THEN\n";
                  }
                  
                  
                  // expression is everything between IF and THEN.
                  // after THEN can be line number or command.
                  //keyword = "IF";
                  
                  
               }
               else if (_strLine.rfind("FOR",i,3) == i)
               {
                  // FOR loop. Allows automatic incrementing of a variable
                  // FOR <Counter-Variable>=<Startnumber> TO <Endnumber> [ STEP <step-size-number>] NEXT
                  // FOR will use the new vArg system.
                  // Note that mixing int and size_t is bad practise, so we should fix it.
                  
                  keyword = "FOR";
                  vArg.push("FOR");
                  // substring to '=' for assignment var.
                  unsigned short int i2=i+3;
                  for(;i2<_strLine.size()&&_strLine[i2]!='=';++i2)
                  {
                     assignmentVar+=_strLine[i2];
                  } ++i2;
                  vArg.push(assignmentVar);
                  //vArg.push("=");
                  // substring from '=' to TO for assignment expression
                  std::size_t found = _strLine.find("TO",i2);
                  if (found==std::string::npos)
                  {
                     std::cout<<"Syntax error\n";
                     return;
                  }
                  expression = _strLine.substr(i2,found-i2);
                  vArg.push(expression);
                  //vArg.push("TO");
                  i2=found+2;
                  // Read in the next number.
                  std::string strTo="";
                  for(;i2<_strLine.size()&&std::isdigit(_strLine[i2]);++i2)
                  {
                     strTo+=_strLine[i2];
                  }
                  vArg.push(strTo);
                  
                  //STEP (optional) push step amount to arg
                  std::size_t foundStep = _strLine.find("STEP",i2);
                  if (foundStep!=std::string::npos)
                  {
                     //vArg.push("STEP");
                     vArg.push(_strLine.substr(foundStep+4,_strLine.size()-1));
                  }
                  else
                  {
                     // the step implicitly becomes +1
                     vArg.push("1");
                  }
                  i+=3;
                  return;
               }
               // NEXT - Increment the current FOR loop by STEP amount
               // Additional variables can be specified
               else if (_strLine.rfind("NEXT",i,4) == i)
               {
                  keyword = "NEXT";
                  vArg.push("NEXT");
                  i+=4;
                  
                  std::string currentArg = "";
                  for (;i<_strLine.size();++i)
                  {
                     if ( _strLine[i] == ' ' && currentArg.size() > 0 )
                     {
                        // push to arg vector
                        vArg.push(currentArg);
                        currentArg="";
                     }
                     else
                     {
                        currentArg+=_strLine[i];
                     }
                     
                  }
                  if ( currentArg.size() > 0 )
                  {
                     vArg.push(currentArg);
                  }
                  
                  return;
               }
               else if (_strLine.rfind("POKE",i,4) == i)
               {
                  // POKE is 2 expressions separated by commas.
                  std::cout<<"FOUND POKE\n";
                  
                  keyword = "POKE";
                  vArg.push("POKE");
                  i+=4;
                  std::string strAdd = "";
                  for (;i<_strLine.size() && _strLine[i]!=',';++i)
                  {
                     if ( DataTools::isNumeric(_strLine[i]) )
                     {
                        strAdd+=_strLine[i];
                     }
                     else
                     {
                        std::cout<<"ERROR: Invalid poke addr\n";
                        return;
                     }
                  } ++i;
                  vArg.push(strAdd);
                  strAdd="";

                  for (;i<_strLine.size();++i)
                  {
                     if ( DataTools::isNumeric(_strLine[i]) )
                     {
                        strAdd+=_strLine[i];
                     }
                     else
                     {
                        std::cout<<"ERROR: Invalid poke num\n";
                        return;
                     }
                  }
                  vArg.push(strAdd);
                  return;
                  
                  
               }
               // REM - Remark
               // A comment line, should be ignored by EASI.
               else if (_strLine.rfind("REM",i,3) == i)
               {
                  keyword = "REM";
                  i+=3;
                  return;
               }
               else if (_strLine.rfind("LET",i,3) == i)
               {
                  keyword = "LET";
                  i+=3;
               }
               else if (_strLine.rfind("END",i,3) == i)
               {
                  keyword = "END";
                  i+=3;
               }
               else if (_strLine.rfind("LABEL",i,5) == i)
               {
                  keyword = "LABEL";
                  label="";
                  // build the label name into label.
                  for (unsigned int i2=5;i2<_strLine.size();++i2)
                  {
                     label+=_strLine[i2];
                  }
                  i+=5;
               }
               else if (_strLine.rfind("GOTO",i,4) == i)
               {
                  keyword = "GOTO";
                  i+=4;
               }
               else if (_strLine.rfind("DIM",i,3) == i)
               {
                  // DIM allocates an array of up to 255 dimensions.
                  // Example: DIM A(5,5,5)
                  // Indices can be 0 - 32767. (16-pit pointer)
                  
                  //Once an array is dimensioned, it cannot be redimensioned.
                  //Arrays are automatically dimensioned to 10 if referenced,
                  // the number of dimensions is equal to the reference. For example:
                  // Q(5,5,5)=1 will allocate a 3D array with indices 0-10.
                  
                  // Indices may be referenced using expressions, for example:
                  // Q(N*2,I,J) = 100
                  keyword = "DIM";
                  i+=3;
               }
               else if (_strLine.rfind("INPUT",i,5) == i)
               {
                  std::cout<<"Found INPUT\n";
                  keyword = "INPUT";
                  i+=5;
                  
                  // INPUT may contain a string, which must be terminated with a ;.
                  // INPUT may then contain multiple input variables, separated with commas.
                  // The first expression token will be the output string.
                  // Any following tokens will be variables to assign.
                  
                  std::string strOutput = "";
                  
                  if ( _strLine.size() > 5 )
                  {
                     if ( _strLine[5] == '\"' )
                     {
                        std::cout<<"INPUT contains a string\n";
                        bool validOutput = false;
                        //build the output string until we find terminating quote
                        for (i=6;i<_strLine.size();++i)
                        {
                           if ( _strLine[i] == '\"' )
                           {
                              // output string is built.
                              validOutput=true;
                              ++i;
                              break;
                           }
                           else
                           {
                              strOutput+=_strLine[i];
                           }
                        }
                        if ( validOutput==false)
                        {
                           std::cout<<"Invalid output\n";
                           // ERROR
                           strOutput="";
                           vExpressionToken.clear();
                           return;
                        }
                     }
                  }
                  std::cout<<"INPUT string built\n";
                  
                  //check for terminating semicolon
                  if (i < _strLine.size() && _strLine[i] == ';')
                  {
                     std::cout<<"Advancing past semicolon\n";
                     ++i;
                  }
                  
                  // push output string, even if it's a null string.
                  // so EASI can always print index 0.
                  std::cout<<"Pushing output string: "<<strOutput<<"\n";
                  vExpressionToken.push(strOutput);
                  
                  // build all vars, which should be separated by commas.
                  // There must be at least 1 variable, otherwise the line
                  // is invalid.
                  // all chars should be alpha or comma.
                  
                  std::string currentVarName = "";
                  for (;i<_strLine.size();++i)
                  {
                     std::cout<<"current i: "<<i<<"\n";
                     if ( _strLine[i] == ',' )
                     {
                        // add var
                        if ( currentVarName.size() > 0 )
                        {
                           std::cout<<"Pushing var: "<<currentVarName<<"\n";
                           vExpressionToken.push(currentVarName);
                           currentVarName="";
                        }
                     }
                     else if (DataTools::isAlpha(_strLine[i]) || _strLine[i]=='$') // build var
                     {
                        currentVarName+=_strLine[i];
                     }
                     else //invalid code
                     {
                        // ERROR
                        std::cout<<"Error building var list. Fail char was: "<<(int)_strLine[i]<<"\n";
                        vExpressionToken.clear();
                        return;
                     }
                  }
                  // add var
                  if ( currentVarName.size() > 0 )
                  {
                     std::cout<<"Pushing var final: "<<currentVarName<<"\n";
                     vExpressionToken.push(currentVarName);
                     currentVarName="";
                  }

                  return;
               }
               //else
               //{
                  isKeyword=false;
                  isExpression=true;
               //}
         }
         if (isExpression) //expression is evaluated externally
         {
            const std::string allowedInputs = " ,!@#$%^&*()\"\'\\=+-/";
            if (DataTools::isAlphaNumeric(_strLine[i]) || allowedInputs.find(_strLine[i]) != std::string::npos)
            {
               expression+=_strLine[i];
            }
         }
      }
      
      // Build expression token list
      bool isString2=false;
      std::string strCurrent = "";
      std::string strCurrentVar = "";
      vExpressionToken.clear();
      if (expression.size() > 0)
      {
         for (unsigned int i=0;i<expression.size();++i)
         {
            if ( expression[i]=='"' )
            {
               isString2 = !isString2;
               strCurrent+=expression[i];
               
               if (strCurrent.size() > 1)
               {
                  vExpressionToken.push(strCurrent);
                  strCurrent="";
               }
               if (strCurrentVar.size() > 0)
               {
                  vExpressionToken.push(strCurrentVar);
                  strCurrentVar="";
               }
               
               continue;
            }
            
            if (isString2)
            {
               // build string
               strCurrent+=expression[i];
            }
            
            // push operators or comma
            else if ( expression[i] == '+' || expression[i] == '-' || expression[i] == '*'
               || expression[i] == '/' || expression[i] == '>' || expression[i] == '<'
               || expression[i] == '=' || expression[i] == '(' || expression[i] == ')')
            {
               
               if (strCurrent.size() > 0)
               {
                  vExpressionToken.push(strCurrent);
                  strCurrent="";
               }
               if (strCurrentVar.size() > 0)
               {
                  vExpressionToken.push(strCurrentVar);
                  strCurrentVar="";
               }

               vExpressionToken.push(std::string(1,expression[i]));
            }
            else if ( expression[i] == ',' )
            { // push non-operator tokens
         
               if (strCurrent.size() > 0)
               {
                  vExpressionToken.push(strCurrent);
                  strCurrent="";
               }
               if (strCurrentVar.size() > 0)
               {
                  vExpressionToken.push(strCurrentVar);
                  strCurrentVar="";
               }
               vExpressionToken.push(std::string(1,expression[i]));
            }
            else if (!isString2) // build var
            {
               strCurrentVar+=expression[i];
            }
         }
         
         // build any final vars
         if (strCurrent.size() > 0)
         {
            vExpressionToken.push(strCurrent);
            strCurrent="";
         }
         if (strCurrentVar.size() > 0)
         {
            vExpressionToken.push(strCurrentVar);
            strCurrentVar="";
         }
         
      }
      
      //Determine positional operators using expression token vector.
      // Strip assignment code and just keep assigment var.
      
      // Raw assignment expression
      // can be a variable or an array
      // in future it might be a function call, so beware.
      // in some corner cases it might be a standalone expression, although
      // it would be rare or invalid. For example -(RAND(10))
      if ( keyword=="" && vExpressionToken.size() > 1)
      {
         if ( vExpressionToken(1) == "=")
         {
            assignmentVar = vExpressionToken(0);
            vExpressionToken.eraseSlot(0);
            vExpressionToken.eraseSlot(0);
         }
         else if (vExpressionToken(1) == "(")
         {
            // maybe we should include brackets in assignment?
            assignmentVar = vExpressionToken(0);
            vExpressionToken.eraseSlot(0);
            vExpressionToken.eraseSlot(0);
            // build assignmentIndices don't include brackets
            
            std::string currentAssignmentIndex = "";
            
            int bracketCounter=1;
            while(vExpressionToken.size()>0)
            {
               if(vExpressionToken(0) == "(")
               {
                  ++bracketCounter;
                  vExpressionToken.eraseSlot(0);
               }
               else if ( vExpressionToken(0) == ")")
               {
                  --bracketCounter;
                  if (bracketCounter==0)
                  { // closing bracket, stop processing
                     vExpressionToken.eraseSlot(0);
                     break;
                  }
               }
               else if ( vExpressionToken(0) == "," )
               {
                  vExpressionToken.eraseSlot(0);
                  // add expression to vector
                  if ( currentAssignmentIndex.size() > 0 )
                  {
                     vAssignmentIndices.push(currentAssignmentIndex);
                     currentAssignmentIndex="";
                  }
                  else
                  {
                     std::cout<<"ERROR, bad vector\n";
                     vAssignmentIndices.clear();
                     return;
                  }
               }
               else
               {
                  currentAssignmentIndex+=vExpressionToken(0);
                  vExpressionToken.eraseSlot(0);
               }
            }
            
            // add expression to vector
            if ( currentAssignmentIndex.size() > 0 )
            {
               vAssignmentIndices.push(currentAssignmentIndex);
               currentAssignmentIndex="";
            }
            else
            {
               std::cout<<"ERROR, bad vector\n";
               vAssignmentIndices.clear();
               return;
            }
            
         }
      }
      
      // LET assignment expression
      if ( vExpressionToken.size() > 2)
      {
         if ( vExpressionToken(0) == "LET" && vExpressionToken(2) == "=" )
         {
            assignmentVar = vExpressionToken(1);
            vExpressionToken.eraseSlot(0);
            vExpressionToken.eraseSlot(0);
            vExpressionToken.eraseSlot(0);
         }
      }
   }
   
   ~CodeLine()
   { }
   
   bool hasKeyword(const std::string _keyword)
   { // return true if this codeline has the given keyword
      if ( vArg.size() > 0 && ( vArg(0) == _keyword || keyword == _keyword ) )
      {
         return true;
      }
      return false;
   }
   
};

#include <Math/Shunting/Shunting.cpp>

// Maintains VarTable, CodeLines, and executes the lines.
class EASI
{
   public:
   
   Shunting shunt; // Constructs with default operators
   
   long int currentLine;
   
   Vector <CodeLine*> vCodeLine;
   Vector <std::string> * vLine; // String for every line of the program, valid or not.
   
   bool terminated;
   short int isWaitingInput; // 0 = not waiting for input. other values are the number of inputs to get.
   Vector <std::string> vInputVar; // variables to put input into
   Vector <std::string> vInput; // user input from INPUT command. Can hold multiple inputs.
   std::string input; // holds current input.
   
   VarTable varTable;
   
   EASI();
   
   std::string load(std::string _code); // Load code into the CodeLine Vector
   
   std::string cycle(); // Execute one cycle of the code, return any output.
   
   std::string evaluate(CodeLine* _codeLine); // Run code line in current state
   
   void jumpToLabel(std::string _label);
   void forCycle(); // Move currentLine up to a FOR loop
   
   //std::string shunt(std::string input); // convert expression to postfix notation

};


#endif