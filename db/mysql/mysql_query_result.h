#ifndef DB_MYSQL_QUERY_RESULT_H
#define DB_MYSQL_QUERY_RESULT_H

#include <mysql/mysql.h>
#include "db/native_query_result.h"

namespace meow {
namespace db {

// Wraps and owns MYSQL_RES ptr
class MySQLQueryResult : public NativeQueryResult
{
public:
    MySQLQueryResult(Connection * connection = nullptr);

    void init(MYSQL_RES * res);

    virtual ~MySQLQueryResult() override {
        freeNative();
    }

    virtual db::ulonglong nativeRowsCount() const override {
        return static_cast<db::ulonglong>(_res->row_count);
    }

    virtual bool hasData() const override;

    virtual void seekRecNo(db::ulonglong value) override;

    virtual QString curRowColumn(std::size_t index,
                                 bool ignoreErrors = false) override;

    virtual bool isNull(std::size_t index) override;

    MYSQL_RES * nativePtr() const { return _res; }

protected:
    virtual void prepareResultForEditing(NativeQueryResult * result) override;
private:

    void freeNative() {
        if (_res) {
            mysql_free_result(_res);
            _res = nullptr;
        }
    }

    QString rowDataToString(MYSQL_ROW row,
                            std::size_t col,
                            unsigned long dataLen);

    void prepareResultForEditing(MYSQL_RES * result);

    void clearColumnData();
    void addColumnData(MYSQL_RES * result);


    std::vector<MYSQL_RES *> resultList() const;

    MYSQL_RES * _res;
    MYSQL_ROW _curRow;
    std::vector<unsigned int> _columnLengths; // FColumnLengths
    bool _columnsParsed;
};

using MySQLQueryResultPtr = std::shared_ptr<MySQLQueryResult>;

} // namespace db
} // namespace meow

#endif // DB_MYSQL_QUERY_RESULT_H
