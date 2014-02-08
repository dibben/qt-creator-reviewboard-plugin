/**************************************************************************
**
** Copyright (c) 2013 David Dibben (http://www.codegardening.com/)
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of
** this software and associated documentation files (the "Software"), to deal in
** the Software without restriction, including without limitation the rights to
** use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
** the Software, and to permit persons to whom the Software is furnished to do so,
** subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
** FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
** COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
** IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************/


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

	bool		initialize(const QStringList &arguments, QString *error_message);
	void		extensionsInitialized();
	void		shutdown();

private slots:

	void		updateReview();

	void		projectStatus();
	void		updateSettings();

private:

	QString		projectTopLevel();
	void		openUrl(const QString& data);
	void		submitReview(const QString &workingDir, const Repository& rep, const QStringList& files);

	void		svnStatus(const QString &workingDir, const QStringList &relativePaths);

	 QSharedPointer<ReviewBoardSettings>	m_Settings;
	 QSharedPointer<ReviewBoardServer>	m_Server;


};

} // namespace Internal
} // namespace ReviewBoard

#endif // REVIEWBOARDPLUGIN_H
