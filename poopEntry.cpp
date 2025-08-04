#include "poopEntry.h"

// 枚举转字符串
QString poopStatusToString(PoopStatus status) {
    switch (status) {
    case PoopStatus::Normal: return "Normal";
    case PoopStatus::Hard: return "Hard";
    case PoopStatus::Soft: return "Soft";
    return "Unknown";
    }
}

// 字符串转枚举
PoopStatus poopStatusFromString(const QString& str) {
    if (str == "Normal") return PoopStatus::Normal;
    if (str == "Hard") return PoopStatus::Hard;
    if (str == "Soft") return PoopStatus::Soft;
    return PoopStatus::Normal; // 默认 fallback
}

// 保存为 JSON
QJsonObject PoopEntry::toJson() const {
    QJsonObject obj;
    obj["time"] = time.toString(Qt::ISODate);
    obj["status"] = poopStatusToString(status);
    return obj;
}

// 从 JSON 加载
PoopEntry PoopEntry::fromJson(const QJsonObject& obj) {
    PoopEntry entry;
    entry.time = QDateTime::fromString(obj["time"].toString(), Qt::ISODate);
    entry.status = poopStatusFromString(obj["status"].toString());
    return entry;
}
