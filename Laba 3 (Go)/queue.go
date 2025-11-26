package main

import (
	"errors"
	"fmt"
)

// QueueNode представляет узел очереди
type QueueNode struct {
	toy  string
	next *QueueNode
}

// Queue представляет саму очередь
type Queue struct {
	head *QueueNode
	tail *QueueNode
}

// NewQueue создает новую пустую очередь
func NewQueue() *Queue {
	return &Queue{}
}

// QPUSH добавляет элемент в конец очереди
func (q *Queue) QPUSH(str string) {
	element := &QueueNode{toy: str, next: nil}
	
	if q.head == nil {
		q.head = element
		q.tail = element
	} else {
		q.tail.next = element
		q.tail = element
	}
}

// QDELETE удаляет элемент из начала очереди
func (q *Queue) QDELETE() error {
	if q.head == nil {
		return errors.New("очередь пуста")
	}
	
	element := q.head
	q.head = q.head.next
	element.next = nil // Помогаем сборщику мусора
	
	// Если очередь стала пустой, обнуляем tail
	if q.head == nil {
		q.tail = nil
	}
	
	return nil
}

// QPOP удаляет и возвращает элемент из начала очереди
func (q *Queue) QPOP() (string, error) {
	if q.head == nil {
		return "", errors.New("очередь пуста")
	}
	
	element := q.head
	item := element.toy
	q.head = q.head.next
	element.next = nil // Помогаем сборщику мусора
	
	// Если очередь стала пустой, обнуляем tail
	if q.head == nil {
		q.tail = nil
	}
	
	return item, nil
}

// ReadQueue выводит все элементы очереди
func (q *Queue) ReadQueue() {
	if q.head == nil {
		fmt.Println("Очередь пуста")
		return
	}
	
	element := q.head
	for element != nil {
		fmt.Printf("%s ", element.toy)
		element = element.next
	}
	fmt.Println()
}

// CountQueue возвращает количество элементов в очереди
func (q *Queue) CountQueue() int {
	count := 0
	element := q.head
	
	for element != nil {
		count++
		element = element.next
	}
	
	return count
}

// ClearQueue очищает всю очередь
func (q *Queue) ClearQueue() {
	for q.head != nil {
		q.QDELETE()
	}
}
