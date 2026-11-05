#include "Locator.h"

Locator::Locator(const std::string& filename, const std::string& code)
    : m_FileName(filename), m_Code(code), m_LineStarts()
{
    preprocess();
}

/// This function traverses the code of a file to build the linestarts vector.
/// For each new line it puts the index of where the line starts into the
/// vector.
void Locator::preprocess()
{
    m_LineStarts.reserve(32);
    m_LineStarts.push_back(0);

    bool flag = false;
    for (size_t i = 0; i < m_Code.size(); i++)
    {
        char ch = m_Code[i];

        if (flag)
        {
            flag = false;
            m_LineStarts.push_back((uint32_t)i + (ch == '\n'));
        }
        else
        {
            if (ch == '\n')
            {
                m_LineStarts.push_back((uint32_t)i + 1);
            }

            flag = ch == '\r';
        }
    }
}

/// Computes the linenumber and column based on the given offset.
/// The linestarts vector contains all integer offsets of
/// the first character on each line.
Location Locator::getLocationByOffset(uint32_t offset) const
{
    size_t line = 0;

    for (size_t i = 0; i < m_LineStarts.size(); i++)
    {
        if (m_LineStarts[i] <= offset)
            line = i;
        else
            break;
    }

    // we have to do `line + 1` because of zero-based indexing
    // and `offset - linestart + 1` yields the column.
    return Location(m_FileName, (uint32_t)line + 1, offset - m_LineStarts[line] + 1);
}

std::ostream& operator<<(std::ostream& os, const Location& loc)
{
    os << loc.filename << ':' << loc.line << ':' << loc.column;
    return os;
}
