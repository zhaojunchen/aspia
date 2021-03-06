//
// Aspia Project
// Copyright (C) 2018 Dmitry Chapyshev <dmitry@aspia.ru>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//

#ifndef CLIENT__UI__DESKTOP_WINDOW_H
#define CLIENT__UI__DESKTOP_WINDOW_H

#include "client/ui/client_window.h"
#include "client/ui/desktop_widget.h"
#include "client/client_desktop.h"
#include "proto/desktop.pb.h"
#include "proto/desktop_extensions.pb.h"

class QHBoxLayout;
class QScrollArea;

namespace common {
class Clipboard;
} // namespace common

namespace desktop {
class Frame;
} // namespace desktop

namespace client {

class DesktopConfigDialog;
class DesktopPanel;
class SystemInfoWindow;

class DesktopWindow :
    public ClientWindow,
    public ClientDesktop::Delegate,
    public DesktopWidget::Delegate
{
    Q_OBJECT

public:
    DesktopWindow(const ConnectData& connect_data, QWidget* parent);
    ~DesktopWindow();

    // ClientDesktop::Delegate implementation.
    void extensionListChanged() override;
    void configRequered() override;
    void setDesktopRect(const desktop::Rect& screen_rect) override;
    void drawDesktop() override;
    desktop::Frame* desktopFrame() override;
    void setRemoteCursor(const QCursor& cursor) override;
    void setRemoteClipboard(const proto::desktop::ClipboardEvent& event) override;
    void setScreenList(const proto::desktop::ScreenList& screen_list) override;
    void setSystemInfo(const proto::system_info::SystemInfo& system_info) override;

    // DesktopWidget::Delegate implementation.
    void onPointerEvent(const QPoint& pos, uint32_t mask) override;
    void onKeyEvent(uint32_t usb_keycode, uint32_t flags) override;
    void onDrawDesktop() override;

protected:
    // QWidget implementation.
    void timerEvent(QTimerEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    bool eventFilter(QObject* object, QEvent* event) override;

    // ClientWindow implementation.
    void sessionError() override;

private slots:
    void changeSettings();
    void onConfigChanged(const proto::desktop::Config& config);
    void autosizeWindow();
    void takeScreenshot();
    void scaleDesktop();

private:
    ClientDesktop* desktopClient();

    static QString createWindowTitle(const ConnectData& connect_data);

    QHBoxLayout* layout_ = nullptr;
    QScrollArea* scroll_area_ = nullptr;
    DesktopPanel* panel_ = nullptr;
    DesktopWidget* desktop_ = nullptr;
    common::Clipboard* clipboard_ = nullptr;

    QPointer<DesktopConfigDialog> config_dialog_;
    QPointer<SystemInfoWindow> system_info_;

    int scroll_timer_id_ = 0;
    QPoint scroll_delta_;

    bool is_maximized_ = false;

    desktop::Point screen_top_left_;

    DISALLOW_COPY_AND_ASSIGN(DesktopWindow);
};

} // namespace client

#endif // CLIENT__UI__DESKTOP_WINDOW_H
