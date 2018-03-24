#ifndef UI_CENTRAL_RIGHT_TABLE_INFO_INDEXES_TAB_H
#define UI_CENTRAL_RIGHT_TABLE_INFO_INDEXES_TAB_H

#include <QtWidgets>

namespace meow {

namespace models {
namespace forms {
    class TableInfoForm;
}
}

namespace ui {
namespace main_window {
namespace central_right {
namespace table_info {

class TableIndexesTools;

class IndexesTab : public QWidget
{
    Q_OBJECT
public:
    explicit IndexesTab(models::forms::TableInfoForm * form,
                        QWidget *parent = nullptr);

    void refreshData();
    void validateControls();

    Q_SLOT void actionAddIndex(bool checked);
    Q_SLOT void actionAddColumnToIndex(bool checked);
    Q_SLOT void actionRemoveCurrentIndexItem(bool checked);
    Q_SLOT void actionClearIndexes(bool checked);
    Q_SLOT void actionMoveUpColumn(bool checked);
    Q_SLOT void actionMoveDownColumn(bool checked);

private:
    void createWidgets();
    void fillDataFromForm();

    Q_SLOT void selectedIndexItemChanged(
        const QItemSelection &selected,
        const QItemSelection &deselected);

    models::forms::TableInfoForm * _tableForm;
    TableIndexesTools * _tools;
    QTreeView * _indexesTree;
    QHBoxLayout * _mainLayout;
};

} // namespace table_info
} // namespace central_right
} // namespace main_window
} // namespace ui
} // namespace meow

#endif // UI_CENTRAL_RIGHT_TABLE_INFO_INDEXES_TAB_H
