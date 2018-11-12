#ifndef OPENINMPVICON_H
#define OPENINMPVICON_H

#include <abstractbuttoninterface.h>

class OpenInMpvIcon : public AbstractButtonInterface
{
	Q_OBJECT

public:
	explicit OpenInMpvIcon(QObject *parent = nullptr);

	QString id() const override;
	QString name() const override;

private:
	void updateState();
	void clicked(ClickController *controller);
};

#endif //OPENINMPVICON_H
