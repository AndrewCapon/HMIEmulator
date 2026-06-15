#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace Tokenizer
{
  class Token 
  {
    public:
      Token(std::string sValue)
      : m_sValue(sValue)
      {

      }

      operator int() 
      { 
        int i = 0;
        try 
        {
          i = std::stoi(m_sValue);
        } 
        catch(std::exception) 
        {
          printf("ERROR : Failed to convert `%s` to int\n", m_sValue.c_str());
        };
        return i;
      }

      operator float()
      {
        float f = 0.0f;
        try 
        {
          f = std::stof(m_sValue);
        } 
        catch(std::exception) 
        {
          printf("ERROR : Failed to convert `%s` to float\n", m_sValue.c_str());
        };
        return f;
      }

      operator std::string()
      {
        return m_sValue;
      }

    private:
      std::string m_sValue;
  };
  
  typedef std::vector<Token>            TokenVector;
  typedef std::vector<Token>::iterator  TokenVectorIter;

  TokenVector tokenise(const std::string str, const char delimiter) 
  { 
    std::vector<Token> vector; 
    std::stringstream ss(str); 
    std::string strToken; 
  
    while(getline(ss, strToken, delimiter)) 
    { 
      // As the token deliminator may be in a string ("") make sure we handle this
      if(strToken[0] == '"')
      {
        std::string strToken2 = strToken;
        while(strToken2.back()!= '"' && getline(ss, strToken2, delimiter))
          strToken += " " + strToken2;
      }
      
      if(strToken[0] !='\n')
        vector.push_back(Token(strToken)); 
    } 
  
    return vector; 
  } 
}