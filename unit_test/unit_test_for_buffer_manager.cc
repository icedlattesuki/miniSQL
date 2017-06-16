#include "../buffer_manager.h"
using namespace std;

int main() {
    BufferManager buffer_manager;
    char* buffer = buffer_manager.getPage("buffer_manager_test" , 0);
    int page_id = buffer_manager.getPageId("buffer_manager_test" , 0);
    cout << "unpin " << buffer_manager.unpinPage(page_id) << endl;
    cout << "unpin " << buffer_manager.unpinPage(page_id) << endl;    
    buffer_manager.pinPage(page_id);
    cout << "unpin " << buffer_manager.unpinPage(page_id) << endl; 
    cout << "page id " << buffer_manager.getPageId("buffer_manager_test" , 0) << endl;       
    strcpy(buffer , "ZJU is the Best!");
    buffer_manager.modifyPage(page_id);
    buffer_manager.flushPage(page_id , "buffer_manager_test" , 0);
}