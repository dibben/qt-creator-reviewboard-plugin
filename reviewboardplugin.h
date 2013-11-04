/**************************************************************************
**
** This file is part of ReviewBoard plugin for Qt Creator
**
** Copyright (c) 2010 David Dibben.
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
** along with ReviewBoard.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef REVIEWBOARDPLUGIN_H
#define REVIEWBOARDPLUGIN_H


#include <extensionsystem/iplugin.h>

#include <QSharedPointer>

#include "subversionclient.h"

namespace ReviewBoard {
namespace Internal {

struct ReviewBoardSettings;
class ReviewBoardServer;



class ReviewBoardPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.ReviewBoardPlugin")
public:
	ReviewBoardPlugin();
	~ReviewBoardPlugin();

	bool initialize(const QStringList &arguments, QString *error_message);
	void extensionsInitialized();
	void shutdown();

private slots:

	void		updateReview();

	void		projectStatus();
	void		updateSettings();

private:

	QString		projectTopLevel();
	void				openUrl(const QString& data);
	void				submitReview(const QString &workingDir, const Repository& rep, const QStringList& files);

	void				svnStatus(const QString &workingDir, const QStringList &relativePaths);

	 QSharedPointer<ReviewBoardSettings>	m_Settings;
	 QSharedPointer<ReviewBoardServer>	m_Server;


};

} // namespace Internal
} // namespace ReviewBoard

#endif // REVIEWBOARDPLUGIN_H
