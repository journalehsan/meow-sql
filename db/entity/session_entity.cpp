#include "session_entity.h"
#include "entity_factory.h"
#include "db/connections_manager.h"
#include "database_entity.h"
#include "table_entity.h"
#include "helpers/logger.h"
#include "app/app.h"
#include "db/connection.h"
#include <QDebug>

namespace meow {
namespace db {

SessionEntity::SessionEntity(const std::shared_ptr<Connection> & connection,
                             ConnectionsManager * parent)
    : Entity(parent),
     _connection(connection),
     _databases(),
     _databasesWereInit(false)
{

}

SessionEntity::~SessionEntity()
{
    meowLogDebug() << "Closing session " << name();
}

QString SessionEntity::name() const
{
    return connection()->connectionParams()->sessionName();
}

Connection * SessionEntity::connection() const
{
    return _connection.get();
}

ConnectionsManager * SessionEntity::connectionsManager() const
{
    return static_cast<ConnectionsManager *>(_parent);
}

int SessionEntity::row() const
{
    return connectionsManager()->indexOf(const_cast<SessionEntity *>(this));
}

int SessionEntity::childCount()
{
    initDatabasesListIfNeed();
    return _databases.size();
}

Entity * SessionEntity::child(int row)
{
    initDatabasesListIfNeed();
    return _databases.value(row).get();
}

QVariant SessionEntity::icon() const
{    
    auto icons = meow::app()->settings()->icons();
    return icons->connection(_connection->connectionParams()->serverType());

}

int SessionEntity::indexOf(DataBaseEntity * session) const
{
    return _databases.indexOf(session->retain());
}

bool SessionEntity::isActive() const
{
    return connectionsManager()->activeSession() == this;
}

DataBaseEntity * SessionEntity::activeDatabase() const
{
    return databaseByName(connection()->database());
}

DataBaseEntity * SessionEntity::databaseByName(const QString & name) const
{
    for (const DataBaseEntityPtr & entity : _databases) {
        if (entity->name() == name) {
            return entity.get();
        }
    }
    return nullptr;
}

void SessionEntity::clearAllDatabaseEntities()
{
    _databases.clear();
    _databasesWereInit = false;
}

void SessionEntity::refreshAllEntities()
{
    clearAllDatabaseEntities();
    initDatabasesListIfNeed();
}

void SessionEntity::editEntityInDB(EntityInDatabase * entity,
                                   EntityInDatabase * newData)
{
    bool changed = connection()->editEntityInDB(entity, newData);
    if (changed) {
        entity->copyDataFrom(newData);
        emit entityEdited(entity);
    }
}

bool SessionEntity::insertEntityToDB(EntityInDatabase * entity)
{
    if (connection()->insertEntityToDB(entity)) {
        entity->setIsNew(false);
        addEntity(entity);
        emit entityInserted(entity->retain());
        return true;
    }
    return false;
}

bool SessionEntity::dropEntityInDB(EntityInDatabase * entity)
{
    // Listening: Behemoth - Bartzabel
    return connection()->dropEntityInDB(entity);
}

bool SessionEntity::dropDatabase(DataBaseEntity * database)
{
    return connection()->dropDatabase(database);
}

void SessionEntity::createDatabase(const QString & name,
                                   const QString & collation)
{
    _connection->createDatabase(name, collation);

    if (_connection->connectionParams()->isAllDatabases()) {
        appendCreatedDatabase(name);
    }
}

bool SessionEntity::editDatabase(DataBaseEntity * database,
                                 const QString & newName,
                                 const QString & newCollation)
{
    // editing database is not supported (at least in MySQL), so we move all
    // its entities to newly created or existing one, create new db and
    // drop old one (but later and outside this function)

    QStringList allDatabases = _connection->allDatabases();
    bool moveToExisting = allDatabases.contains(newName);

    bool changed = _connection->editDatabase(database, newName, newCollation);

    if (changed) {
        //removeEntity(database); // nope: remove later
        if (moveToExisting) {
            DataBaseEntity * database = databaseByName(newName);
            database->clearChildren();
            emit entityEdited(database);
        } else {
            appendCreatedDatabase(newName, database->name());
        }
    }

    return changed;
}

db::ulonglong SessionEntity::dataSize() const // override
{
    db::ulonglong sum = 0;

    foreach (const DataBaseEntityPtr & entity, _databases) {
        sum += entity->dataSize();
    }

    return sum;
}

void SessionEntity::initDatabasesListIfNeed()
{
    if (_databasesWereInit == false) {

        QStringList databaseNames = connection()->databases(true);

        for (const QString & dbName : databaseNames) {
            DataBaseEntityPtr dbEntity = EntityFactory::createDataBase(
                        dbName,
                        const_cast<SessionEntity *>(this));
            _databases.push_back(dbEntity);
        }

        _databasesWereInit = true;
    }
}

void SessionEntity::addEntity(Entity * entity)
{
    if ( (int)entity->type() >= (int)Entity::Type::Table ) {
        EntityInDatabase * entityInDb = static_cast<EntityInDatabase *>(entity);
        entityInDb->dataBaseEntity()->appendEntity(entityInDb);
    }
}

void SessionEntity::appendCreatedDatabase(
        const QString & name,
        const QString & afterName)
{
    QStringList allDatabases = _connection->allDatabases(true);
    if (allDatabases.contains(name) == false) return;

    initDatabasesListIfNeed();
    DataBaseEntityPtr dbEntity = EntityFactory::createDataBase(
                name,
                const_cast<SessionEntity *>(this));

    if (afterName.isNull() == false) {
        for (int i = 0; i < _databases.size(); ++i) {
            if (_databases.at(i)->name() == afterName) {
                _databases.insert(i+1, dbEntity);
                break;
            }
        }
    } else {
        _databases.push_back(dbEntity);
    }

    emit databaseInserted(dbEntity);
    emit entityInserted(dbEntity);
}

bool SessionEntity::removeEntity(Entity * entity)
{
    // Listening: The Agonist - Business Suits And Combat Boots

    bool success = false;

    // retain for signals, then remove

    EntityPtr entityPtr = entity->retain();

    emit beforeEntityRemoved(entity);
    if ( (int)entity->type() >= (int)Entity::Type::Table ) {
        EntityInDatabase * entityInDb = static_cast<EntityInDatabase *>(entity);
        success = entityInDb->dataBaseEntity()->removeEntity(entityInDb);
    } else if (entity->type() == Entity::Type::Database) {
        DataBaseEntityPtr database
                = std::static_pointer_cast<DataBaseEntity>(entityPtr);
        success = _databases.removeOne(database);
        if (success) {
            emit databaseRemoved(database);
        }
    } else {
        Q_ASSERT(0);
    }

    if (success) {
        emit entityRemoved(entityPtr);
    }

    return success;
}

} // namespace db
} // namespace meow
