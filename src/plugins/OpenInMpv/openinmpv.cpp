/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2018 David Rosca <nowrep@gmail.com>
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
#include "openinmpv.h"
#include "browserwindow.h"
#include "webview.h"
#include "pluginproxy.h"
#include "mainapplication.h"
#include "sidebar.h"
#include "webhittestresult.h"
#include "../config.h"
#include "desktopfile.h"
#include "navigationbar.h"

#include <QMenu>
#include <QPushButton>
#include <QProcess>

#include "icon.h"

OpenInMpv::OpenInMpv()
    : QObject()
{
    // Don't do anything expensive in constructor!
    // It will be called even if user doesn't have the plugin allowed
}

void OpenInMpv::init(InitState state, const QString &)
{
    m_icon = new OpenInMpvIcon();
    m_icon->setIcon(QIcon(QL1S(":openinmpv/data/icon.svg")));
    connect(m_icon, SIGNAL(clicked(ClickController*)), this, SLOT(actionIconSlot()));
    connect(mApp->plugins(), &PluginProxy::mainWindowCreated, this, &OpenInMpv::mainWindowCreated);
    connect(mApp->plugins(), &PluginProxy::mainWindowDeleted, this, &OpenInMpv::mainWindowDeleted);

    if (state == LateInitState) {
        const auto windows = mApp->windows();
        for (BrowserWindow *window : windows) {
            mainWindowCreated(window);
        }
    }

}

void OpenInMpv::mainWindowCreated(BrowserWindow *window)
{
    window->navigationBar()->addToolButton(m_icon);
}

void OpenInMpv::mainWindowDeleted(BrowserWindow *window)
{
    window->navigationBar()->removeToolButton(m_icon);
}

void OpenInMpv::unload()
{
    const auto windows = mApp->windows();
    for (BrowserWindow *window : windows) {
        mainWindowDeleted(window);
    }
}

bool OpenInMpv::testPlugin()
{
    // This function is called right after init()
    // There should be some testing if plugin is loaded correctly
    // If this function returns false, plugin is automatically unloaded

    return (Qz::VERSION == QLatin1String(FALKON_VERSION));
}

void OpenInMpv::populateWebViewMenu(QMenu* menu, WebView*, const WebHitTestResult &r)
{
    // Called from WebView when creating context menu

    QUrl link;

    if (!r.linkUrl().isEmpty()) {
//        qDebug() << __FUNCTION__ << "linkUrl";
        link = r.linkUrl();
    } else if (!r.mediaUrl().isEmpty()) {
//        qDebug() << __FUNCTION__ << "mediaUrl";
        link = r.mediaUrl();
    }

    if (link.isEmpty()) {
        return;
    }

    menu->addAction(tr("Open In Mpv"), this, SLOT(actionSlot()))->setData(link);
}

void OpenInMpv::work(const QUrl url) {
    QProcess::startDetached("/bin/mpv", QStringList() << url.toString());
}
void OpenInMpv::actionSlot()
{
    const QUrl url = qobject_cast<QAction*>(sender())->data().toUrl();
    work(url);
}

void OpenInMpv::actionIconSlot()
{
    WebView *view = m_icon->webView();
    if (!view) {
        return;
    }

    const QUrl pageUrl = view->url();
    work(pageUrl);
}
