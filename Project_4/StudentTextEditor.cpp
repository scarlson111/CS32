#include "StudentTextEditor.h"
#include "Undo.h"
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

TextEditor* createTextEditor(Undo* un)
{
    return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo) {
     m_currentCol = 0;
     m_currentRow = 0;
     m_numLines = 1;
     m_textEditor.push_back("");
     m_it = m_textEditor.begin();
     
}

StudentTextEditor::~StudentTextEditor()
{
    m_textEditor.clear();
}

bool StudentTextEditor::load(std::string file) {
    ifstream infile(file);
    if(!infile) //specified input file cannot be found;
        return false;
    reset(); //input file can be loaded so reset the editor
    string temp;
    while(getline(infile,temp))
    {
        if(temp != "" && temp.substr(temp.size()-1) == "\r")//strip carriage return at end of string if there is one
        {
            temp = temp.substr(0,temp.size()-1);
        }
        m_textEditor.push_back(temp);
        m_numLines++;
    }
    m_it =  m_textEditor.begin();
    return true;
}

bool StudentTextEditor::save(std::string file) {
    ofstream outfile(file);
    if(!outfile) //specified file failed to be opened
        return false;
    list<string>::iterator it;
    for(it = m_textEditor.begin(); it!= m_textEditor.end(); it++)
    {
        outfile << (*it) << '\n';
    }
    return true;
}

void StudentTextEditor::reset() {
    m_textEditor.clear();
    m_numLines = 0;
    m_currentCol = 0;
    m_currentRow = 0;
    m_it = m_textEditor.begin();
    getUndo()->clear();
}

void StudentTextEditor::move(Dir dir) {
    if(dir == UP)
    {
        if(m_currentRow > 0)
        {
            m_currentRow--;
            m_it--;
            if((*m_it).size()<m_currentCol)
            {
                m_currentCol = (*m_it).size();
            }
        }
    }
    else if(dir == DOWN)
    {
        if(m_currentRow != m_numLines-1)
        {
            m_currentRow++;
            m_it++;
            if((*m_it).size()<m_currentCol)
            {
                m_currentCol = (*m_it).size();
            }
        }
    }
    else if(dir == LEFT)
    {
        if(m_currentCol != 0)
        {
            m_currentCol--;
        }
        else if(m_currentRow!=0)
        {
            m_currentRow--;
            m_it--;
            m_currentCol = (*m_it).size();
        }
    }
    else if(dir == RIGHT)
    {
        if(m_currentCol == (*m_it).size())
        {
            if(m_currentRow + 1 != m_numLines)
            {
                m_currentRow++;
                m_it++;
                m_currentCol = 0;
            }
        }
        else
        {
            m_currentCol++;
        }
    }
    else if(dir == HOME)
    {
        m_currentCol = 0;
    }
    else if(dir == END)
    {
        m_currentCol = (*m_it).size();
    }
}

void StudentTextEditor::del() {
    if(m_currentCol == (*m_it).size())
    {
        if(m_currentRow != m_numLines-1)
        {
            m_it++;
            string temp = *m_it;
            m_it = m_textEditor.erase(m_it);
            m_it--;
            *m_it += temp;
            m_numLines--;
            getUndo()->submit(Undo::Action::JOIN, m_currentRow, m_currentCol);
        }
    }
    else
    {
        char c = (*m_it).at(m_currentCol);
        *m_it = (*m_it).substr(0,m_currentCol) + (*m_it).substr(m_currentCol+1);
        getUndo()->submit(Undo::Action::DELETE, m_currentRow, m_currentCol, c);
    }
    
}

void StudentTextEditor::backspace() {
    if(m_currentCol > 0)
    {
        char c = (*m_it).at(m_currentCol-1);
        *m_it = (*m_it).substr(0,m_currentCol-1) + (*m_it).substr(m_currentCol);
        m_currentCol--;
        getUndo()->submit(Undo::Action::DELETE, m_currentRow, m_currentCol,c);
    }
    else if(m_currentRow > 0)
    {
        string temp = *m_it;
        m_it = m_textEditor.erase(m_it);
        m_it--;
        m_currentRow--;
        m_numLines--;
        m_currentCol = (*m_it).size();
        *m_it += temp;
        getUndo()->submit(Undo::Action::JOIN, m_currentRow, m_currentCol);
    }
}

void StudentTextEditor::insert(char ch) {
    if(ch != '\t')
    {
        *m_it = (*m_it).substr(0,m_currentCol) + ch + (*m_it).substr(m_currentCol);
        m_currentCol++;
    }
    else
    {
        *m_it = (*m_it).substr(0,m_currentCol) + "    " + (*m_it).substr(m_currentCol);
        m_currentCol += 4;
    }
    getUndo()->submit(Undo::Action::INSERT, m_currentRow, m_currentCol,ch);
}

void StudentTextEditor::enter() {
    getUndo()->submit(Undo::Action::SPLIT, m_currentRow, m_currentCol);
    string temp = (*m_it).substr(m_currentCol);
    *m_it = (*m_it).substr(0,m_currentCol);
    m_it++;
    m_textEditor.insert(m_it, temp);
    m_it--;
    m_currentRow++;
    m_currentCol = 0;
    m_numLines++;
}

void StudentTextEditor::getPos(int& row, int& col) const {
    row = m_currentRow;
    col = m_currentCol;
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
    if(startRow < 0 || numRows < 0 || startRow > m_numLines)
        return -1;
    lines.clear();
    list<string>::iterator it = m_it;
    int i = 0;
    if(startRow < m_currentRow)
    {
        while(i < m_currentRow - startRow)
        {
            it--;
            i++;
        }
    }
    else if(startRow > m_currentRow)
    {
        while(i < startRow-m_currentRow)
        {
            it++;
            i++;
        }
    }
    i = 0;
    while(i < numRows && it != m_textEditor.end())
    {
        lines.push_back(*it);
        it++;
        i++;
    }
    return lines.size();
}

void StudentTextEditor::undo() {
    int row = 0;
    int col = 0;
    int count = 0 ;
    string text = "";
    Undo::Action action = getUndo()->get(row, col, count, text);
    
    //if there is no action to undo, return right away
    if(action == Undo::Action::ERROR)
        return;
    
    //reposition cursor to specified row and column
    m_currentCol = col;
    while(m_currentRow < row)
    {
        m_currentRow++;
        m_it++;
    }
    while(m_currentRow > row)
    {
        m_currentRow--;
        m_it--;
    }
    
    //perform undo
    if(action == Undo::Action::DELETE)
    {
        while(count > 0)
        {
            *m_it = (*m_it).substr(0,m_currentCol-1) + (*m_it).substr(m_currentCol);
            m_currentCol--;
            count--;
        }
    }
    else if(action == Undo::Action::INSERT)
    {
        *m_it = (*m_it).substr(0,m_currentCol) + text + (*m_it).substr(m_currentCol);
    }
    else if(action == Undo::Action::JOIN)
    {
        m_it++;
        string temp = *m_it;
        m_it = m_textEditor.erase(m_it);
        m_it--;
        *m_it += temp;
        m_numLines--;
    }
    else if(action == Undo::Action::SPLIT)
    {
        string temp = (*m_it).substr(m_currentCol);
        *m_it = (*m_it).substr(0,m_currentCol);
        m_it++;
        m_textEditor.insert(m_it, temp);
        m_it--;
        m_it--;
        m_numLines++;
    }
}

