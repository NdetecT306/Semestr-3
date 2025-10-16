package main

import (
	"fmt"
)

// Garden - элемент стека
type Garden struct {
	item  string
	point *Garden
}
// addStack - добавление элемента (push)
func addStack(ptr **Garden, str string) {
	element := &Garden{
		item:  str,
		point: *ptr,
	}
	*ptr = element
}
// deleteStack - удаление элемента (pop без возврата значения)
func deleteStack(ptr **Garden) {
	if *ptr != nil {
		*ptr = (*ptr).point
	}
}
// readStack - чтение стека (рекурсивно)
func readStack(ptr *Garden) {
	if ptr == nil {
		fmt.Println()
		return
	}
	fmt.Print(ptr.item, " ")
	readStack(ptr.point)
}
// readStackIterative - чтение стека (итеративно)
func readStackIterative(ptr *Garden) {
	current := ptr
	for current != nil {
		fmt.Print(current.item, " ")
		current = current.point
	}
	fmt.Println()
}
// popStack - чтение и удаление элемента из стека (pop с возвратом значения)
func popStack(ptr **Garden) string {
	if *ptr == nil {
		return "STACK_EMPTY"
	}
	item := (*ptr).item
	*ptr = (*ptr).point
	return item
}
// countStack - подсчет элементов в стеке
func countStack(ptr *Garden) int {
	count := 0
	element := ptr
	for element != nil {
		count++
		element = element.point
	}
	return count
}
// isEmpty - проверка пустоты стека
func isEmpty(ptr *Garden) bool {
	return ptr == nil
}
// peek - просмотр верхнего элемента без удаления
func peek(ptr *Garden) string {
	if ptr == nil {
		return "STACK_EMPTY"
	}
	return ptr.item
}
// clearStack - очистка всего стека
func clearStack(ptr **Garden) {
	*ptr = nil
}
