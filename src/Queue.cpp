#include "Queue.h"

template <class T>
Queue<T>::Queue():queue(0),last(0),size(0){
}


template <class T>
void Queue<T>::forEach(void (*cb)(T*)) const {
	Elem<T>* tek = this->queue;
	while(tek) {
		cb(tek->data);
		tek= tek->next;
	}
}


template <class T>
void Queue<T>::filter(int (*cb)(T*)) {
	Elem<T>* tek = this->queue;
	Elem<T>* last = 0;
	Elem<T>* old = 0;
	while(tek) {
		int stay = cb(tek->data);
		if(stay == 0) {
			if(last == 0) {
				this->queue = tek->next;
			} else {
				last->next = tek->next;
			}
			old = tek;
			tek = tek->next;			
		}
		if(!old) {			
			last = tek;
			tek = tek->next;
		} else {
			old->next = 0;
			delete old;
			old = 0;
			this->size -= 1;
		}
	}
}

template <class T>
int Queue<T>::getSize() const {
	return this->size;
}

template <class T>
Queue<T>::~Queue() {
	Elem<T>* tek = this->queue;

	while(tek != 0) {
		Elem<T>* old = tek;
		tek = tek->next;
		delete old;
	}

	this->queue = 0;
}

template <class T>
void Queue<T>::put(T *const data) {
	Elem<T>* newElem = new Elem<T>;
	newElem->data = data;
	newElem->next = 0;
	this->size += 1;
	if (this->queue == 0) {
		this->queue =  newElem;
	} else {
		this->last->next = newElem;
	}
	this->last = newElem;
}


template <class T>
T* Queue<T>::get() {
	if(this->queue == 0) {	
		return 0;
	}
	this->size -= 1;
	Elem<T>* tek = this->queue;
	this->queue = tek->next;
	
	T* data = tek->data;
	delete tek;
	return data;
}


template <class T>
T* Queue<T>::get(T * data){
	Elem<T>* tek = this->queue;
	Elem<T>* last = 0;
	T* oldData = 0;
	
	while(tek != 0) {
		if(tek->data == data) {
			if(last == 0) {
				this->queue = tek->next;
			} else {
				last->next = tek->next;
			}
			this->size -= 1;
			oldData = tek->data;
			delete tek;
			return oldData;
		}
		last = tek;
		tek = tek->next;
	}
	return oldData;
}


template <class T>
T* Queue<T>::findById(ID id) const {
	Elem<T>* tek = this->queue;

	while(tek != 0) {
		if(tek->data->id == id) {
			return tek->data;
		}
		tek = tek->next;
	}
	return 0;
}

template <class T>
ID Queue<T>::find(T * data) const {
	if(!data) {
		return -1;
	}

	Elem<T>* tek = this->queue;

	while(tek != 0) {
		if(tek->data == data) {
			return tek->data->id;
		}
		tek = tek->next;
	}

	return -1;

}