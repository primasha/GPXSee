#include "stravacache.h"
#include <QDir>
#include <QDirIterator>

#include <sstream>

StravaCache::Iterator::Iterator(const std::string& root)
    : it_(std::make_unique<QDirIterator>(QString::fromStdString(root), QDir::Filter::Files | QDir::Filter::NoDotAndDotDot))
{
}

std::string StravaCache::Iterator::Iterator::next() const
{
    return it_->next().toStdString();
}

bool StravaCache::Iterator::Iterator::has_next() const
{
    return it_->hasNext();
}

StravaCache::StravaCache(const std::string& root)
    : root_(root)
{
}

std::vector<std::string> StravaCache::detail_levels() const
{
    std::vector<std::string> levels;
    QDirIterator it(QString::fromStdString(root_), QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);
    while (it.hasNext())
    {
        it.next();
        levels.push_back(it.fileName().toStdString());
    }

    return levels;
}

std::vector<std::string> StravaCache::years(const std::string& level) const
{
    std::vector<std::string> years;
    if (level.empty())
    {
        return years;
    }

    QDirIterator it(QDir::cleanPath(QString::fromStdString(root_) + QDir::separator() + QString::fromStdString(level)),
                    QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);
    while (it.hasNext())
    {
        it.next();
        years.push_back(it.fileName().toStdString());
    }

    return years;
}

std::unique_ptr<StravaCache::Iterator> StravaCache::activities(const std::string& level, int year) const
{
    std::ostringstream path;
    path << root_ << QDir::separator().toLatin1() << level << QDir::separator().toLatin1() << year;

    const auto& root = QDir::cleanPath(QString::fromStdString(path.str()));
    std::unique_ptr<StravaCache::Iterator> it;
    it.reset(new StravaCache::Iterator(root.toStdString()));
    return it;
}
