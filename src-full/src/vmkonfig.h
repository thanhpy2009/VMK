/*
 * SPDX-FileCopyrightText: 2025 Võ Ngô Hoàng Thành <thanhpy2009@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef _FCITX5_vmk_vmkCONFIG_H_ // ĐỔI TÊN HEADER GUARD
#define _FCITX5_vmk_vmkCONFIG_H_

#include <fcitx-config/configuration.h>
#include <fcitx-config/option.h>
#include <fcitx-utils/i18n.h>
#include <fcitx-utils/stringutils.h>
#include <algorithm> // Thêm thư viện này để đảm bảo std::find hoạt động
#include <string>
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

struct InputMethodAnnotation : public StringListAnnotation {};
struct ModeListAnnotation : public StringListAnnotation {
    ModeListAnnotation() { list_ = {"vmk1","vmk2","vmk1hc","vmkpre"}; }};

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
    void dumpDescription(RawConfig &) const {}

protected:
    const InputMethodOption *option_;
};

// ĐỔI TÊN CONFIGURATIONS
FCITX_CONFIGURATION(vmkKeymap,
                    Option<std::string> key{this, "Key", _("Key"), ""};
                    Option<std::string> value{this, "Value", _("Value"), ""};);

FCITX_CONFIGURATION(
    vmkMacroTable,
    OptionWithAnnotation<std::vector<vmkKeymap>, ListDisplayOptionAnnotation>
        macros{this,
               "Macro",
               _("Macro"),
               {},
               {}, // BẮT BUỘC: Thêm Constrain mặc định
               {}, // BẮT BUỘC: Thêm Annotation mặc định
               ListDisplayOptionAnnotation("Key")};);

FCITX_CONFIGURATION(
    vmkCustomKeymap,
    OptionWithAnnotation<std::vector<vmkKeymap>, ListDisplayOptionAnnotation>
        customKeymap{this,
                     "CustomKeymap",
                     _("Custom Keymap"),
                     {},
                     {}, // BẮT BUỘC: Thêm Constrain mặc định
                     {}, // BẮT BUỘC: Thêm Annotation mặc định
                     ListDisplayOptionAnnotation("Key")};);

// ĐỔI TÊN CONFIGURATION CHÍNH

FCITX_CONFIGURATION(
    vmkConfig, 

OptionWithAnnotation<std::string, ModeListAnnotation> mode{
        this,
        "Mode",
        _("Chế độ"),
        "vmk1",
        {},
        {},
        ModeListAnnotation()
    };
    //KeyListOption restoreKeyStroke{this,
                                   //"RestoreKeyStroke",
                                 //  _("Restore Key Stroke"),
                                 //  {},
                                //   KeyListConstrain()};
    Option<std::string, InputMethodConstrain, DefaultMarshaller<std::string>,
           InputMethodAnnotation>
        inputMethod{this, "InputMethod", _("Input Method"), "Telex",
                    InputMethodConstrain(&inputMethod),
                    // BẮT BUỘC SỬA: Thêm đối số DefaultMarshaller (Marshaller mặc định)
                    {}, 
                    // BẮT BUỘC: Thêm đối số Annotation bị thiếu
                    InputMethodAnnotation()}; 
OptionWithAnnotation<std::string, StringListAnnotation> outputCharset{
        this, "OutputCharset", _("Output Charset"), "Unicode",
        // BẮT BUỘC SỬA: OptionWithAnnotation cần đủ 7 đối số trong trường hợp này:
        // 5. Constrain (Mặc định: NoConstrain)
        {}, 
        // 6. Marshaller (Mặc định: DefaultMarshaller)
        {}, 
        // 7. Annotation
        StringListAnnotation()}; 
 //   Option<bool> spellCheck{this, "SpellCheck", _("Enable spell check"), true};
   // Option<bool> macro{this, "Macro", _("Enable Macro"), true};
  //  Option<bool> capitalizeMacro{this, "CapitalizeMacro", _("Capitalize Macro"),
                        //         true};
    //Option<bool> autoNonVnRestore{this, "AutoNonVnRestore",
                                 // _("Auto restore keys with invalid words"),
                                 // true};
  // Option<bool> modernStyle{this, "ModernStyle",
                      //   _("Use oà, _uý (instead of òa, úy)"), false};
  //  Option<bool> freeMarking{this, "FreeMarking",
                            // _("Allow type with more freedom"), true};
    // Option<bool> displayUnderline{this, "DisplayUnderline",
                              //    _("Underline the preedit text"), true};
   // SubConfigOption custumKeymap{this, "CustomKeymap", _("Custom Keymap"),
                                 // ĐỔI TÊN PATH
                               //  "fcitx://config/addon/vmk/custom_keymap"};
    
Option<bool> gemini{this, "Gemini", _("Gemini-fix"), false};
Option<bool> chromex11{this, "ChromeX11", _("Chrome X11 Fix"), false};
                           );
} // namespace fcitx

#endif
