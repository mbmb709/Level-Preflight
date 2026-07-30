#pragma once

#include <Geode/Geode.hpp>

#include <string>
#include <vector>

namespace level_preflight {

enum class FindingKind {
    Ready,
    Blocker,
    Review,
    Info,
};

struct Finding {
    FindingKind kind;
    std::string title;
    std::string detail;
};

class PreflightReport final {
public:
    static PreflightReport inspect(GJGameLevel* level);

    [[nodiscard]] int blockerCount() const;
    [[nodiscard]] int reviewCount() const;
    [[nodiscard]] std::string markdown() const;
    [[nodiscard]] std::string plainText() const;

private:
    std::string m_levelName;
    std::vector<Finding> m_findings;

    void add(FindingKind kind, std::string title, std::string detail);
};

} // namespace level_preflight
