#ifndef UI_CENTRAL_RIGHT_TABLE_INFO_WIDGET_MODEL_H
#define UI_CENTRAL_RIGHT_TABLE_INFO_WIDGET_MODEL_H

// Main Window
//   Central Right Widget
//      Table Tab Widget
//          Info Tab Model

#include <QString>
#include <QIcon>

namespace meow {
namespace models {
namespace ui {

enum class TableInfoWidgetTabs {
    Basic,
    Options,
    Indexes,
    ForeignKeys
};

class TableInfoWidgetModel
{
public:
    TableInfoWidgetModel();
    const QString tabTitle(TableInfoWidgetTabs tab) const;
    const QIcon tabIcon(TableInfoWidgetTabs tab) const;
};

} // namespace ui
} // namespace models
} // namespace meow

#endif // UI_CENTRAL_RIGHT_TABLE_INFO_WIDGET_MODEL_H
