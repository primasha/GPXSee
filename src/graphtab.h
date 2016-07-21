#ifndef GRAPHTAB_H
#define GRAPHTAB_H

#include "graphview.h"
#include "units.h"

class GPX;

class GraphTab : public GraphView
{
	Q_OBJECT

public:
	GraphTab(QWidget *parent = 0) : GraphView(parent)
	  {setFrameShape(QFrame::NoFrame);}

	virtual QString label() const = 0;
	virtual void loadGPX(const GPX &gpx) = 0;
	virtual void clear() = 0;
	virtual void setUnits(enum Units units) = 0;
};

#endif // GRAPHTAB_H