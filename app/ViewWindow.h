// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch

#ifndef EQUATOR_APP_VIEWWINDOW_H
#define EQUATOR_APP_VIEWWINDOW_H

#include <gtkgl--/glarea.h>
#include <gtk--/window.h>
#include <gtk--/menu.h>

class MainWindow;
class GlView;

class ViewWindow : public Gtk::Window
{
  private:
    GlView * m_glarea;
    Gtk::Menu * m_popup;
    std::string m_name;

  public:
    MainWindow & m_mainwindow;

    explicit ViewWindow(MainWindow &);

    gint buttonEvent(GdkEventButton*);

    const std::string & getName() const {
        return m_name;
    }

    void setName(const std::string & n);
    void setTitle();
};

#endif // EQUATOR_APP_VIEWWINDOW_H
