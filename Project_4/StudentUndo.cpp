#include "StudentUndo.h"

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
	// TODO
    undoInfo temp;
    if(action == INSERT)
    {
        if(!m_undoStack.empty() && m_undoStack.top().type == INSERT && m_undoStack.top().row == row && m_undoStack.top().col + 1 == col) //consecutive insert operations to be batched
        {
            if(ch == '\t')
                m_undoStack.top().numChars += 4;
            else
                m_undoStack.top().numChars++;
            m_undoStack.top().col = col;
        }
        else
        {
        temp.type = INSERT;
        temp.row = row;
        temp.col = col;
        if(ch == '\t')
            temp.numChars = 4;
        else
            temp.numChars = 1;
        m_undoStack.push(temp);
        }
    }
    else if(action == DELETE)
    {
        if(!m_undoStack.empty() && m_undoStack.top().type == DELETE && m_undoStack.top().row == row)
        {
            if(m_undoStack.top().col == col) //consecutive delete operations to be batched
            {
                m_undoStack.top().str += ch;
            }
            else if(m_undoStack.top().col - 1 == col)  //consecutive backspace operations to be batched
            {
                m_undoStack.top().col = col;
                m_undoStack.top().str = ch + m_undoStack.top().str;
            }
        }
        else
        {
            temp.type = DELETE;
            temp.col = col;
            temp.row = row;
            temp.str = ch;
            m_undoStack.push(temp);
        }
    }
    else if(action == SPLIT)
    {
        temp.type = SPLIT;
        temp.col = col;
        temp.row = row;
        m_undoStack.push(temp);
    }
    else if (action == JOIN)
    {
        temp.type = JOIN;
        temp.col = col;
        temp.row = row;
        m_undoStack.push(temp);
    }
}

StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text) {
    if(m_undoStack.empty())
        return Action::ERROR;
    if(m_undoStack.top().type == INSERT)
    {
        row = m_undoStack.top().row;
        col = m_undoStack.top().col;
        count = m_undoStack.top().numChars;
        m_undoStack.pop();
        return Action::DELETE;
    }
    else if(m_undoStack.top().type == DELETE)
    {
        row = m_undoStack.top().row;
        col = m_undoStack.top().col;
        text = m_undoStack.top().str;
        m_undoStack.pop();
        return Action::INSERT;
    }
    else if(m_undoStack.top().type == SPLIT)
    {
        row = m_undoStack.top().row;
        col = m_undoStack.top().col;
        m_undoStack.pop();
        return Action::JOIN;
    }
    else if(m_undoStack.top().type == JOIN)
    {
        row = m_undoStack.top().row;
        col = m_undoStack.top().col;
        m_undoStack.pop();
        return Action::SPLIT;
    }
    else
        return Action::ERROR;
}

void StudentUndo::clear() {
    while(!m_undoStack.empty())
        m_undoStack.pop();
}


