#include "PreflightPopup.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/ui/Layout.hpp>

using namespace geode::prelude;

class $modify(LevelPreflightEditLevelLayer, EditLevelLayer) {
    bool init(GJGameLevel* level) {
        if (!EditLevelLayer::init(level)) {
            return false;
        }

        auto actionsMenu = getChildByID("level-actions-menu");
        if (!actionsMenu) {
            log::warn("Level Preflight could not find EditLevelLayer's action menu");
            return true;
        }

        auto icon = CircleButtonSprite::createWithSprite(
            "preflight.png"_spr,
            .72f,
            CircleBaseColor::Green,
            CircleBaseSize::Small
        );
        if (!icon) {
            log::warn("Level Preflight could not create its button sprite");
            return true;
        }

        auto button = CCMenuItemSpriteExtra::create(
            icon,
            this,
            menu_selector(LevelPreflightEditLevelLayer::onPreflight)
        );
        button->setID("preflight-button"_spr);
        actionsMenu->addChild(button);
        actionsMenu->updateLayout();

        return true;
    }

    void onPreflight(CCObject*) {
        auto popup = level_preflight::PreflightPopup::create(m_level);
        if (popup) {
            popup->show();
        }
    }
};
