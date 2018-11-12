#include "icon.h"

OpenInMpvIcon::OpenInMpvIcon(QObject *parent)
	: AbstractButtonInterface(parent)
{
	setIcon(QIcon::fromTheme(QSL("openinmpv-icon"), QIcon(QSL(":openinmpv/data/icon.svg"))));
	setTitle(tr("Open In Mpv"));
	setToolTip(tr("Open In Mpv"));
}

QString OpenInMpvIcon::id() const
{
	return QSL("openinmpv");
}

QString OpenInMpvIcon::name() const
{
	return tr("Open In Mpv");
}
