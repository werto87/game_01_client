#ifndef E18680A5_3B06_4019_A849_6CDB82D14796
#define E18680A5_3B06_4019_A849_6CDB82D14796

#include "src/model/database.hxx"

void createEmptyDatabase ();
void createTables ();
void upsertAccount (database::Account const &account);
#endif /* E18680A5_3B06_4019_A849_6CDB82D14796 */
