#ifndef STRAVACACHEWIDGET_H
#define STRAVACACHEWIDGET_H

#include <QDialog>
#include "ui_stravacachewidget.h"
#include <memory>

class StravaCache;

class StravaCacheWidget : public QDialog
{
    Q_OBJECT

public:
    explicit StravaCacheWidget(QWidget *parent = nullptr);

private slots:
    void on_read_button_clicked();
    void on_detail_level_combo_currentTextChanged(const QString &level);

    void on_display_button_clicked();

private:
    virtual void hideEvent(QHideEvent* event) override;
    virtual void showEvent(QShowEvent* event) override;

private:
    Ui::Form ui;
    QRect _geometry;
    bool _showFirstTime;
    std::unique_ptr<StravaCache> strava_cache_;
};

#endif // STRAVACACHEWIDGET_H
