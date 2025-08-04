#ifndef POOPENTRY_H
#define POOPENTRY_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

// 定义枚举类型
enum class PoopStatus {
    Normal,
    Hard,
    Soft
};

// 提供转换函数
QString poopStatusToString(PoopStatus status);
PoopStatus poopStatusFromString(const QString& str);

// 数据结构
struct PoopEntry {
    QDateTime time;
    PoopStatus status;

    QJsonObject toJson() const;
    static PoopEntry fromJson(const QJsonObject& obj);
};

#endif // POOPENTRY_H
