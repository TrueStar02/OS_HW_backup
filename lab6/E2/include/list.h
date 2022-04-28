#ifndef LIST_H_
#define LIST_H_
struct ListItem
{	ListItem *previous;
    ListItem *next;
};
class List
{	public:
    	ListItem head;
		List();
		void initialize();
		int size();
		bool empty();
		ListItem *back();
		void push_back(ListItem *itemPtr);
		void pop_back();
		ListItem *front();
		void push_front(ListItem *itemPtr);
		void pop_front();
		void insert(int pos, ListItem *itemPtr);
		void erase(int pos);
		void erase(ListItem *itemPtr);
		ListItem *at(int pos);
		int find(ListItem *itemPtr);
};
#endif
