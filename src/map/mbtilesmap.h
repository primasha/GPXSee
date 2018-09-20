#ifndef MBTILESMAP_H
#define MBTILESMAP_H

#include <QSqlDatabase>
#include <QByteArray>
#include "common/range.h"
#include "map.h"

class MBTilesMap : public Map
{
public:
	MBTilesMap(const QString &fileName, QObject *parent = 0);

	QString name() const;

	QRectF bounds();
	qreal resolution(const QRectF &rect);

	int zoom() const {return _zoom;}
	void setZoom(int zoom) {_zoom = zoom;}
	int zoomFit(const QSize &size, const RectC &rect);
	int zoomIn();
	int zoomOut();

	QPointF ll2xy(const Coordinates &c);
	Coordinates xy2ll(const QPointF &p);

	void draw(QPainter *painter, const QRectF &rect, Flags flags);

	void load();
	void unload();
	void setDevicePixelRatio(qreal ratio) {_deviceRatio = ratio;}

	bool isValid() const {return _valid;}
	QString errorString() const {return _errorString;}

private:
	int limitZoom(int zoom) const;
	qreal tileSize() const;
	qreal coordinatesRatio() const;
	qreal imageRatio() const;
	QByteArray tileData(int zoom, const QPoint &tile) const;

	QSqlDatabase _db;

	QString _fileName;
	RectC _bounds;
	Range _zooms;
	int _zoom;
	qreal _deviceRatio, _tileRatio;

	bool _valid;
	QString _errorString;
};

#endif // MBTILESMAP_H
