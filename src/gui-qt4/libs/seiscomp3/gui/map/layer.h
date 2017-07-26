/***************************************************************************
 *   Copyright (C) by gempa GmbH                                           *
 *                                                                         *
 *   You can redistribute and/or modify this program under the             *
 *   terms of the SeisComP Public License.                                 *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   SeisComP Public License for more details.                             *
 ***************************************************************************/


#ifndef __SEISCOMP_GUI_MAP_LAYER_H__
#define __SEISCOMP_GUI_MAP_LAYER_H__


#ifndef Q_MOC_RUN
#include <seiscomp3/core/baseobject.h>
#include <seiscomp3/core/interfacefactory.h>
#endif

#include <seiscomp3/gui/qt4.h>
#include <seiscomp3/gui/map/legend.h>
#include <seiscomp3/gui/map/mapsymbol.h>

#include <QObject>


class QContextMenuEvent;
class QMouseEvent;
class QDialog;
class QMenu;
class QPainter;
class QWidget;

namespace Seiscomp {

namespace Config {
class Config;
}

namespace Gui {
namespace Map {

class Canvas;

DEFINE_SMARTPOINTER(Layer);

class SC_GUI_API Layer : public QObject, public Seiscomp::Core::BaseObject {
	Q_OBJECT

	public:
		enum UpdateHint{Position};
		Q_DECLARE_FLAGS(UpdateHints, UpdateHint)

		typedef QList<Legend*> Legends;


	public:
		Layer(QObject* parent = NULL);
		virtual ~Layer();


	public:
		virtual void setConfig(const std::string &/*config*/) {}
		virtual void init(const Seiscomp::Config::Config&);
		virtual void draw(const Seiscomp::Gui::Map::Canvas*, QPainter&) {}

		virtual Layer &operator =(const Layer &other);
		virtual Layer *clone() const { return NULL; }


	public slots:
		void setAntiAliasingEnabled(bool);
		virtual void setVisible(bool);


	public:
		void setName(const QString&);
		const QString &name() const;

		void setDescription(const QString&);
		const QString &description() const;

		bool addLegend(Seiscomp::Gui::Map::Legend *legend);
		bool removeLegend(Seiscomp::Gui::Map::Legend *legend);

		int legendCount() const { return _legends.count(); }
		Legend *legend(int i) const;

		const QList<Legend*> &legends() const { return _legends; }

		bool isVisible() const;
		bool isAntiAliasingEnabled() const;

		Canvas *canvas() const { return _canvas; }


	public:
		/**
		 * @brief Convenience function that returns the size in pixels of the
		 *        layer. By default it forwards the request to the canvas if
		 *        the layer is attached to canvas or an invalid size otherwise.
		 * @return The size of the layer.
		 */
		virtual QSize size() const;

		virtual void calculateMapPosition(const Map::Canvas *canvas);
		virtual bool isInside(int x, int y) const;
		virtual void baseBufferUpdated(Map::Canvas *canvas);
		virtual void bufferUpdated(Map::Canvas *canvas);

		virtual void handleEnterEvent();
		virtual void handleLeaveEvent();

		virtual bool filterContextMenuEvent(QContextMenuEvent*, QWidget*);
		virtual bool filterMouseMoveEvent(QMouseEvent *event, const QPointF &geoPos);
		virtual bool filterMousePressEvent(QMouseEvent *event, const QPointF &geoPos);
		virtual bool filterMouseReleaseEvent(QMouseEvent *event, const QPointF &geoPos);
		virtual bool filterMouseDoubleClickEvent(QMouseEvent *event, const QPointF &geoPos);

		virtual QMenu *menu(QWidget*) const;


	signals:
		void updateRequested(const Layer::UpdateHints& = UpdateHints());


	private slots:
		void onObjectDestroyed(QObject *object);


	private:
		Canvas                       *_canvas;
		QString                       _name;
		QString                       _description;
		bool                          _visible;
		bool                          _antiAliasing;
		Legends                       _legends;


	friend class Canvas;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Layer::UpdateHints)

DEFINE_INTERFACE_FACTORY(Layer);

#define REGISTER_LAYER_INTERFACE(Class, Service) \
Seiscomp::Core::Generic::InterfaceFactory<Seiscomp::Gui::Map::Layer, Class> __##Class##InterfaceFactory__(Service)


} // namespace Map
} // namespce Gui
} // namespace Seiscomp

#endif