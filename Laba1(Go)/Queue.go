package main

import (
	"fmt"
)

// Toys - элемент очереди
type Toys struct {
	toy  string
	site *Toys
}
// addQueue - добавление в очередь (enqueue)
func addQueue(head **Toys, tail **Toys, str string) {
	element := &Toys{
		toy:  str,
		site: nil,
	}
	if *head == nil {
		*head = element
		*tail = element
	} else {
		(*tail).site = element
		*tail = element
	}
}
// deleteQueue - удаление из очереди (dequeue без возврата значения)
func deleteQueue(head **Toys) {
	if *head == nil {
		fmt.Println("Очередь пуста")
		return
	}
	*head = (*head).site
}
// QPOP - извлечение элемента из очереди с возвратом значения (dequeue)
func QPOP(head **Toys) string {
	if *head == nil {
		return "Очередь пуста"
	}
	element := *head
	item := element.toy
	*head = (*head).site
	return item
}
// readQueue - чтение очереди (итеративно)
func readQueue(head *Toys) {
	if head == nil {
		fmt.Println("Очередь пуста")
		return
	}
	element := head
	for element != nil {
		fmt.Print(element.toy, " ")
		element = element.site
	}
	fmt.Println()
}
// readQueueRecursive - чтение очереди (рекурсивно)
func readQueueRecursive(head *Toys) {
	if head == nil {
		fmt.Println()
		return
	}
	fmt.Print(head.toy, " ")
	readQueueRecursive(head.site)
}
// countQueue - подсчет элементов
func countQueue(head *Toys) int {
	count := 0
	element := head
	for element != nil {
		count++
		element = element.site
	}
	return count
}
// isEmpty - проверка пустоты очереди
func isEmpty(head *Toys) bool {
	return head == nil
}
// peek - просмотр первого элемента без удаления
func peek(head *Toys) string {
	if head == nil {
		return "Очередь пуста"
	}
	return head.toy
}
// clearQueue - очистка всей очереди
func clearQueue(head **Toys, tail **Toys) {
	*head = nil
	*tail = nil
}
// getTail - получение указателя на хвост (для тестирования)
func getTail(tail *Toys) string {
	if tail == nil {
		return "Хвост не существует"
	}
	return tail.toy
}
