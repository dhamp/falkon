/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2018 Anmol Gautam <tarptaeya@gmail.com>
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
#include "qmlbookmarks.h"
#include "bookmarks.h"

#include <QDebug>

QmlBookmarks::QmlBookmarks(QObject *parent) :
    QObject(parent)
{
    connect(mApp->bookmarks(), &Bookmarks::bookmarkAdded, this, [=](BookmarkItem *item){
        auto treeNode = QmlBookmarkTreeNode::fromBookmarkItem(item);
        emit created(treeNode);
    });

    connect(mApp->bookmarks(), &Bookmarks::bookmarkChanged, this, [=](BookmarkItem *item){
        auto treeNode = QmlBookmarkTreeNode::fromBookmarkItem(item);
        emit changed(treeNode);
    });

    connect(mApp->bookmarks(), &Bookmarks::bookmarkRemoved, this, [=](BookmarkItem *item){
        auto treeNode = QmlBookmarkTreeNode::fromBookmarkItem(item);
        emit removed(treeNode);
    });
}

BookmarkItem *QmlBookmarks::getBookmarkItem(QmlBookmarkTreeNode *treeNode)
{
    auto bookmarks = mApp->bookmarks();
    QList<BookmarkItem*> items;

    if (treeNode->url().isEmpty()) {
        if (isTreeNodeEqualsItem(treeNode, bookmarks->rootItem())) {
            return bookmarks->rootItem();

        } else if (isTreeNodeEqualsItem(treeNode, bookmarks->toolbarFolder())) {
            return bookmarks->toolbarFolder();

        } else if (isTreeNodeEqualsItem(treeNode, bookmarks->menuFolder())) {
            return bookmarks->menuFolder();

        } else if (isTreeNodeEqualsItem(treeNode, bookmarks->unsortedFolder())) {
            return bookmarks->unsortedFolder();
        }

        items = bookmarks->searchBookmarks(treeNode->title());
    } else {
        items = bookmarks->searchBookmarks(QUrl(treeNode->url()));
    }

    for (auto item : items) {
        if (isTreeNodeEqualsItem(treeNode, item)) {
            return item;
        }
    }

    return nullptr;
}

BookmarkItem *QmlBookmarks::getBookmarkItem(QObject *object)
{
    auto treeNode = qobject_cast<QmlBookmarkTreeNode*>(object);
    if (!treeNode) {
        return nullptr;
    }

    auto item = getBookmarkItem(treeNode);
    if (!item || item->urlString() != treeNode->url()) {
        return nullptr;
    }

    return item;
}

bool QmlBookmarks::isBookmarked(const QUrl &url)
{
    return mApp->bookmarks()->isBookmarked(url);
}

QmlBookmarkTreeNode *QmlBookmarks::rootItem() const
{
    return QmlBookmarkTreeNode::fromBookmarkItem(mApp->bookmarks()->rootItem());
}


QmlBookmarkTreeNode *QmlBookmarks::toolbarFolder() const
{
    return QmlBookmarkTreeNode::fromBookmarkItem(mApp->bookmarks()->toolbarFolder());
}


QmlBookmarkTreeNode *QmlBookmarks::menuFolder() const
{
    return QmlBookmarkTreeNode::fromBookmarkItem(mApp->bookmarks()->menuFolder());
}


QmlBookmarkTreeNode *QmlBookmarks::unsortedFolder() const
{
    return QmlBookmarkTreeNode::fromBookmarkItem(mApp->bookmarks()->unsortedFolder());
}


QmlBookmarkTreeNode *QmlBookmarks::lastUsedFolder() const
{
    return QmlBookmarkTreeNode::fromBookmarkItem(mApp->bookmarks()->lastUsedFolder());
}

bool QmlBookmarks::create(const QVariantMap &map)
{
    if (!map["parent"].isValid()) {
        qWarning() << "Unable to create new bookmark:" << "parent not found";
        return false;
    }
    QString title = map["title"].toString();
    QString urlString = map["url"].toString();
    QString description = map["description"].toString();

    BookmarkItem::Type type;
    if (map["type"].isValid()) {
        type = BookmarkItem::Type(map["type"].toInt());
    } else if (urlString.isEmpty()){
        if (!title.isEmpty()) {
            type = BookmarkItem::Folder;
        } else {
            type = BookmarkItem::Invalid;
        }
    } else {
        type = BookmarkItem::Url;
    }

    // FIXME: try removing qvariant_cast
    auto object = qvariant_cast<QObject*>(map["parent"]);
    auto parent = getBookmarkItem(object);
    if (!parent) {
        qWarning() << "Unable to create new bookmark:" << "parent not found";
        return false;
    }
    auto item = new BookmarkItem(type);
    item->setTitle(title);
    item->setUrl(QUrl(urlString));
    item->setDescription(description);
    mApp->bookmarks()->addBookmark(parent, item);
    return true;
}

bool QmlBookmarks::remove(QObject *object)
{
    auto item = getBookmarkItem(object);
    if (!item) {
        qWarning() << "Unable to remove bookmark:" <<"BookmarkItem not found";
        return false;
    }
    mApp->bookmarks()->removeBookmark(item);
    return true;
}

QList<QObject*> QmlBookmarks::search(const QVariantMap &map)
{
    if (!map["query"].isValid() && !map["url"].isValid()) {
        qWarning() << "Unable to search bookmarks";
        return QList<QObject*>();
    }

    QString query = map["query"].toString();
    QString urlString = map["url"].toString();
    QList<BookmarkItem*> items;
    if (urlString.isEmpty()) {
        items = mApp->bookmarks()->searchBookmarks(query);
    } else {
        items = mApp->bookmarks()->searchBookmarks(QUrl(urlString));
    }
    QList<QObject*> ret;
    for (auto item : items) {
        ret.append(QmlBookmarkTreeNode::fromBookmarkItem(item));
    }
    return ret;
}

bool QmlBookmarks::update(QObject *object, const QVariantMap &changes)
{
    auto treeNode = qobject_cast<QmlBookmarkTreeNode*>(object);
    if (!treeNode) {
        qWarning() << "Unable to update bookmark:" << "unable to cast QVariant to QmlBookmarkTreeNode";
        return false;
    }

    auto item = getBookmarkItem(treeNode);
    if (!item) {
        qWarning() << "Unable to update bookmark:" << "bookmark not found";
        return false;
    }

    if (!mApp->bookmarks()->canBeModified(item)) {
        qWarning() << "Unable to update bookmark:" << "bookmark can not be modified";
    }

    QString newTitle = changes["title"].toString().isNull() ? treeNode->title() : changes["title"].toString();
    QString newDescription = changes["description"].toString().isNull() ? treeNode->description() : changes["description"].toString();
    QString newKeyword = changes["keyword"].toString().isNull() ? treeNode->keyword() : changes["keyword"].toString();

    item->setTitle(newTitle);
    item->setDescription(newDescription);
    item->setKeyword(newKeyword);
    mApp->bookmarks()->changeBookmark(item);
    return true;
}

QmlBookmarkTreeNode *QmlBookmarks::get(const QString &string)
{
    auto items = mApp->bookmarks()->searchBookmarks(QUrl(string));
    for (auto item : items) {
        if (item->urlString() == string) {
            return QmlBookmarkTreeNode::fromBookmarkItem(item);
        }
    }

    return new QmlBookmarkTreeNode();
}

QList<QObject*> QmlBookmarks::getChildren(QObject *object)
{
    QList<QObject*> ret;

    auto bookmarkItem = getBookmarkItem(object);
    if (!bookmarkItem) {
        qWarning() << "Unable to get children:" << "parent not found";
        return ret;
    }

    auto items = bookmarkItem->children();
    for (auto item : items) {
        ret.append(QmlBookmarkTreeNode::fromBookmarkItem(item));
    }

    return ret;
}

bool QmlBookmarks::isTreeNodeEqualsItem(QmlBookmarkTreeNode *treeNode, BookmarkItem *item)
{
    return treeNode->title() == item->title()
            && treeNode->url() == item->urlString()
            && treeNode->description() == item->description()
            && (int)(treeNode->type()) == (int)(item->type());
}
