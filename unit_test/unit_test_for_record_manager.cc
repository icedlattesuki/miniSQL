#include "../record_manager.h"
using namespace std;

int main() {
    RecordManager record_manager;
    CatalogManager catalog_manager;
    Attribute attr;
    attr.num = 3;
    attr.name[0] = "name";
    attr.name[1] = "age";
    attr.name[2] = "salary";
    attr.type[0] = 3;
    attr.type[1] = -1;
    attr.type[2] = 0;
    attr.unique[0] = false;
    attr.unique[1] = false;
    attr.unique[2] = false;
    attr.primary_key = -1;
    Index index;
    index.num = 0;
    catalog_manager.createTable("test",attr,-1,index);
    record_manager.createTableFile("test");
    catalog_manager.createTable("test1",attr,-1,index);
    record_manager.createTableFile("test1");
    catalog_manager.dropTable("test1");
    record_manager.dropTableFile("test1");
    for (int i = 0;i < 100;i++) {
        Tuple tuple;
        Data data1 , data2 , data3;
        data1.type = 3;
        data1.datas = "hys";
        data2.type = -1;
        data2.datai = i;
        data3.type = 0;
        data3.dataf = 100.5;
        tuple.addData(data1);
        tuple.addData(data2);
        tuple.addData(data3);
        record_manager.insertRecord("test",tuple);
    }
    // Tuple tuple1;
    // data1.datas = "sr1";
    // tuple1.addData(data1);
    // tuple1.addData(data2);
    // tuple1.addData(data3);
    // record_manager.insertRecord("test",tuple1);
    Table table;
    table = record_manager.selectRecord("test");
    table.showTable(10);
    cout << endl;
    Where where;
    where.data.type = -1;
    where.data.datai = 50;
    where.relation_character = EQUAL;
    table = record_manager.selectRecord("test","age",where);
    table.showTable();
    cout << endl;
    record_manager.deleteRecord("test","age",where);
    table = record_manager.selectRecord("test","age",where);
    table.showTable();
    cout << endl;
}