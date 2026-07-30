#include "PreflightPopup.hpp"

#include <Geode/Geode.hpp>
#include <Geode/ui/MDTextArea.hpp>
#include <Geode/ui/Notification.hpp>

using namespace geode::prelude;

namespace level_preflight {

PreflightPopup* PreflightPopup::create(GJGameLevel* level) {
    auto popup = new PreflightPopup();
    if (popup && popup->init(level)) {
        popup->autorelease();
        return popup;
    }

    CC_SAFE_DELETE(popup);
    return nullptr;
}

bool PreflightPopup::init(GJGameLevel* level) {
    if (!Popup::init(430.f, 290.f)) {
        return false;
    }

    auto report = PreflightReport::inspect(level);
    m_plainReport = report.plainText();

    setTitle("Level Preflight", "goldFont.fnt", .8f, 22.f);

    auto reportArea = MDTextArea::create(
        report.markdown(),
        {m_size.width - 40.f, m_size.height - 96.f}
    );
    if (!reportArea) {
        return false;
    }
    reportArea->setPosition({m_size.width / 2.f, m_size.height / 2.f + 7.f});
    reportArea->setID("report"_spr);
    m_mainLayer->addChild(reportArea);

    auto copySprite = ButtonSprite::create("Copy report");
    if (!copySprite) {
        return false;
    }
    auto copyButton = CCMenuItemSpriteExtra::create(
        copySprite,
        this,
        menu_selector(PreflightPopup::onCopyReport)
    );
    if (!copyButton) {
        return false;
    }
    copyButton->setID("copy-report-button"_spr);

    auto copyMenu = CCMenu::create();
    if (!copyMenu) {
        return false;
    }
    copyMenu->setPosition({m_size.width / 2.f, 23.f});
    copyMenu->setID("copy-report-menu"_spr);
    copyMenu->addChild(copyButton);
    m_mainLayer->addChild(copyMenu);

    return true;
}

void PreflightPopup::onCopyReport(CCObject*) {
    if (utils::clipboard::write(m_plainReport)) {
        Notification::create(
            "Preflight report copied",
            NotificationIcon::Success,
            1.2f
        )->show();
    }
    else {
        Notification::create(
            "Could not access the clipboard",
            NotificationIcon::Error,
            1.8f
        )->show();
    }
}

} // namespace level_preflight
