#include "stravacachewidget.h"
#include <QMessageBox>
#include <QCloseEvent>
#include "data/stravacache.h"

using namespace std::string_literals;

StravaCacheWidget::StravaCacheWidget(QWidget *parent) :
    QDialog(parent),
    _showFirstTime(true)
{
    setWindowModality(Qt::WindowModality::NonModal);
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    ui.setupUi(this);
    strava_cache_ = std::make_unique<StravaCache>(R"(D:\GitRepo\strava\strava-activity-loader\StravaActivitiesLoader\bin\Debug\Pryimachuk_4238847)"s);
}

void StravaCacheWidget::hideEvent(QHideEvent *event)
{
    _geometry = geometry();
}

void StravaCacheWidget::showEvent(QShowEvent *event)
{
    if (!_showFirstTime)
    {
        setGeometry(_geometry);
    }
    _showFirstTime = false;
}

void StravaCacheWidget::on_read_button_clicked()
{
    QStringList qlevels;
    for(const auto& l : strava_cache_->detail_levels())
    {
        qlevels.push_back(QString::fromStdString(l));
    }

    ui.detail_level_combo->clear();
    ui.detail_level_combo->addItems(qlevels);
    ui.detail_level_combo->setCurrentIndex(ui.detail_level_combo->findText("Medium"));
}

void StravaCacheWidget::on_detail_level_combo_currentTextChanged(const QString &level)
{
    QStringList qyears;
    for(const auto& l : strava_cache_->years(level.toStdString()))
    {
        qyears.push_back(QString::fromStdString(l));
    }

    ui.date_range_from->clear();
    ui.date_range_from->addItems(qyears);
    ui.date_range_to->clear();
    ui.date_range_to->addItems(qyears);
    ui.date_range_to->setCurrentIndex(qyears.size() - 1);
}

void StravaCacheWidget::on_display_button_clicked()
{
    const auto& level = ui.detail_level_combo->currentText().toStdString();
    const auto& year_from = ui.date_range_from->currentText().toInt();
    const auto& year_to = ui.date_range_to->currentText().toInt();
    for (auto year = year_from; year <= year_to; ++year)
    {
        const auto& it = strava_cache_->activities(level, year);
        while (it->has_next())
        {
            const auto& activity = it->next();

            QFileOpenEvent event(QString::fromStdString(activity));
            QApplication::sendEvent(qApp, &event);
        }
    }
}
