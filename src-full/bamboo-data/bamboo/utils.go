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
	"unicode"
)

const (
	VnCaseAllSmall uint8 = iota + 1
	VnCaseAllCapital
	VnCaseNoChange
)

func determineMacroCase(str string) uint8 {
	var chars = []rune(str)
	if unicode.IsLower(chars[0]) {
		return VnCaseAllSmall
	} else {
		for _, c := range chars[1:] {
			if unicode.IsLower(c) {
				return VnCaseNoChange
			}
			if bamboo.IsWordBreakSymbol(c) {
				return VnCaseNoChange
			}
		}
	}
	return VnCaseAllCapital
}

func inKeyList(list []rune, key rune) bool {
	for _, s := range list {
		if s == key {
			return true
		}
	}
	return false
}

// Backport missing function in bamboo-core
func (e *MacroTable) HasPrefix(key string) bool {
	if e.mTable[key] != "" {
		return true
	}
	for k := range e.mTable {
		if strings.HasPrefix(k, key) {
			return true
		}
	}
	return false
}
