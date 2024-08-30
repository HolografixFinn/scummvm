/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

namespace Cometengine {


	static const PlainGameDescriptor cometGames[] = {
		{"comet", "Shadow of the comet"},
		{"museum", "Lovecraft Museum"},
		{0, 0} };

	static const CometGameDescription gameDescriptions[] = {
		// Comet PC version
		{
		{
			"comet",
			"English Floppy Version",
			AD_ENTRY2s("E.CC4", 0, 120789, "CUDENGL.CC1", 0, 64401),
			Common::EN_ANY,
			Common::kPlatformDOS,
			ADGF_NO_FLAGS,
			GUIO1(GUIO_NONE)}
			,
		GType_COMET,
		0
		},
		{
		{"comet",
		 "Italian Floppy Version",
		 AD_ENTRY2s("I.CC4", 0, 122744, "CTUI.CC1", 0, 64557),
		 Common::IT_ITA,
		 Common::kPlatformDOS,
		 ADGF_NO_FLAGS,
		 GUIO1(GUIO_NONE)},
		GType_COMET,
		0
		},
		{
		{"comet",
		 "French Floppy Version",
		 AD_ENTRY2s("T.CC4", 0, 131545, "CTUF.CC1", 0, 64549),
		 Common::FR_FRA,
		 Common::kPlatformDOS,
		 ADGF_NO_FLAGS,
		 GUIO1(GUIO_NONE)},
			GType_COMET,
			0},
		{
		{"comet",
		 "Spanish Floppy Version",
		 AD_ENTRY2s("S.CC4", 0, 180367, "CTUS.CC1", 0, 64513),
		 Common::ES_ESP,
		 Common::kPlatformDOS,
		 ADGF_NO_FLAGS,
		 GUIO1(GUIO_NONE)},
		GType_COMET,
		0},
		{
		{"comet",
		 "Multilanguage CD Version",
		 AD_ENTRY2s("SHADO_CD.CFG", 0, -1, "SHADOW.EXE",0,-1),
		 Common::EN_ANY,
		 Common::kPlatformDOS,
		 ADGF_CD | ADGF_DROPLANGUAGE,
		 GUIO1(GUIO_NONE)},
		GType_COMET,
		GF_CD}
		,
		{
		{"comet",
		 "Demo Version",
		 AD_ENTRY2s("BA.EXE", 0, -1, "BA2.EXE", 0, -1),
		 Common::EN_ANY,
		 Common::kPlatformDOS,
		 ADGF_NO_FLAGS,
		 GUIO1(GUIO_NONE)},
		GType_COMET,
		GF_DEMO},
		{
		{"museum",
		 "Lovecraft Museum CD Version",
		 AD_ENTRY2s("SHADO_CD.CFG", 0, -1, "MUSEUM.EXE", 0, -1),
		 Common::EN_ANY,
		 Common::kPlatformDOS,
		 ADGF_CD | ADGF_DROPLANGUAGE,
		 GUIO1(GUIO_NONE)},
		GType_MUSEUM,
		GF_CD},

		AD_TABLE_END_MARKER };

} // End of namespace Cometengine
