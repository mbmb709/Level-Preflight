#include "PreflightReport.hpp"

#include <algorithm>
#include <cctype>
#include <fmt/format.h>
#include <string_view>
#include <utility>

namespace level_preflight {
namespace {

std::string trim(std::string_view input) {
    auto first = std::find_if_not(input.begin(), input.end(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    });
    auto last = std::find_if_not(input.rbegin(), input.rend(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    }).base();

    if (first >= last) {
        return {};
    }
    return std::string(first, last);
}

std::string lowercase(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

char const* markdownBadge(FindingKind kind) {
    switch (kind) {
        case FindingKind::Ready:   return "<cg>READY</c>";
        case FindingKind::Blocker: return "<cr>BLOCKER</c>";
        case FindingKind::Review:  return "<cy>REVIEW</c>";
        case FindingKind::Info:    return "<cb>INFO</c>";
    }
    return "<cb>INFO</c>";
}

char const* plainBadge(FindingKind kind) {
    switch (kind) {
        case FindingKind::Ready:   return "READY";
        case FindingKind::Blocker: return "BLOCKER";
        case FindingKind::Review:  return "REVIEW";
        case FindingKind::Info:    return "INFO";
    }
    return "INFO";
}

} // namespace

PreflightReport PreflightReport::inspect(GJGameLevel* level) {
    PreflightReport report;

    if (!level) {
        report.m_levelName = "Unknown level";
        report.add(
            FindingKind::Blocker,
            "Level data",
            "The level could not be inspected. Close this page and open it again."
        );
        return report;
    }

    report.m_levelName = trim(level->m_levelName.c_str());
    auto normalizedName = lowercase(report.m_levelName);

    if (
        report.m_levelName.empty() ||
        normalizedName == "unnamed" ||
        normalizedName == "new level"
    ) {
        report.add(
            FindingKind::Blocker,
            "Level name",
            "Choose a clear name before sharing the level."
        );
    }
    else {
        report.add(
            FindingKind::Ready,
            "Level name",
            "A non-default name is present."
        );
    }

    auto objectCount = static_cast<int>(level->m_objectCount);
    auto hasSerializedObjects = !trim(level->m_levelString.c_str()).empty();
    if (objectCount > 0) {
        report.add(
            FindingKind::Ready,
            "Level content",
            fmt::format("{} objects are recorded.", objectCount)
        );
    }
    else if (hasSerializedObjects) {
        report.add(
            FindingKind::Review,
            "Level content",
            "The level has saved data, but its object count is not available yet."
        );
    }
    else {
        report.add(
            FindingKind::Blocker,
            "Level content",
            "No objects are recorded in this level."
        );
    }

    auto verified =
        static_cast<int>(level->m_isVerified) != 0 ||
        level->m_isVerifiedRaw;
    if (verified) {
        report.add(
            FindingKind::Ready,
            "Verification",
            "A complete test run is recorded."
        );
    }
    else {
        report.add(
            FindingKind::Blocker,
            "Verification",
            level->isPlatformer()
                ? "Finish the platformer level once before sharing it."
                : "Complete one normal-mode run from 0% before sharing it."
        );
    }

    auto description = trim(level->getUnpackedLevelDescription().c_str());
    if (description.empty()) {
        report.add(
            FindingKind::Review,
            "Description",
            "The description is empty. Add context, credits, or play notes if useful."
        );
    }
    else {
        report.add(
            FindingKind::Ready,
            "Description",
            "A description is present."
        );
    }

    auto coinCount = static_cast<int>(level->m_coins);
    if (coinCount <= 0) {
        report.add(
            FindingKind::Info,
            "User coins",
            "This level does not declare user coins."
        );
    }
    else if (level->areCoinsVerified()) {
        report.add(
            FindingKind::Ready,
            "User coins",
            fmt::format(
                "{} user coin{} verified.",
                coinCount,
                coinCount == 1 ? "" : "s"
            )
        );
    }
    else {
        report.add(
            FindingKind::Review,
            "User coins",
            "At least one user coin is not verified."
        );
    }

    auto customSongID = static_cast<int>(level->m_songID);
    auto builtInTrackID = static_cast<int>(level->m_audioTrack);
    if (customSongID > 0) {
        report.add(
            FindingKind::Info,
            "Audio",
            fmt::format("Custom song ID {} is selected.", customSongID)
        );
    }
    else {
        report.add(
            FindingKind::Info,
            "Audio",
            fmt::format(
                "Built-in Geometry Dash track {} is selected.",
                builtInTrackID
            )
        );
    }

    std::string visibility = "Public";
    if (level->m_friendsOnly) {
        visibility = "Friends only";
    }
    else if (level->m_unlisted) {
        visibility = "Unlisted";
    }
    report.add(
        FindingKind::Info,
        "Visibility",
        fmt::format("{} sharing is currently selected.", visibility)
    );

    if (level->m_lowDetailMode) {
        report.add(
            FindingKind::Ready,
            "Low Detail Mode",
            "An LDM version is available."
        );
    }
    else {
        report.add(
            FindingKind::Info,
            "Low Detail Mode",
            "No LDM version is marked. This is optional; test performance manually."
        );
    }

    return report;
}

int PreflightReport::blockerCount() const {
    return static_cast<int>(std::count_if(
        m_findings.begin(),
        m_findings.end(),
        [](Finding const& finding) {
            return finding.kind == FindingKind::Blocker;
        }
    ));
}

int PreflightReport::reviewCount() const {
    return static_cast<int>(std::count_if(
        m_findings.begin(),
        m_findings.end(),
        [](Finding const& finding) {
            return finding.kind == FindingKind::Review;
        }
    ));
}

std::string PreflightReport::markdown() const {
    std::string text;

    if (blockerCount() == 0) {
        text += "<cg>Automated checks passed.</c> ";
    }
    else {
        text += fmt::format(
            "<cr>{} blocker{} found.</c> ",
            blockerCount(),
            blockerCount() == 1 ? "" : "s"
        );
    }

    if (reviewCount() > 0) {
        text += fmt::format(
            "<cy>{} item{} require manual review.</c>\n\n",
            reviewCount(),
            reviewCount() == 1 ? "" : "s"
        );
    }
    else {
        text += "No extra review items were detected.\n\n";
    }

    for (auto const& finding : m_findings) {
        text += fmt::format(
            "- {} **{}** - {}\n",
            markdownBadge(finding.kind),
            finding.title,
            finding.detail
        );
    }

    text += "\n---\n";
    text += "**Still check manually:** gameplay readability, credits, song/SFX use, "
            "performance on another device, and every upload option. "
            "This report never blocks uploading and is not a rating guarantee.";

    return text;
}

std::string PreflightReport::plainText() const {
    auto safeName = m_levelName.empty() ? std::string("Unnamed level") : m_levelName;
    std::string text = fmt::format("Level Preflight - {}\n", safeName);
    text += fmt::format(
        "{} blocker{}, {} review item{}\n\n",
        blockerCount(),
        blockerCount() == 1 ? "" : "s",
        reviewCount(),
        reviewCount() == 1 ? "" : "s"
    );

    for (auto const& finding : m_findings) {
        text += fmt::format(
            "[{}] {}: {}\n",
            plainBadge(finding.kind),
            finding.title,
            finding.detail
        );
    }

    text += "\nManual review: gameplay readability, credits, song/SFX use, "
            "performance on another device, and upload options.";
    return text;
}

void PreflightReport::add(
    FindingKind kind,
    std::string title,
    std::string detail
) {
    m_findings.push_back({
        kind,
        std::move(title),
        std::move(detail),
    });
}

} // namespace level_preflight
