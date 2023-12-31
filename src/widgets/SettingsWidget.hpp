#ifndef SETTINGSWIDGET_HPP
#define SETTINGSWIDGET_HPP

#include <QListWidget>
#include <QLabel>
#include <QStackedWidget>

#include "WinWindow.hpp"
#include "ConfigItem.hpp"



class SettingsWidget : public WinWindow {
    Q_OBJECT

public:
    SettingsWidget(QWidget * parentWindow, ConfigItem & config);

    void addPage(QWidget * widget, const QString & title);

private Q_SLOTS:
    void _selectPage(int index);
    void _setTitle(const QString & title);

private:
    void _createShortcuts();
    void _createPages();

private:
    QListWidget * _menu;
    QLabel * _title;
    QStackedWidget * _pages;

    ConfigItem & _config;
};



#endif // SETTINGSWIDGET_HPP
