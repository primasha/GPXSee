#ifndef STRAVACACHE_H
#define STRAVACACHE_H

#include <string>
#include <vector>

#include <QDirIterator>

class StravaCache
{
public:
    class Iterator
    {
        friend class StravaCache;
    public:
        std::string next() const;
        bool has_next() const;

    private:
        Iterator(const std::string& root);
        Iterator(const Iterator&) = delete;

    private:
        std::unique_ptr<QDirIterator> it_;
    };

    StravaCache(const std::string& root);

    std::vector<std::string> detail_levels() const;
    std::vector<std::string> years(const std::string& level) const;
    std::unique_ptr<Iterator> activities(const std::string& level, int year) const;

private:
    const std::string root_;
};

#endif // STRAVACACHE_H
