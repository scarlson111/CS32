#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>

class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck() { }
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
    //removes all non-alphabetical, non-apostrophe characters from str
    void stripNonAlpha(std::string& str);
    
    //sets all alphabetical characters in str to lowercase
    void toLowercase(std::string& str);
    
    //input precondition: ch must be a lowercase letter or apostrophe
    //returns index of letter in children array in trieNode struct
    int getIndex(char ch);
    
    //input precondition: index must be an integer between 0 and 26 inclusive
    //returns character at the given index
    char getChar(int index);
    
    //this function simply returns true if a word is found in the dictionary
    //and returns false if not found
    bool findWord(std::string word);

    struct trieNode
    {
        trieNode()
        {
            for(int i = 0; i < 27; i++)
            {
                children[i] = nullptr;
            }
            isWord = false;
        }
        
        ~trieNode()
        {
            for(int i = 0; i < 27; i++)
            {
                if(children[i] != nullptr)
                    delete children[i];
            }
        }
        
        //slots 0-25 represent the first 26 letters of the alphabet
        //slot 26 represents the apostrophe character
        trieNode* children[27];
        
        //true if this character and the ones preceding it represent a valid word
        bool isWord;
    };
    
    trieNode* m_root;
};

#endif  // STUDENTSPELLCHECK_H_
