#pragma once

#include "PreflightReport.hpp"

#include <Geode/ui/Popup.hpp>
#include <string>

namespace level_preflight {

class PreflightPopup final : public geode::Popup {
public:
    static PreflightPopup* create(GJGameLevel* level);

private:
    std::string m_plainReport;

    bool init(GJGameLevel* level);
    void onCopyReport(cocos2d::CCObject*);
};

} // namespace level_preflight
