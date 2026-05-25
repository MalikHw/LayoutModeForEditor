#include <Geode/modify/EditorUI.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/ui/Button.hpp>

using namespace geode::prelude;

static constexpr std::array<std::pair<int, cocos2d::ccColor3B>, 8> specialColors = {{
	{1000, {40, 125, 255}},		// BG
	{1001, {0, 102, 255}},		// G1
	{1002, {255, 255, 255}},	// LINE
	{1003, {255, 255, 255}},	// 3DL
	{1004, {255, 255, 255}},	// OBJ
	{1009, {0, 102, 255}},		// G2
	{1013, {40, 125, 255}},		// MG
	{1014, {40, 125, 255}}		// MG2
}};

static constexpr std::array<GameObjectType, 11> ringTypes = {{
	GameObjectType::YellowJumpRing,
	GameObjectType::PinkJumpRing,
	GameObjectType::GravityRing,
	GameObjectType::GreenRing,
	GameObjectType::DropRing,
	GameObjectType::RedJumpRing,
	GameObjectType::CustomRing,
	GameObjectType::DashRing,
	GameObjectType::GravityDashRing,
	GameObjectType::SpiderOrb,
	GameObjectType::TeleportOrb
}};

static constexpr std::array<GameObjectType, 5> padTypes = {{
	GameObjectType::YellowJumpPad,
	GameObjectType::PinkJumpPad,
	GameObjectType::GravityPad,
	GameObjectType::RedJumpPad,
	GameObjectType::SpiderPad
}};

static constexpr std::array<GameObjectType, 18> portalTypes = {{
	GameObjectType::InverseGravityPortal,
	GameObjectType::NormalGravityPortal,
	GameObjectType::ShipPortal,
	GameObjectType::CubePortal,
	GameObjectType::InverseMirrorPortal,
	GameObjectType::NormalMirrorPortal,
	GameObjectType::BallPortal,
	GameObjectType::RegularSizePortal,
	GameObjectType::MiniSizePortal,
	GameObjectType::UfoPortal,
	GameObjectType::DualPortal,
	GameObjectType::SoloPortal,
	GameObjectType::WavePortal,
	GameObjectType::RobotPortal,
	GameObjectType::TeleportPortal,
	GameObjectType::SpiderPortal,
	GameObjectType::SwingPortal,
	GameObjectType::GravityTogglePortal
}};

static constexpr std::array<int, 5> speedPortalIDs = {{ 200, 201, 202, 203, 1334 }};
static constexpr std::array<int, 11> colorTriggerIDs = {{ 29, 30, 104, 105, 221, 717, 718, 743, 744, 899, 915 }};
static constexpr std::array<int, 9> alphaPulseTintIDs = {{ 1006, 1007, 2903, 3009, 3010, 3014, 3015, 3020, 3021 }};

static constexpr std::array<int, 8> cameraTriggerIDs = {{ 1913, 1914, 1916, 2015, 2016, 2062, 2901, 2925 }};
static constexpr std::array<int, 18> shaderTriggerIDs = {{ 2904, 2905, 2907, 2909, 2910, 2911, 2912, 2913, 2914, 2915, 2916, 2917, 2919, 2920, 2921, 2922, 2923, 2924 }};

class $modify(MyEditorUI, EditorUI) {
	static bool notRing(const GameObjectType t) {
		return !std::binary_search(ringTypes.begin(), ringTypes.end(), t);
	}
	static bool notPad(const GameObjectType t) {
		return !std::binary_search(padTypes.begin(), padTypes.end(), t);
	}
	static bool notPortal(const GameObjectType t) {
		return !std::binary_search(portalTypes.begin(), portalTypes.end(), t);
	}
	static bool notSpeed(const int i) {
		return !std::binary_search(speedPortalIDs.begin(), speedPortalIDs.end(), i);
	}
	static bool notGameplay(const GameObjectType t, const int i) {
		return MyEditorUI::notRing(t) && MyEditorUI::notPad(t) && MyEditorUI::notPortal(t) && MyEditorUI::notSpeed(i);
	}
	static bool isColorTriggerBothModernAndClassic(const int i) {
		return std::binary_search(colorTriggerIDs.begin(), colorTriggerIDs.end(), i);
	}
	static bool isAlphaOrPulseOrSimilar(const int i) {
		return std::binary_search(alphaPulseTintIDs.begin(), alphaPulseTintIDs.end(), i);
	}
	static bool isColorOrAlphaOrPulseOrSimilar(const int i) {
		return MyEditorUI::isColorTriggerBothModernAndClassic(i) || MyEditorUI::isAlphaOrPulseOrSimilar(i);
	}
	static bool isShader(const int i, const bool shaders) {
		if (!shaders) return false;
		return std::binary_search(shaderTriggerIDs.begin(), shaderTriggerIDs.end(), i);
	}
	static bool isCamera(const int i, const bool cameras) {
		if (!cameras) return false;
		return std::binary_search(cameraTriggerIDs.begin(), cameraTriggerIDs.end(), i);
	}
	static bool isStupidFuckingShakeTrigger(const int i, const bool shakes) {
		if (!shakes) return false;
		return i == 1520;
	}
	static bool isReallyFuckingAnnoying(const int i, const bool shaders, const bool cameras, const bool shakes) {
		return MyEditorUI::isShader(i, shaders) || MyEditorUI::isCamera(i, cameras) || MyEditorUI::isStupidFuckingShakeTrigger(i, shakes);
	}
	static void setColorActionValues(ColorAction* colorAction, const cocos2d::ccColor3B& color) {
		if (!colorAction) return;

		colorAction->m_color = color;
		colorAction->m_fromColor = color;
		colorAction->m_toColor = color;
		colorAction->m_currentOpacity = 1.f;
		colorAction->m_fromOpacity = 1.f;
		colorAction->m_toOpacity = 1.f;
		colorAction->m_blending = false;

		if (colorAction->m_colorSprite) {
			colorAction->m_colorSprite->m_color = color;
			colorAction->m_colorSprite->m_opacity = 1.f;
		}

		if (colorAction->m_colorID == 1002) {
			colorAction->m_blending = true;
		}
	}
	bool init(LevelEditorLayer* layer) {
		if (!EditorUI::init(layer)) return false;
		if (!Mod::get()->getSettingValue<bool>("enabled")) return true;

		CCMenu* menu = typeinfo_cast<CCMenu*>(this->querySelector("delete-category-menu > delete-button-menu"));
		if (!menu) return true;

		CCLabelBMFont* label = CCLabelBMFont::create("Layout\nMode", "goldFont.fnt");
		label->setScale(.375f);
		label->setAlignment(kCCTextAlignmentCenter);

		CCMenuItemSpriteExtra* trashButton = EditorUI::getSpriteButton("edit_delBtn_001.png", menu_selector(MyEditorUI::onDeleteDeco), menu, .9f, 4, {0.f, 0.f});
		trashButton->setTag(20260524);
		trashButton->setID("delete-deco-button"_spr);
		trashButton->addChildAtPosition(label, Anchor::Center);
		menu->addChild(trashButton);
		menu->updateLayout();
		return true;
	}

	void deleteAllDeco() {
		if (!m_editorLayer || !m_editorLayer->getAllObjects()) return;
		if (!Mod::get()->getSettingValue<bool>("enabled")) return Notification::create("The mod was disabled. No objects were deleted.", NotificationIcon::None, .5f)->show();

		const bool shaders = Mod::get()->getSettingValue<bool>("shaders");
		const bool cameras = Mod::get()->getSettingValue<bool>("cameras");
		const bool shakes = Mod::get()->getSettingValue<bool>("shakes");

		const bool hideCollectible = Mod::get()->getSettingValue<bool>("hideCollectible");
		const bool hideCollisionObject = Mod::get()->getSettingValue<bool>("hideCollisionObject");

		const bool hideCollectibleOverride = Mod::get()->getSettingValue<bool>("hideCollectibleOverride");
		const bool hideCollisionObjectOverride = Mod::get()->getSettingValue<bool>("hideCollisionObjectOverride");

		std::vector<int> groupIDsFromPulseAlphaTintTriggers = {};

		const CCArrayExt<GameObject*> allObjects = CCArrayExt<GameObject*>(m_editorLayer->getAllObjects());
		for (GameObject* obj : allObjects) {
			if (!obj || obj->m_classType != GameObjectClassType::Effect || !MyEditorUI::isAlphaOrPulseOrSimilar(obj->m_objectID)) continue;
			const EffectGameObject* effect = static_cast<EffectGameObject*>(obj);
			if (effect->m_targetGroupID > 0) groupIDsFromPulseAlphaTintTriggers.push_back(effect->m_targetGroupID);
			if (effect->m_centerGroupID > 0) groupIDsFromPulseAlphaTintTriggers.push_back(effect->m_centerGroupID);
			if (effect->m_rotationTargetID > 0) groupIDsFromPulseAlphaTintTriggers.push_back(effect->m_rotationTargetID);
			if (effect->m_targetModCenterID > 0) groupIDsFromPulseAlphaTintTriggers.push_back(effect->m_targetModCenterID);
		}

		std::sort(groupIDsFromPulseAlphaTintTriggers.begin(), groupIDsFromPulseAlphaTintTriggers.end());
		groupIDsFromPulseAlphaTintTriggers.erase(
			std::unique(groupIDsFromPulseAlphaTintTriggers.begin(), groupIDsFromPulseAlphaTintTriggers.end()),
			groupIDsFromPulseAlphaTintTriggers.end()
		);

		const bool hasNoGlowOverride = Mod::get()->getSettingValue<bool>("hasNoGlowOverride");
		const bool isDontFadeOverride = Mod::get()->getSettingValue<bool>("isDontFadeOverride");
		const bool isDontEnterOverride = Mod::get()->getSettingValue<bool>("isDontEnterOverride");
		const bool hasNoParticlesOverride = Mod::get()->getSettingValue<bool>("hasNoParticlesOverride");
		const bool hasNoAudioScaleOverride = Mod::get()->getSettingValue<bool>("hasNoAudioScaleOverride");
		const bool hasNoEffectsOverride = Mod::get()->getSettingValue<bool>("hasNoEffectsOverride");

		const bool hasNoGlow = Mod::get()->getSettingValue<bool>("hasNoGlow");
		const bool isDontFade = Mod::get()->getSettingValue<bool>("isDontFade");
		const bool isDontEnter = Mod::get()->getSettingValue<bool>("isDontEnter");
		const bool hasNoParticles = Mod::get()->getSettingValue<bool>("hasNoParticles");
		const bool hasNoAudioScale = Mod::get()->getSettingValue<bool>("hasNoAudioScale");

		const std::string& decoDeletionMode = geode::utils::string::toLower(Mod::get()->getSettingValue<std::string>("decoration"));
		const bool hideAllInsteadDecoDeletionMode = decoDeletionMode == "hide all instead";
		const bool aggressiveDecoDeletionMode = decoDeletionMode == "aggressive";

		CCArray* objectsToDelete = CCArray::create();
		for (GameObject* obj : allObjects) {
			if (!obj) continue;

			const GameObjectType t = obj->m_objectType;
			const int i = obj->m_objectID;

			if (hasNoGlowOverride) obj->m_hasNoGlow = hasNoGlow;
			if (isDontFadeOverride) obj->m_isDontFade = isDontFade;
			if (isDontEnterOverride) obj->m_isDontEnter = isDontEnter;
			if (hasNoParticlesOverride) obj->m_hasParticles = !hasNoParticles;
			if (hasNoParticlesOverride) obj->m_hasNoParticles = hasNoParticles;
			if (hasNoAudioScaleOverride) obj->m_usesAudioScale = !hasNoAudioScale;
			if (hasNoAudioScaleOverride) obj->m_hasNoAudioScale = hasNoAudioScale;

			if (hideCollectibleOverride && t == GameObjectType::Collectible) {
				obj->m_isHide = hideCollectible;
			} else if (hideCollisionObjectOverride && t == GameObjectType::CollisionObject) {
				obj->m_isHide = hideCollisionObject;
			} else if (MyEditorUI::notGameplay(t, i)) {
				// don't affect isHide property of gameplay objects, some people like to keep things invisible for a reason
				obj->m_isHide = false;
			} else if (hasNoEffectsOverride) {
				// at this point we already know it is a gameplay object and cant be any of the other types after this conditional
				obj->m_hasNoEffects = obj->m_isHide;
				continue;
			}

			if (MyEditorUI::isColorOrAlphaOrPulseOrSimilar(i) || MyEditorUI::isReallyFuckingAnnoying(i, shaders, cameras, shakes)) {
				objectsToDelete->addObject(obj);
				continue;
			}

			if (t != GameObjectType::Decoration) continue;

			if (hideAllInsteadDecoDeletionMode) {
				obj->m_isHide = true;
				continue;
			}

			if (aggressiveDecoDeletionMode) {
				objectsToDelete->addObject(obj);
				continue;
			}

			if (obj->m_groups) {
				if (obj->m_groups->empty() || obj->m_groupCount < 1) {
					objectsToDelete->addObject(obj);
					continue;
				}
				bool decoOnlyTargetedByPulseAlphaOrTint = true;
				for (size_t groupIdx = 0; groupIdx < obj->m_groupCount; groupIdx++) {
					if (std::binary_search(groupIDsFromPulseAlphaTintTriggers.begin(), groupIDsFromPulseAlphaTintTriggers.end(), obj->m_groups->at(groupIdx))) continue;
					decoOnlyTargetedByPulseAlphaOrTint = false;
					break;
				}
				if (decoOnlyTargetedByPulseAlphaOrTint) {
					objectsToDelete->addObject(obj);
				} else {
					obj->m_isHide = true;
				}
				continue;
			}

			objectsToDelete->addObject(obj);
		}

		const int numToDelete = objectsToDelete->count();
		bool objectsWereDeleted = false;
		bool colorsWereReset = false;

		if (numToDelete > 0) {
			m_editorLayer->addToUndoList(UndoObject::createWithArray(objectsToDelete, UndoCommand::DeleteMulti), false);
			for (GameObject* obj : CCArrayExt<GameObject*>(objectsToDelete)) {
				if (obj) EditorUI::deleteObject(obj, true);
			}
			objectsWereDeleted = true;
		}

		if (m_editorLayer->m_levelSettings && m_editorLayer->m_levelSettings->m_effectManager) {
			if (Mod::get()->getSettingValue<bool>("resetAllColorChannels")) {
				colorsWereReset = true;
				for (int id = 1; id < 1000; ++id) {
					MyEditorUI::setColorActionValues(m_editorLayer->m_levelSettings->m_effectManager->getColorAction(id), {255, 255, 255});
				}
			}

			if (Mod::get()->getSettingValue<bool>("resetAllSpecialColorChannels")) {
				colorsWereReset = true;
				for (const auto& [id, color] : specialColors) {
					MyEditorUI::setColorActionValues(m_editorLayer->m_levelSettings->m_effectManager->getColorAction(id), color);
				}
			}
		}

		Notification::create(fmt::format("{}, and {}", objectsWereDeleted ? fmt::format("{} object{} deleted", numToDelete, numToDelete != 1 ? "s were" : " was") : "No objects were deleted", colorsWereReset ? "some colors were reset." : "no colors were reset."), NotificationIcon::None, 1.f)->show();
	}

	void onDeleteDeco(CCObject* sender) {
		if (!sender || sender->getTag() != 20260524) return;
		geode::Popup* popup = openSettingsPopup(Mod::get(), false);
		if (!popup || !popup->m_buttonMenu) return;
		popup->addOnExitCallback([foo = geode::WeakRef(popup)]() {
			if (Ref<geode::Popup> pp = foo.lock()) {
				if (pp->getTag() != 25052026) Notification::create("You closed the settings popup. No objects were deleted.", NotificationIcon::None, .5f)->show();
			}
		});
		geode::Button* fullyCommitted = geode::Button::createWithLabel(
			"Convert to Layout",
			"goldFont.fnt",
			[foo = this, bar = geode::WeakRef(popup)](geode::Button*) {
				if (Ref<geode::Popup> pp = bar.lock()) {
					pp->setTag(25052026);
					geode::Popup::CloseEvent(pp).send();
					pp->setKeypadEnabled(false);
					pp->setTouchEnabled(false);
					pp->removeFromParent();
				}
				if (foo) {
					foo->deleteAllDeco();
				}
			}
		);
		popup->m_buttonMenu->addChild(fullyCommitted);
		fullyCommitted->setScale(.5f);
		fullyCommitted->setID("layout-mode-fully-committed"_spr);
		fullyCommitted->setPosition({popup->m_mainLayer->getContentWidth() / 2, 1.f});
		fullyCommitted->runAction(CCRepeatForever::create(CCSequence::create(CCScaleTo::create(1.f, 1.f), CCScaleTo::create(.5f, 1.f), nullptr)));
	}
};