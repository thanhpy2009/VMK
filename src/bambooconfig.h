/*
 * SPDX-FileCopyrightText: 2022-2022 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX5_BAMBOO_BAMBOOCONFIG_H_
#define _FCITX5_BAMBOO_BAMBOOCONFIG_H_

#include <algorithm>
#include <cstddef>
#include <fcitx-config/configuration.h>
#include <fcitx-config/option.h>
#include <fcitx-config/rawconfig.h>
#include <fcitx-utils/i18n.h>
#include <fcitx-utils/stringutils.h>
#include <string>
#include <utility>
#include <vector>

namespace fcitx {

struct InputMethodConstrain;
struct InputMethodAnnotation;
using InputMethodOption =
    Option<std::string, InputMethodConstrain, DefaultMarshaller<std::string>,
           InputMethodAnnotation>;

struct StringListAnnotation : public EnumAnnotation {
    void setList(std::vector<std::string> list) { list_ = std::move(list); }
    const auto &list() { return list_; }
    void dumpDescription(RawConfig &config) const {
        EnumAnnotation::dumpDescription(config);
        for (size_t i = 0; i < list_.size(); i++) {
            config.setValueByPath("Enum/" + std::to_string(i), list_[i]);
        }
    }

protected:
    std::vector<std::string> list_;
};

struct InputMethodAnnotation : public StringListAnnotation {
    void dumpDescription(RawConfig &config) const {
        StringListAnnotation::dumpDescription(config);
        config.setValueByPath("LaunchSubConfig", "True");
        for (size_t i = 0; i < list_.size(); i++) {
            config.setValueByPath(
                "SubConfigPath/" + std::to_string(i),
                stringutils::concat("fcitx://config/addon/bamboo/macro/",
                                    list_[i]));
        }
    }
};

struct InputMethodConstrain {
    using Type = std::string;

    InputMethodConstrain(const InputMethodOption *option) : option_(option) {}

    bool check(const std::string &name) const {
        // Avoid check during initialization
        const auto &list = option_->annotation().list();
        if (list.empty()) {
            return true;
        }
        return std::find(list.begin(), list.end(), name) != list.end();
    }
    void dumpDescription(RawConfig & /*unused*/) const {}

protected:
    const InputMethodOption *option_;
};

FCITX_CONFIGURATION(BambooKeymap,
                    Option<std::string> key{this, "Key", _("Key"), ""};
                    Option<std::string> value{this, "Value", _("Value"), ""};);

FCITX_CONFIGURATION(
    BambooMacroTable,
    OptionWithAnnotation<std::vector<BambooKeymap>, ListDisplayOptionAnnotation>
        macros{this,
               "Macro",
               _("Macro"),
               {},
               {},
               {},
               ListDisplayOptionAnnotation("Key")};);

FCITX_CONFIGURATION(
    BambooCustomKeymap,
    OptionWithAnnotation<std::vector<BambooKeymap>, ListDisplayOptionAnnotation>
        customKeymap{this,
                     "CustomKeymap",
                     _("Custom Keymap"),
                     {},
                     {},
                     {},
                     ListDisplayOptionAnnotation("Key")};);

using InputMethodOption =
    Option<std::string, InputMethodConstrain, DefaultMarshaller<std::string>,
           InputMethodAnnotation>;

FCITX_CONFIGURATION(
    BambooConfig, KeyListOption restoreKeyStroke{this,
                                                 "RestoreKeyStroke",
                                                 _("Restore Key Stroke"),
                                                 {},
                                                 KeyListConstrain()};
    Option<std::string, InputMethodConstrain, DefaultMarshaller<std::string>,
           InputMethodAnnotation>
        inputMethod{this, "InputMethod", _("Input Method"), "Telex",
                    InputMethodConstrain(&inputMethod)};
    OptionWithAnnotation<std::string, StringListAnnotation> outputCharset{
        this, "OutputCharset", _("Output Charset"), "Unicode"};
    Option<bool> spellCheck{this, "SpellCheck", _("Enable spell check"), true};
    Option<bool> macro{this, "Macro", _("Enable Macro"), true};
    Option<bool> capitalizeMacro{this, "CapitalizeMacro", _("Capitalize Macro"),
                                 true};
    Option<bool> autoNonVnRestore{this, "AutoNonVnRestore",
                                  _("Auto restore keys with invalid words"),
                                  true};
    Option<bool> modernStyle{this, "ModernStyle",
                             _("Use oà, _uý (instead of òa, úy)"), false};
    Option<bool> freeMarking{this, "FreeMarking",
                             _("Allow type with more freedom"), true};
    Option<bool> displayUnderline{this, "DisplayUnderline",
                                  _("Underline the preedit text"), true};
    SubConfigOption custumKeymap{this, "CustomKeymap", _("Custom Keymap"),
                                 "fcitx://config/addon/bamboo/custom_keymap"};);
} // namespace fcitx

#endif
