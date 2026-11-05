#pragma once

#include <ostream>
#include <stdint.h>
#include <string>
#include <vector>

struct Location
{
    const std::string& filename;
    const uint32_t line;
    const uint32_t column;

    Location(const std::string& filename, uint32_t line, uint32_t col)
        : filename(filename), line(line), column(col)
    {
    }
};

/// The locator can transform integer offsets
/// into concrete Locations without keeping the file in memory
struct Locator
{
private:
    const std::string m_FileName;
    const std::string& m_Code;
    std::vector<uint32_t> m_LineStarts;

public:
    Locator(const std::string& filename, const std::string& code);

    Location getLocationByOffset(uint32_t offset) const;

    const std::string& getFileName() const { return m_FileName; };

private:
    void preprocess();
};

std::ostream& operator<<(std::ostream& os, const Location& loc);
