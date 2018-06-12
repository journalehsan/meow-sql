#ifndef DB_MYSQLQUERY_H
#define DB_MYSQLQUERY_H

#include "query.h"
#include "mysql_connection.h"
#include "mysql_query_result.h"

namespace meow {
namespace db {

class MySQLQuery : public Query
{
public:
    MySQLQuery(Connection * connection = nullptr);
    virtual ~MySQLQuery();

    virtual void execute(bool addResult = false) override;

    virtual bool hasResult() override;

    virtual void seekRecNo(db::ulonglong value) override;

    virtual QString curRowColumn(std::size_t index,
                                 bool ignoreErrors = false) override;

    virtual bool isNull(std::size_t index) override;

private:

    DataTypeIndex dataTypeOfField(MYSQL_FIELD * field);
    void throwOnInvalidColumnIndex(std::size_t index);

    std::vector<MySQLQueryResultPt> _resultList;
    MYSQL_ROW _curRow;
    MySQLQueryResultPt _currentResult; // TODO: really need this? H: FCurrentResults
    std::vector<unsigned int> _columnLengths; // FColumnLengths


};

} // namespace db
} // namespace meow

#endif // DB_MYSQLQUERY_H
