#include "buffer_manager.h"

int main() {
    BufferManager buffer_manager;
    int page_id = buffer_manager.getEmptyPageId();
    buffer_manager.loadDiskBlock(page_id , "buffer_manager_test" , 0);
    cout << "unpin " << buffer_manager.unpinPage(page_id) << endl;
    cout << "unpin " << buffer_manager.unpinPage(page_id) << endl;    
    buffer_manager.pinPage(page_id);
    cout << "unpin " << buffer_manager.unpinPage(page_id) << endl; 
    cout << "page id " << buffer_manager.getPageId("buffer_manager_test" , 0) << endl;       
    char* buffer = buffer_manager.getPage(page_id);
    strcpy(buffer , "I love you");
    buffer_manager.modifyPage(page_id);
    buffer_manager.flushPage(page_id , "buffer_manager_test" , 0);
}