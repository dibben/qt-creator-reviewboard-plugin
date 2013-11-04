/**************************************************************************
**
** This file is part of ReviewBoard plugin for Qt Creator
**
** Copyright (c) 2010 David Dibben
**
** This plugin is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as
** published by the Free Software Foundation, either version 2.1
** of the License, or (at your option) any later version.
**
** This plugin is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with REVIEWBOARD.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef REVIEWBOARDCONSTANTS_H
#define REVIEWBOARDCONSTANTS_H


namespace ReviewBoard {
namespace Constants {

const char * const M_TOOLS_REVIEWBOARD  = "ReviewBoard.Tools.Menu";
const char * const CREATE_REVIEW = "ReviewBoard.CreateReview";
const char * const UPDATE_REVIEW = "ReviewBoard.UpdateReview";

// QSettings keys for use by the "New Class" wizards.
const char * const REVIEWBOARD_SETTINGSGROUP = "REVIEWBOARD";
//const char * const REVIEWBOARD_AUTHOR = "Author";

const char * const REVIEWBOARD_SETTINGS_ID = QT_TRANSLATE_NOOP("ReviewBoard", "Review Board Options");
const char * const REVIEWBOARD_SETTINGS_CATEGORY = QT_TRANSLATE_NOOP("ReviewBoard", "Code Review");



} // namespace Constants
} // namespace ReviewBoard

#endif // REVIEWBOARDCONSTANTS_H
