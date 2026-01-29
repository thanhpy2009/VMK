/*
 * SPDX-FileCopyrightText: 2018 Luong Thanh Lam <ltlam93@gmail.com>
 * SPDX-FileCopyrightText: 2022-2022 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
package main

import (
	"bamboo-core"
	"strings"
)

type FcitxBambooEngine struct {
	preeditor               bamboo.IEngine
	macroTable              *MacroTable
	dictionary              map[string]bool
	autoNonVnRestore        bool
	ddFreeStyle             bool
	macroEnabled            bool
	autoCapitalizeMacro     bool
	lastKeyWithShift        bool
	spellCheckWithDicts     bool
	preeditText             string
	commitText              string
	shouldRestoreKeyStrokes bool
	outputCharset           string
}

const (
	FcitxShiftMask   = 1 << 0
	FcitxLockMask    = 1 << 1
	FcitxControlMask = 1 << 2
	FcitxMod1Mask    = 1 << 3

	/* The next few modifiers are used by XKB so we skip to the end.
	 * Bits 15 - 23 are currently unused. Bit 29 is used internally.
	 */

	FcitxForwardMask = 1 << 25
	FcitxIgnoredMask = FcitxForwardMask

	FcitxSuperMask = 1 << 26
	FcitxHyperMask = 1 << 27
	FcitxMetaMask  = 1 << 28
)
const (
	FcitxBackSpace = 0xff08
	FcitxSpace     = 0x020
	FcitxTab       = 0xff09
)

func (e *FcitxBambooEngine) preeditProcessKeyEvent(keyVal uint32, state uint32) bool {
	var rawKeyLen = e.getRawKeyLen()
	var keyRune = rune(keyVal)
	var oldText = e.getPreeditString()
	defer e.updateLastKeyWithShift(keyVal, state)

	if !e.shouldRestoreKeyStrokes {
		if !e.preeditor.CanProcessKey(keyRune) && rawKeyLen == 0 && !e.macroEnabled {
			// don't process special characters if rawKeyLen == 0,
			// workaround for Chrome's address bar and Google SpreadSheets
			return false
		}
	}

	if keyVal == FcitxBackSpace {
		if e.runeCount() == 1 {
			e.commitPreeditAndReset("")
			return true
		}
		if rawKeyLen > 0 {
			e.preeditor.RemoveLastChar(true)
			e.updatePreedit(e.getPreeditString())
			return true
		} else {
			return false
		}
	}
	if keyVal == FcitxTab {
		if ok, macText := e.getMacroText(); ok {
			e.commitPreeditAndReset(macText)
		} else {
			e.commitPreeditAndReset(e.getComposedString(oldText))
			return false
		}
		return true
	}

	newText, isWordBreakRune := e.getCommitText(keyVal, state)
	isPrintableKey := e.isPrintableKey(state, keyVal)
	if isWordBreakRune {
		e.commitPreeditAndReset(newText)
		return isPrintableKey
	}
	e.updatePreedit(newText)
	return isPrintableKey
}

func (e *FcitxBambooEngine) expandMacro(str string) string {
	var macroText = e.macroTable.GetText(str)
	if e.autoCapitalizeMacro {
		switch determineMacroCase(str) {
		case VnCaseAllSmall:
			return strings.ToLower(macroText)
		case VnCaseAllCapital:
			return strings.ToUpper(macroText)
		}
	}
	return macroText
}

func (e *FcitxBambooEngine) updatePreedit(processedStr string) {
	var encodedStr = e.encodeText(processedStr)
	var preeditLen = uint32(len([]rune(encodedStr)))
	if preeditLen == 0 {
		e.preeditText = ""
		e.commitText = ""
		return
	}

	e.preeditText = encodedStr
}

func (e *FcitxBambooEngine) getBambooInputMode() bamboo.Mode {
	if e.shouldFallbackToEnglish(false) {
		return bamboo.EnglishMode
	}
	return bamboo.VietnameseMode
}

func (e *FcitxBambooEngine) shouldFallbackToEnglish(checkVnRune bool) bool {
	if !e.autoNonVnRestore {
		return false
	}
	var vnSeq = e.getProcessedString(bamboo.VietnameseMode | bamboo.LowerCase)
	var vnRunes = []rune(vnSeq)
	if len(vnRunes) == 0 {
		return false
	}
	if ok, _ := e.getMacroText(); ok {
		return false
	}
	// we want to allow dd even in non-vn sequence, because dd is used a lot in abbreviation
	if e.ddFreeStyle && !bamboo.HasAnyVietnameseVower(vnSeq) &&
		(vnRunes[len(vnRunes)-1] == 'd' || strings.ContainsRune(vnSeq, 'đ')) {
		return false
	}
	if checkVnRune && !bamboo.HasAnyVietnameseRune(vnSeq) {
		return false
	}
	return !e.preeditor.IsValid(false)
}

func (e *FcitxBambooEngine) mustFallbackToEnglish() bool {
	if !e.autoNonVnRestore {
		return false
	}
	var vnSeq = e.getProcessedString(bamboo.VietnameseMode | bamboo.LowerCase)
	var vnRunes = []rune(vnSeq)
	if len(vnRunes) == 0 {
		return false
	}
	// we want to allow dd even in non-vn sequence, because dd is used a lot in abbreviation
	if e.ddFreeStyle && strings.ContainsRune(vnSeq, 'đ') {
		return false
	}
	if e.spellCheckWithDicts {
		return !e.dictionary[vnSeq]
	}
	return !e.preeditor.IsValid(true)
}

func (e *FcitxBambooEngine) getComposedString(oldText string) string {
	if bamboo.HasAnyVietnameseRune(oldText) && e.mustFallbackToEnglish() {
		return e.getProcessedString(bamboo.EnglishMode)
	}
	return oldText
}

func (e *FcitxBambooEngine) encodeText(text string) string {
	return bamboo.Encode(e.outputCharset, text)
}

func (e *FcitxBambooEngine) getProcessedString(mode bamboo.Mode) string {
	return e.preeditor.GetProcessedString(mode)
}

func (e *FcitxBambooEngine) getPreeditString() string {
	if e.macroEnabled {
		return e.getProcessedString(bamboo.PunctuationMode)
	}
	if e.shouldFallbackToEnglish(true) {
		return e.getProcessedString(bamboo.EnglishMode)
	}
	return e.getProcessedString(bamboo.VietnameseMode)
}

func (e *FcitxBambooEngine) commitPreeditAndReset(s string) {
	e.commitText = s
	e.preeditText = ""
	e.preeditor.Reset()
}
