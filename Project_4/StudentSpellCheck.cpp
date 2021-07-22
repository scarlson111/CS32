#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

StudentSpellCheck::~StudentSpellCheck() {
    delete m_root;
}

bool StudentSpellCheck::load(std::string dictionaryFile) {
    ifstream infile(dictionaryFile);
    if(!infile) //dictionary file can not be opened
        return false;
    
    //initalize root of trie
    m_root = new trieNode;
    for(int i = 0; i < 27; i++)
    {
        m_root->children[i] = nullptr;
    }
    
    string temp;
    while(getline(infile,temp))
    {
        stripNonAlpha(temp);
        if(temp.size() != 0)
        {
            toLowercase(temp);
            int index;
            trieNode* currNode = m_root;
            for(int i = 0; i < temp.size(); i++)
            {
                index = getIndex(temp.at(i));
                if(currNode->children[index] == nullptr)
                    currNode->children[index] = new trieNode;
                currNode = currNode->children[index];
            }
            currNode->isWord = true;
        }
    }
	return true;
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
    if(findWord(word))
        return true;
    toLowercase(word);
    suggestions.clear();
    string str = "";
    bool reachedMax = false;
    for(int i = 0; i < word.size(); i++)
    {
        bool isValid = true;
        trieNode* currNode = m_root;
        for(int j = 0; j < i; j++)
        {
            if(currNode->children[getIndex(word.at(j))] != nullptr)
                currNode = currNode->children[getIndex(word.at(j))];
            else
            {
                isValid = false;
                break;
            }
        }
        if(!isValid)
            break;
        for(int j = 0 ; j < 27; j++)
        {
            isValid = true;
            trieNode* tempNode = currNode;
            str = word.substr(0,i);
            if(tempNode->children[j] != nullptr)
            {
                tempNode = tempNode->children[j];
                str += getChar(j);
                for(int k = i+1; k < word.size(); k++)
                {
                    if(tempNode->children[getIndex(word.at(k))] != nullptr)
                    {
                        tempNode = tempNode->children[getIndex(word.at(k))];
                        str += word.at(k);
                    }
                    else
                    {
                        isValid = false;
                        break;
                    }
                }
                if(isValid && tempNode->isWord)
                {
                    suggestions.push_back(str);
                    if(suggestions.size() == max_suggestions)
                    {
                        reachedMax = true;
                        break;
                    }
                }
            }
        }
        if(reachedMax)
            break;
    }
    return false;
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
    problems.clear();
    for(int i = 0; i < line.size(); i++)
    {
        if(isalpha(line.at(i)) || line.at(i) == '\'')
        {
            string str = "";
            int startIndex = i;
            while(i < line.size() && (isalpha(line.at(i)) || line.at(i) == '\''))
            {
                str += line.at(i);
                i++;
            }
            if(!findWord(str))
            {
                SpellCheck::Position temp;
                temp.start = startIndex;
                temp.end = i-1;
                problems.push_back(temp);
            }
        }
    }
}

void StudentSpellCheck::stripNonAlpha(string& str)
{
    for(int i = 0; i  < str.size(); i++)
    {
        if(!isalpha(str.at(i)) && str.at(i) != '\'')
        {
            str = str.substr(0,i) + str.substr(i+1);
            i--;
        }
    }
}

void StudentSpellCheck::toLowercase(string& str)
{
    for(int i = 0; i < str.size(); i++)
    {
        if(isalpha(str.at(i)))
        {
            char c = tolower(str.at(i));
            str = str.substr(0,i) + c + str.substr(i+1);
        }
    }
}

int StudentSpellCheck::getIndex(char ch)
{
    if(ch == 'a')
        return 0;
    else if(ch == '\'')
        return 26;
    else
        return ch - 'a';
}

char StudentSpellCheck::getChar(int index)
{
    if(index == 26)
    {
        return '\'';
    }
    return index + 'a';
}

bool StudentSpellCheck::findWord(string word)
{
    toLowercase(word);
    int index;
    trieNode* currNode = m_root;
    bool wordPossiblyFound = true;
    for(int i = 0; i < word.size(); i++)
    {

        index = getIndex(word.at(i));
        if(currNode->children[index] != nullptr)
        {
            currNode = currNode->children[index];
        }
        else
        {
            wordPossiblyFound = false;
            break;
        }
    }
    if(wordPossiblyFound && currNode->isWord)
    {
        return true;
    }
    return false;
}
