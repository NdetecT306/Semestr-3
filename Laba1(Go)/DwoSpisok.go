package main

import (
	"fmt"
)

// Group - элемент двусвязного списка
type Group struct {
	student string
	next    *Group
	prev    *Group
}
// GroupPoint - структура для управления списком
type GroupPoint struct {
	head *Group
	tail *Group
}
// CreateGroup - создание списка
func CreateGroup(str string) *GroupPoint {
	node := &Group{
		student: str,
		next:    nil,
		prev:    nil,
	}
	return &GroupPoint{
		head: node,
		tail: node,
	}
}
// AddStudPosle - добавить студента после ptr
func AddStudPosle(gp *GroupPoint, ptr *Group, n string) {
	if ptr == nil {
		return
	}
	element := &Group{
		student: n,
		next:    ptr.next,
		prev:    ptr,
	}
	if ptr.next != nil {
		ptr.next.prev = element
	}
	ptr.next = element
	if ptr == gp.tail {
		gp.tail = element
	}
}
// AddStudDo - добавить студента до ptr
func AddStudDo(gp *GroupPoint, ptr *Group, n string) {
	if ptr == nil {
		return
	}
	element := &Group{
		student: n,
		next:    ptr,
		prev:    ptr.prev,
	}
	if ptr.prev != nil {
		ptr.prev.next = element
	} else {
		// Если добавляем перед головой, обновляем голову
		gp.head = element
	}
	ptr.prev = element
}
// deleteStudPosle - удалить студента после ptr
func deleteStudPosle(gp *GroupPoint, ptr *Group) {
	if ptr == nil || ptr.next == nil {
		return
	}
	element := ptr.next
	ptr.next = element.next
	if element.next != nil {
		element.next.prev = ptr
	}
	if element == gp.tail {
		gp.tail = ptr
	}
}
// deleteStudDo - удалить студента до ptr
func deleteStudDo(gp *GroupPoint, ptr *Group) {
	if ptr == nil || ptr.prev == nil {
		return
	}
	element := ptr.prev
	if element.prev == nil {
		ptr.prev = nil
		gp.head = ptr
	} else {
		element.prev.next = ptr
		ptr.prev = element.prev
	}
	if element == gp.tail {
		gp.tail = ptr
	}
}
// AddStudToStart - добавить в голову
func AddStudToStart(gp *GroupPoint, n string) {
	element := &Group{
		student: n,
		next:    gp.head,
		prev:    nil,
	}
	if gp.head != nil {
		gp.head.prev = element
	}
	gp.head = element
	if gp.tail == nil {
		gp.tail = element
	}
}
// AddStudToEnd - добавить в конец
func AddStudToEnd(gp *GroupPoint, n string) {
	element := &Group{
		student: n,
		next:    nil,
		prev:    gp.tail,
	}
	if gp.head == nil {
		gp.head = element
		gp.tail = element
		return
	}
	gp.tail.next = element
	gp.tail = element
}
// DeleteStudFromStart - удалить голову
func DeleteStudFromStart(gp *GroupPoint) {
	if gp.head == nil {
		return
	}
	if gp.head.next == nil {
		// Только один элемент в списке
		gp.head = nil
		gp.tail = nil
		return
	}
	gp.head = gp.head.next
	gp.head.prev = nil
}
// DeleteStudFromEnd - удалить конец
func DeleteStudFromEnd(gp *GroupPoint) {
	if gp.head == nil {
		return
	}
	if gp.head.next == nil {
		// Только один элемент в списке
		gp.head = nil
		gp.tail = nil
		return
	}
	gp.tail = gp.tail.prev
	gp.tail.next = nil
}
// PrintGroupForward - печать списка в прямом порядке
func PrintGroupForward(gp *GroupPoint) {
	ptr := gp.head
	for ptr != nil {
		fmt.Print(ptr.student, " ")
		ptr = ptr.next
	}
	fmt.Println()
}
// PrintGroupBackward - печать списка в обратном порядке
func PrintGroupBackward(gp *GroupPoint) {
	ptr := gp.tail
	for ptr != nil {
		fmt.Print(ptr.student, " ")
		ptr = ptr.prev
	}
	fmt.Println()
}
// PrintGroupRecursive - рекурсивная печать в прямом порядке
func PrintGroupRecursive(ptr *Group) {
	if ptr == nil {
		fmt.Println()
		return
	}
	fmt.Print(ptr.student, " ")
	PrintGroupRecursive(ptr.next)
}
// OtherPrintGroup - печать в обратном порядке с использованием рекурсии
func OtherPrintGroup(ptr *Group) {
	if ptr == nil {
		fmt.Println()
		return
	}
	OtherPrintGroup(ptr.next)
	fmt.Print(ptr.student, " ")
}
// PoiskGroup - поиск элемента по значению (есть или нет)
func PoiskGroup(gp *GroupPoint, str string) bool {
	element := gp.head
	for element != nil {
		if element.student == str {
			return true
		}
		element = element.next
	}
	return false
}
// FindGroupElement - поиск элемента и возврат указателя
func FindGroupElement(gp *GroupPoint, str string) *Group {
	element := gp.head
	for element != nil {
		if element.student == str {
			return element
		}
		element = element.next
	}
	return nil
}
// DeleteGroupPoZnach - удаление по значению
func DeleteGroupPoZnach(gp *GroupPoint, str string) {
	if gp.head == nil {
		return
	}
	if !PoiskGroup(gp, str) {
		fmt.Println("Нельзя удалить несуществующий элемент")
		return
	}
	element := gp.head
	for element != nil && element.student != str {
		element = element.next
	}
	if element == nil {
		fmt.Println("Элемент не найден")
		return
	}
	if element.prev != nil {
		element.prev.next = element.next
	} else {
		// Удаляем голову
		gp.head = element.next
	}
	if element.next != nil {
		element.next.prev = element.prev
	} else {
		// Удаляем хвост
		gp.tail = element.prev
	}
}
// SizeGroup - размер списка
func SizeGroup(gp *GroupPoint) int {
	count := 0
	ptr := gp.head
	for ptr != nil {
		count++
		ptr = ptr.next
	}
	return count
}
// IsEmpty - проверка пустоты списка
func IsEmpty(gp *GroupPoint) bool {
	return gp.head == nil
}
