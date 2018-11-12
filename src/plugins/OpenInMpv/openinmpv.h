/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2014  David Rosca <nowrep@gmail.com>
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
* ============================================================ */
#ifndef OPENINMPVPLUGIN_H
#define OPENINMPVPLUGIN_H

// Include plugininterface.h for your version of Falkon
#include "plugininterface.h"

#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QPointer>

class BrowserWindow;
class OpenInMpvIcon;

class OpenInMpv : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "Falkon.Browser.plugin.OpenInMpv" FILE "openinmpv.json")

public:
	explicit OpenInMpv();

	void init(InitState state, const QString &settingsPath) override;
	void unload() override;
	bool testPlugin() override;

	void populateWebViewMenu(QMenu *menu, WebView *view, const WebHitTestResult &r) override;

public slots:
    void mainWindowCreated(BrowserWindow *window);
    void mainWindowDeleted(BrowserWindow *window);
private Q_SLOTS:
	void actionSlot();
    void actionIconSlot();
private:

	QString m_settingsPath;
    OpenInMpvIcon* m_icon;
    void work(const QUrl);
};

#endif // OPENINMPVPLUGIN_H
