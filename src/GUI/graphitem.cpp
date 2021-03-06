#include <QPainter>
#include "graphitem.h"


GraphItem::GraphItem(const Graph &graph, GraphType type, QGraphicsItem *parent)
  : QGraphicsObject(parent), _graph(graph), _type(type)
{
	Q_ASSERT(_graph.isValid());

	_id = 0;
	_width = 1;
	_pen = QPen(Qt::black, _width);
	_sx = 1.0; _sy = 1.0;
	_time = _graph.hasTime();

	setZValue(2.0);

	updatePath();
	updateShape();
	updateBounds();

	setAcceptHoverEvents(true);
}

void GraphItem::updateShape()
{
	QPainterPathStroker s;
	s.setWidth(_width + 1);
	_shape = s.createStroke(_path);
}

void GraphItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
  QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	painter->setPen(_pen);
	painter->drawPath(_path);

/*
	QPen p = QPen(QBrush(Qt::red), 0);
	painter->setPen(p);
	painter->drawRect(boundingRect());
*/
}

void GraphItem::setGraphType(GraphType type)
{
	if (type == _type)
		return;

	prepareGeometryChange();

	_type = type;
	updatePath();
	updateShape();
	updateBounds();
}

void GraphItem::setColor(const QColor &color)
{
	if (_pen.color() == color)
		return;

	_pen.setColor(color);
	update();
}

void GraphItem::setWidth(int width)
{
	if (width == _width)
		return;

	prepareGeometryChange();

	_width = width;
	_pen.setWidth(width);

	updateShape();
}

const GraphSegment *GraphItem::segment(qreal x, GraphType type) const
{
	for (int i = 0; i < _graph.size(); i++)
		if (x <= _graph.at(i).last().x(type))
			return &(_graph.at(i));

	return 0;
}

qreal GraphItem::yAtX(qreal x)
{
	const GraphSegment *seg = segment(x, _type);
	if (!seg)
		return NAN;

	int low = 0;
	int high = seg->count() - 1;
	int mid = 0;

	if (!(x >= seg->at(low).x(_type) && x <= seg->at(high).x(_type)))
		return NAN;

	while (low <= high) {
		mid = low + ((high - low) / 2);
		const GraphPoint &p = seg->at(mid);
		if (p.x(_type) > x)
			high = mid - 1;
		else if (p.x(_type) < x)
			low = mid + 1;
		else
			return -p.y();
	}

	QLineF l;
	if (seg->at(mid).x(_type) < x)
		l = QLineF(seg->at(mid).x(_type), seg->at(mid).y(),
		  seg->at(mid+1).x(_type), seg->at(mid+1).y());
	else
		l = QLineF(seg->at(mid-1).x(_type), seg->at(mid-1).y(),
		  seg->at(mid).x(_type), seg->at(mid).y());

	return -l.pointAt((x - l.p1().x()) / (l.p2().x() - l.p1().x())).y();
}

qreal GraphItem::distanceAtTime(qreal time)
{
	const GraphSegment *seg = segment(time, Time);
	if (!seg)
		return NAN;

	int low = 0;
	int high = seg->count() - 1;
	int mid = 0;

	if (!(time >= seg->at(low).t() && time <= seg->at(high).t()))
		return NAN;

	while (low <= high) {
		mid = low + ((high - low) / 2);
		const GraphPoint &p = seg->at(mid);
		if (p.t() > time)
			high = mid - 1;
		else if (p.t() < time)
			low = mid + 1;
		else
			return seg->at(mid).s();
	}

	QLineF l;
	if (seg->at(mid).t() < time)
		l = QLineF(seg->at(mid).t(), seg->at(mid).s(), seg->at(mid+1).t(),
		  seg->at(mid+1).s());
	else
		l = QLineF(seg->at(mid-1).t(), seg->at(mid-1).s(),
		  seg->at(mid).t(), seg->at(mid).s());

	return l.pointAt((time - l.p1().x()) / (l.p2().x() - l.p1().x())).y();
}

void GraphItem::emitSliderPositionChanged(qreal pos)
{
	if (_type == Time)
		emit sliderPositionChanged(_time ? distanceAtTime(pos) : NAN);
	else
		emit sliderPositionChanged(pos);
}

void GraphItem::hover(bool hover)
{
	if (hover) {
		_pen.setWidth(_width + 1);
		setZValue(zValue() + 1.0);
	} else {
		_pen.setWidth(_width);
		setZValue(zValue() - 1.0);
	}

	update();
}

void GraphItem::setScale(qreal sx, qreal sy)
{
	if (_sx == sx && _sy == sy)
		return;

	prepareGeometryChange();

	_sx = sx; _sy = sy;
	updatePath();
	updateShape();
}

void GraphItem::updatePath()
{
	_path = QPainterPath();

	if (_type == Time && !_time)
		return;

	for (int i = 0; i < _graph.size(); i++) {
		const GraphSegment &segment = _graph.at(i);

		_path.moveTo(segment.first().x(_type) * _sx, -segment.first().y() * _sy);
		for (int i = 1; i < segment.size(); i++)
			_path.lineTo(segment.at(i).x(_type) * _sx, -segment.at(i).y() * _sy);
	}
}

void GraphItem::updateBounds()
{
	if (_type == Time && !_time) {
		_bounds = QRectF();
		return;
	}

	qreal bottom, top, left, right;

	QPointF p = QPointF(_graph.first().first().x(_type),
	  -_graph.first().first().y());
	bottom = p.y(); top = p.y(); left = p.x(); right = p.x();

	for (int i = 0; i < _graph.size(); i++) {
		const GraphSegment &segment = _graph.at(i);

		for (int j = 0; j < segment.size(); j++) {
			p = QPointF(segment.at(j).x(_type), -segment.at(j).y());
			bottom = qMax(bottom, p.y()); top = qMin(top, p.y());
			right = qMax(right, p.x()); left = qMin(left, p.x());
		}
	}

	_bounds = QRectF(QPointF(left, top), QPointF(right, bottom));
}

qreal GraphItem::max() const
{
	qreal ret = _graph.first().first().y();

	for (int i = 0; i < _graph.size(); i++) {
		const GraphSegment &segment = _graph.at(i);

		for (int j = 0; j < segment.size(); j++) {
			qreal y = segment.at(j).y();
			if (y > ret)
				ret = y;
		}
	}

	return ret;
}

qreal GraphItem::min() const
{
	qreal ret = _graph.first().first().y();

	for (int i = 0; i < _graph.size(); i++) {
		const GraphSegment &segment = _graph.at(i);

		for (int j = 0; j < segment.size(); j++) {
			qreal y = segment.at(j).y();
			if (y < ret)
				ret = y;
		}
	}

	return ret;
}

qreal GraphItem::avg() const
{
	qreal sum = 0;

	for (int i = 0; i < _graph.size(); i++) {
		const GraphSegment &segment = _graph.at(i);

		for (int j = 1; j < segment.size(); j++)
			sum += segment.at(j).y() * (segment.at(j).s() - segment.at(j-1).s());
	}

	return sum/_graph.last().last().s();
}

void GraphItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);

	_pen.setWidthF(_width + 1);
	setZValue(zValue() + 1.0);
	update();

	emit selected(true);
}

void GraphItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);

	_pen.setWidthF(_width);
	setZValue(zValue() - 1.0);
	update();

	emit selected(false);
}
