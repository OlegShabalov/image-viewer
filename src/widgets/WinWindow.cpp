#include "WinWindow.hpp"

#include <QSettings>
#include <QApplication>
#include <QDir>

#include <windows.h>



WinWindow::WinWindow(const QString & settingsPath,
                     const QString & windowIdentifier,
                     int defaultWidth, int defaultHeight,
                     QWidget * parent)
    : QWidget(parent)
    , _settingsPath(settingsPath)
    , _windowIdentifier(windowIdentifier)
{
    resize(defaultWidth, defaultHeight);
    _loadGeometry();
}



void WinWindow::setMaximizedSizeRestoring(bool enable) {
    _isMaximizedSizeRestoring = enable;
}



bool WinWindow::changeDownScreenMode(bool enableNormalazeFromMaximize,
                                     bool enableHiding)
{
    if (isFullScreen()) {
        changeFullscreenMode();
        return true;
    }
    if (enableNormalazeFromMaximize && isMaximized()) {
        showNormal();
        return true;
    }
    if (enableHiding) {
        showMinimized();
        return true;
    }
    return false;
}



void WinWindow::changeFullscreenMode() {
    if (isFullScreen()) {
        if (_isMaximizedBeforeFullScreen) {
            _isProblemGeometryEvent = true;
            showMaximized();
            _isProblemGeometryEvent = false;
        }
        else {
            showNormal();
        }

        if (_isKeepOnTopEnable) {
            _setKeepOnTopMode(true);
        }
    }
    else {
        _saveGeometry();

        if (_isKeepOnTopEnable) {
            _setKeepOnTopMode(false);
        }

        if (isMaximized()) {
            _isMaximizedBeforeFullScreen = true;
            QWidget::showFullScreen();
        }
        else {
            _isMaximizedBeforeFullScreen = false;
            //setVisible(false);
            showMaximized();
            QWidget::showFullScreen();
            //setVisible(true);
        }
    }
}

bool WinWindow::setFullscreenMode(bool enable) {
    if (isFullScreen() == enable) {
        return false;
    }
    changeFullscreenMode();
    return true;
}



void WinWindow::changeKeepOnTopMode() {
    _isKeepOnTopEnable = !_isKeepOnTopEnable;
    if (!isFullScreen()) {
        _setKeepOnTopMode(_isKeepOnTopEnable);
    }
}

bool WinWindow::setKeepOnTopMode(bool enable) {
    if (_isKeepOnTopEnable == enable) {
        return false;
    }
    changeKeepOnTopMode();
    return true;
}



void WinWindow::closeEvent(QCloseEvent * event) {
    if (!isFullScreen()) {
        _saveGeometry();
    }
    QWidget::closeEvent(event);
}



void WinWindow::_setKeepOnTopMode(bool enable) {
    const HWND state = enable ? HWND_TOPMOST : HWND_NOTOPMOST;
    SetWindowPos(HWND(winId()), state, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}



void WinWindow::_loadGeometry() {
    const QString settingsPath =
        QDir(QApplication::applicationDirPath()).filePath(_settingsPath);

    const QSettings settings(settingsPath, QSettings::IniFormat);
    const QByteArray a =
        settings.value(_windowIdentifier+":WindowPlacement").toByteArray();

    if (!a.isEmpty()) {
        const WINDOWPLACEMENT * placement =
            reinterpret_cast<const WINDOWPLACEMENT *>(a.constData());

        if (placement->showCmd == SW_SHOWMAXIMIZED) {
            showMaximized();
        }

        SetWindowPlacement(HWND(winId()), placement);
    }
}

void WinWindow::_saveGeometry() {
    WINDOWPLACEMENT placement;
    placement.length = sizeof(WINDOWPLACEMENT);

    if (GetWindowPlacement(HWND(winId()), &placement)) {
        if (_isMaximizedSizeRestoring && isMaximized()) {
            placement.showCmd = SW_SHOWMAXIMIZED;
        }
        else {
            placement.showCmd = SW_SHOWNORMAL;
        }

        const char * p = reinterpret_cast<const char * >(&placement);
        const QByteArray a = QByteArray::fromRawData(p, sizeof(WINDOWPLACEMENT));

        const QString settingsPath =
            QDir(QApplication::applicationDirPath()).filePath(_settingsPath);

        QSettings settings(settingsPath, QSettings::IniFormat);
        settings.setValue(_windowIdentifier+":WindowPlacement", a);
    }
}
