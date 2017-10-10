#ifndef H_QUEUE_H_
#define H_QUEUE_H_

#include "typedefs.h"

template<class T>
struct Elem {
	  T* data;
	  Elem<T>* next;
};

template <class T>
class Queue {

	private:
	Elem<T>* queue;
	Elem<T>* last;
	int size;
	
	public:
	Queue();
	
	T* get();
	T* get(T *);

	void put(T *const);
	
	T* findById(ID id) const;
	ID find(T *) const;
	
	void forEach(void (*cb)(T*)) const;
	void filter(int (*cb)(T*)); 
	int getSize() const;
	
	~Queue();
};


#endif /* H_QUEUE_H_ */
