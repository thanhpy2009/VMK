/*
 * SPDX-FileCopyrightText: 2022-2022 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */

#include "bamboo.h"
#include "bambooconfig.h"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <fcitx-config/iniparser.h>
#include <fcitx-config/rawconfig.h>
#include <fcitx-utils/capabilityflags.h>
#include <fcitx-utils/charutils.h>
#include <fcitx-utils/i18n.h>
#include <fcitx-utils/keysym.h>
#include <fcitx-utils/log.h>
#include <fcitx-utils/macros.h>
#include <fcitx-utils/misc.h>
#include <fcitx-utils/standardpaths.h>
#include <fcitx-utils/stringutils.h>
#include <fcitx-utils/textformatflags.h>
#include <fcitx-utils/utf8.h>
#include <fcitx/action.h>
#include <fcitx/addoninstance.h>
#include <fcitx/event.h>
#include <fcitx/inputcontext.h>
#include <fcitx/inputcontextmanager.h>
#include <fcitx/inputmethodentry.h>
#include <fcitx/inputpanel.h>
#include <fcitx/menu.h>
#include <fcitx/statusarea.h>
#include <fcitx/text.h>
#include <fcitx/userinterface.h>
#include <fcitx/userinterfacemanager.h>
#include <fcntl.h>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace fcitx {

namespace {

constexpr std::string_view MacroPrefix = "macro/";
constexpr std::string_view InputMethodActionPrefix = "bamboo-input-method-";
constexpr std::string_view CharsetActionPrefix = "bamboo-charset-";
const std::string CustomKeymapFile = "conf/bamboo-custom-keymap.conf";

FCITX_DEFINE_LOG_CATEGORY(bamboo, "bamboo");

std::string macroFile(std::string_view imName) {
    return stringutils::concat("conf/bamboo-macro-", imName, ".conf");
}

uintptr_t newMacroTable(const BambooMacroTable &macroTable) {
    std::vector<char *> charArray;
    RawConfig r;
    macroTable.save(r);
    for (const auto &keymap : *macroTable.macros) {
        charArray.push_back(const_cast<char *>(keymap.key->data()));
        charArray.push_back(const_cast<char *>(keymap.value->data()));
    }
    charArray.push_back(nullptr);
    return NewMacroTable(charArray.data());
}

std::vector<std::string> convertToStringList(char **array) {
    std::vector<std::string> result;
    for (int i = 0; array[i]; i++) {
        result.push_back(array[i]);
        free(array[i]);
    }
    free(array);
    return result;
}

} // namespace

#define FCITX_BAMBOO_DEBUG() FCITX_LOGC(bamboo, Debug)

class BambooState final : public InputContextProperty {
public:
    BambooState(BambooEngine *engine, InputContext *ic)
        : engine_(engine), ic_(ic) {
        setEngine();
    }

    ~BambooState() {}

    void setEngine() {
        bambooEngine_.reset();

        if (*engine_->config().inputMethod == "Custom") {
            std::vector<char *> charArray;
            for (const auto &keymap : *engine_->customKeymap().customKeymap) {
                charArray.push_back(const_cast<char *>(keymap.key->data()));
                FCITX_INFO() << charArray.back();
                charArray.push_back(const_cast<char *>(keymap.value->data()));
                FCITX_INFO() << charArray.back();
            }
            charArray.push_back(nullptr);
            bambooEngine_.reset(NewCustomEngine(charArray.data(),
                                                engine_->dictionary(),
                                                engine_->macroTable()));
        } else {
            bambooEngine_.reset(NewEngine(engine_->config().inputMethod->data(),
                                          engine_->dictionary(),
                                          engine_->macroTable()));
        }
        setOption();
    }

    void setOption() {
        if (!bambooEngine_) {
            return;
        }
        FcitxBambooEngineOption option = {
            .autoNonVnRestore = *engine_->config().autoNonVnRestore,
            .ddFreeStyle = true,
            .macroEnabled = *engine_->config().macro,
            .autoCapitalizeMacro = *engine_->config().capitalizeMacro,
            .spellCheckWithDicts = *engine_->config().spellCheck,
            .outputCharset = engine_->config().outputCharset->data(),
            .modernStyle = *engine_->config().modernStyle,
            .freeMarking = *engine_->config().freeMarking,
        };
        EngineSetOption(bambooEngine_.handle(), &option);
    }

    void keyEvent(KeyEvent &keyEvent) {
        if (!bambooEngine_) {
            return;
        }
        // Ignore all key release.
        if (keyEvent.isRelease()) {
            return;
        }
        if (keyEvent.rawKey().check(FcitxKey_Shift_L) ||
            keyEvent.rawKey().check(FcitxKey_Shift_R)) {
            return;
        }

        if (keyEvent.key().checkKeyList(*engine_->config().restoreKeyStroke)) {
            EngineSetRestoreKeyStroke(bambooEngine_.handle());
            keyEvent.filterAndAccept();
            return;
        }

        if (EngineProcessKeyEvent(bambooEngine_.handle(),
                                  keyEvent.rawKey().sym(),
                                  keyEvent.rawKey().states())) {
            keyEvent.filterAndAccept();
        }

        if (char *commit = EnginePullCommit(bambooEngine_.handle())) {
            if (commit[0]) {
                ic_->commitString(commit);
            }
            free(commit);
        }

        ic_->inputPanel().reset();
        UniqueCPtr<char> preedit(EnginePullPreedit(bambooEngine_.handle()));
        if (preedit && preedit.get()[0]) {
            std::string_view preeditView = preedit.get();
            Text text;
            TextFormatFlags format;
            if (ic_->capabilityFlags().test(CapabilityFlag::Preedit) &&
                *engine_->config().displayUnderline) {
                format = TextFormatFlag::Underline;
            }
            if (utf8::validate(preeditView)) {
                text.append(std::string(preeditView), format);
            }
            text.setCursor(text.textLength());

            if (ic_->capabilityFlags().test(CapabilityFlag::Preedit)) {
                ic_->inputPanel().setClientPreedit(text);
            } else {
                ic_->inputPanel().setPreedit(text);
            }
        }
        ic_->updatePreedit();
        ic_->updateUserInterface(UserInterfaceComponent::InputPanel);
    }

    void reset() {
        ic_->inputPanel().reset();
        if (bambooEngine_) {
            ResetEngine(bambooEngine_.handle());
        }
        ic_->updateUserInterface(UserInterfaceComponent::InputPanel);
        ic_->updatePreedit();
    }

    void commitBuffer() {
        ic_->inputPanel().reset();
        if (bambooEngine_) {
            // The reason that we do not commit here is we want to force the
            // behavior. When client get unfocused, the framework will try to
            // commit the string.
            EngineCommitPreedit(bambooEngine_.handle());
            UniqueCPtr<char> commit(EnginePullCommit(bambooEngine_.handle()));
            if (commit && commit.get()[0]) {
                ic_->commitString(commit.get());
            }
        }
        ic_->updateUserInterface(UserInterfaceComponent::InputPanel);
        ic_->updatePreedit();
    }

private:
    BambooEngine *engine_;
    InputContext *ic_;
    CGoObject bambooEngine_;
};

BambooEngine::BambooEngine(Instance *instance)
    : instance_(instance), factory_([this](InputContext &ic) {
          return new BambooState(this, &ic);
      }) {
    Init();
    {
        auto imNames = convertToStringList(GetInputMethodNames());
        imNames.push_back("Custom");
        imNames_ = std::move(imNames);
    }
    if (std::find(imNames_.begin(), imNames_.end(), "Telex") ==
        imNames_.end()) {
        throw std::runtime_error("Failed to find required input method Telex");
    }
    FCITX_BAMBOO_DEBUG() << "Supported input methods: " << imNames_;
    config_.inputMethod.annotation().setList(imNames_);

    auto fd = StandardPaths::global().open(StandardPathsType::PkgData,
                                           "bamboo/vietnamese.cm.dict");
    if (!fd.isValid()) {
        throw std::runtime_error("Failed to load dictionary");
    }
    dictionary_.reset(NewDictionary(fd.release()));

    auto &uiManager = instance_->userInterfaceManager();
    inputMethodAction_ = std::make_unique<SimpleAction>();
    inputMethodAction_->setIcon("document-edit");
    inputMethodAction_->setShortText(_("Input Method"));
    uiManager.registerAction("bamboo-input-method", inputMethodAction_.get());

    inputMethodMenu_ = std::make_unique<Menu>();
    inputMethodAction_->setMenu(inputMethodMenu_.get());
    for (const auto &imName : imNames_) {
        inputMethodSubAction_.emplace_back(std::make_unique<SimpleAction>());
        auto *action = inputMethodSubAction_.back().get();
        action->setShortText(imName);
        action->setCheckable(true);
        uiManager.registerAction(
            stringutils::concat(InputMethodActionPrefix, imName), action);
        connections_.emplace_back(action->connect<SimpleAction::Activated>(
            [this, imName](InputContext *ic) {
                if (config_.inputMethod.value() == imName) {
                    return;
                }
                config_.inputMethod.setValue(imName);
                saveConfig();
                refreshEngine();
                updateInputMethodAction(ic);
            }));

        inputMethodMenu_->addAction(action);
    }

    charsetAction_ = std::make_unique<SimpleAction>();
    charsetAction_->setShortText(_("Output charset"));
    charsetAction_->setIcon("character-set");
    uiManager.registerAction("bamboo-charset", charsetAction_.get());
    charsetMenu_ = std::make_unique<Menu>();
    charsetAction_->setMenu(charsetMenu_.get());

    auto charsets = convertToStringList(GetCharsetNames());
    for (const auto &charset : charsets) {
        charsetSubAction_.emplace_back(std::make_unique<SimpleAction>());
        auto *action = charsetSubAction_.back().get();
        action->setShortText(charset);
        action->setCheckable(true);
        connections_.emplace_back(action->connect<SimpleAction::Activated>(
            [this, charset](InputContext *ic) {
                if (config_.outputCharset.value() == charset) {
                    return;
                }
                config_.outputCharset.setValue(charset);
                saveConfig();
                refreshEngine();
                updateCharsetAction(ic);
            }));
        uiManager.registerAction(
            stringutils::concat(CharsetActionPrefix, charset), action);
        charsetMenu_->addAction(action);
    }
    config_.outputCharset.annotation().setList(charsets);

    spellCheckAction_ = std::make_unique<SimpleAction>();
    spellCheckAction_->setLongText(_("Spell check"));
    spellCheckAction_->setIcon("tools-check-spelling");
    connections_.emplace_back(
        spellCheckAction_->connect<SimpleAction::Activated>(
            [this](InputContext *ic) {
                config_.spellCheck.setValue(!*config_.spellCheck);
                saveConfig();
                refreshOption();
                updateSpellAction(ic);
            }));
    uiManager.registerAction("bamboo-spell-check", spellCheckAction_.get());
    macroAction_ = std::make_unique<SimpleAction>();
    macroAction_->setLongText(_("Macro"));
    macroAction_->setIcon("edit-find");
    connections_.emplace_back(macroAction_->connect<SimpleAction::Activated>(
        [this](InputContext *ic) {
            config_.macro.setValue(!*config_.macro);
            saveConfig();
            refreshOption();
            updateMacroAction(ic);
        }));
    uiManager.registerAction("bamboo-macro", macroAction_.get());

    reloadConfig();
    instance_->inputContextManager().registerProperty("bambooState", &factory_);
}

void BambooEngine::reloadConfig() {
    readAsIni(config_, "conf/bamboo.conf");
    readAsIni(customKeymap_, CustomKeymapFile);
    for (const auto &imName : imNames_) {
        auto &table = macroTables_[imName];
        readAsIni(table, macroFile(imName));
        macroTableObject_[imName].reset(newMacroTable(table));
    }

    populateConfig();
}

const Configuration *BambooEngine::getSubConfig(const std::string &path) const {
    if (path == "custom_keymap") {
        return &customKeymap_;
    }
    if (path.starts_with(MacroPrefix)) {
        const auto imName = path.substr(MacroPrefix.size());
        if (auto iter = macroTables_.find(imName); iter != macroTables_.end()) {
            return &iter->second;
        }
        return nullptr;
    }
    return nullptr;
}

void BambooEngine::setConfig(const RawConfig &config) {
    config_.load(config, true);
    saveConfig();
    populateConfig();
}

void BambooEngine::populateConfig() {
    refreshEngine();
    refreshOption();
    updateMacroAction(nullptr);
    updateSpellAction(nullptr);
    updateInputMethodAction(nullptr);
    updateCharsetAction(nullptr);
}

void BambooEngine::setSubConfig(const std::string &path,
                                const RawConfig &config) {
    if (path == "custom_keymap") {
        customKeymap_.load(config, true);
        safeSaveAsIni(customKeymap_, CustomKeymapFile);
        refreshEngine();
    } else if (path.starts_with(MacroPrefix)) {
        const auto imName = path.substr(MacroPrefix.size());
        if (auto iter = macroTables_.find(imName); iter != macroTables_.end()) {
            iter->second.load(config, true);
            safeSaveAsIni(iter->second, macroFile(imName));
            macroTableObject_[imName].reset(newMacroTable(iter->second));
            refreshEngine();
        }
    }
}

std::string BambooEngine::subMode(const fcitx::InputMethodEntry & /*entry*/,
                                  fcitx::InputContext & /*inputContext*/) {
    return *config_.inputMethod;
}

void BambooEngine::activate(const InputMethodEntry &entry,
                            InputContextEvent &event) {
    FCITX_UNUSED(entry);
    FCITX_UNUSED(event);
    auto &statusArea = event.inputContext()->statusArea();

    updateMacroAction(event.inputContext());
    updateSpellAction(event.inputContext());
    updateInputMethodAction(event.inputContext());
    updateCharsetAction(event.inputContext());

    statusArea.addAction(StatusGroup::InputMethod, inputMethodAction_.get());
    statusArea.addAction(StatusGroup::InputMethod, charsetAction_.get());
    statusArea.addAction(StatusGroup::InputMethod, spellCheckAction_.get());
    statusArea.addAction(StatusGroup::InputMethod, macroAction_.get());
}

void BambooEngine::deactivate(const InputMethodEntry &entry,
                              InputContextEvent &event) {
    FCITX_UNUSED(entry);
    auto *state = event.inputContext()->propertyFor(&factory_);
    if (event.type() != EventType::InputContextFocusOut) {
        state->commitBuffer();
    } else {
        state->reset();
    }
}

void BambooEngine::keyEvent(const InputMethodEntry &entry, KeyEvent &keyEvent) {
    FCITX_UNUSED(entry);
    auto *state = keyEvent.inputContext()->propertyFor(&factory_);

    state->keyEvent(keyEvent);
}

void BambooEngine::reset(const InputMethodEntry &entry,
                         InputContextEvent &event) {
    FCITX_UNUSED(entry);
    auto *state = event.inputContext()->propertyFor(&factory_);
    state->reset();
}

void BambooEngine::refreshEngine() {
    FCITX_BAMBOO_DEBUG() << "Refresh engine";
    if (!factory_.registered()) {
        return;
    }

    instance_->inputContextManager().foreach([this](InputContext *ic) {
        auto *state = ic->propertyFor(&factory_);
        state->setEngine();
        if (ic->hasFocus()) {
            state->reset();
        }
        return true;
    });
}

void BambooEngine::refreshOption() {
    if (!factory_.registered()) {
        return;
    }
    instance_->inputContextManager().foreach([this](InputContext *ic) {
        auto *state = ic->propertyFor(&factory_);
        state->setOption();
        if (ic->hasFocus()) {
            state->reset();
        }
        return true;
    });
}

void BambooEngine::updateSpellAction(InputContext *ic) {
    spellCheckAction_->setChecked(*config_.spellCheck);
    spellCheckAction_->setShortText(*config_.spellCheck
                                        ? _("Spell Check Enabled")
                                        : _("Spell Check Disabled"));
    if (ic) {
        spellCheckAction_->update(ic);
    }
}

void BambooEngine::updateMacroAction(InputContext *ic) {
    macroAction_->setChecked(*config_.macro);
    macroAction_->setShortText(*config_.macro ? _("Macro Enabled")
                                              : _("Macro Disabled"));
    if (ic) {
        macroAction_->update(ic);
    }
}

void BambooEngine::updateInputMethodAction(InputContext *ic) {
    auto name =
        stringutils::concat(InputMethodActionPrefix, *config_.inputMethod);
    for (const auto &action : inputMethodSubAction_) {
        action->setChecked(action->name() == name);
        if (ic) {
            action->update(ic);
        }
    }
}

void BambooEngine::updateCharsetAction(InputContext *ic) {
    auto name =
        stringutils::concat(CharsetActionPrefix, *config_.outputCharset);
    for (const auto &action : charsetSubAction_) {
        action->setChecked(action->name() == name);
        if (ic) {
            action->update(ic);
        }
    }
}

} // namespace fcitx

FCITX_ADDON_FACTORY_V2(bamboo, fcitx::BambooFactory)
