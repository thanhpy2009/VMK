/*
 * SPDX-FileCopyrightText: 2025 CSSlayer <thanhpy2009@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0
 *
 */
#ifndef _FCITX5_vmk_vmkCONFIG_H_
#define _FCITX5_vmk_vmkCONFIG_H_

#include <fcitx-config/configuration.h>
#include <fcitx-config/option.h>
#include <fcitx-utils/i18n.h>
#include <fcitx-utils/stringutils.h>
#include <algorithm>
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
    ModeListAnnotation() { list_ = {"vmk1", "vmk2", "vmk1hc", "vmkpre"}; }
};

struct InputMethodConstrain {
    using Type = std::string;

    InputMethodConstrain(const InputMethodOption *option) : option_(option) {}

    bool check(const std::string &name) const {
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
               {},
               {},
               ListDisplayOptionAnnotation("Key")};);

FCITX_CONFIGURATION(
    vmkCustomKeymap,
    OptionWithAnnotation<std::vector<vmkKeymap>, ListDisplayOptionAnnotation>
        customKeymap{this,
                     "CustomKeymap",
                     _("Custom Keymap"),
                     {},
                     {},
                     {},
                     ListDisplayOptionAnnotation("Key")};);

FCITX_CONFIGURATION(
    vmkConfig,

    OptionWithAnnotation<std::string, ModeListAnnotation> mode{
        this, "Mode", _("Chế độ"), "vmk1", {}, {}, ModeListAnnotation()};

    Option<std::string, InputMethodConstrain, DefaultMarshaller<std::string>,
           InputMethodAnnotation>
        inputMethod{this,
                    "InputMethod",
                    _("Kiểu gõ"),
                    "Telex",
                    InputMethodConstrain(&inputMethod),
                    {},
                    InputMethodAnnotation()};

    OptionWithAnnotation<std::string, StringListAnnotation> outputCharset{
        this, "OutputCharset", _("Bảng mã"), "Unicode", {}, {},
        StringListAnnotation()};

    // ---- Gõ lẫn tiếng Anh / tiếng Việt (hiện trên fcitx5-configtool & sconfig) ----

    /// Khi bật: từ không phải tiếng Việt (vd. clear, with, class) được giữ nguyên
    /// keystroke, không bị Telex/VNI “xử” thành chữ có dấu.
    Option<bool> autoNonVnRestore{
        this, "AutoNonVnRestore",
        _("Giữ từ tiếng Anh (auto restore)"), true};

    /// Khi bật: đối chiếu từ điển bamboo trước khi chốt từ tiếng Việt.
    /// Kết hợp với “Giữ từ tiếng Anh” để giảm dính dấu khi code/chat EN.
    Option<bool> spellCheckWithDicts{
        this, "SpellCheckWithDicts",
        _("Kiểm tra từ điển tiếng Việt"), true};

    /// Free-marking kiểu UniKey: đặt dấu linh hoạt hơn trong từ tiếng Việt.
    Option<bool> freeMarking{
        this, "FreeMarking",
        _("Gõ dấu tự do (free marking)"), true};

    /// Dùng danh sách từ tiếng Anh (builtin + ~/.config/fcitx5/vmk-english-words.txt)
    /// để ưu tiên không bỏ dấu các từ phổ biến (clear, window, ...).
    Option<bool> englishWordList{
        this, "EnglishWordList",
        _("Danh sách từ tiếng Anh (whitelist)"), true};

    // ---- Fix app ----
    Option<bool> gemini{this, "Gemini", _("Sửa Gemini / Chrome RichText"),
                        false};
    Option<bool> chromex11{this, "ChromeX11", _("Sửa Chrome trên X11"), false};
);

} // namespace fcitx

#endif
