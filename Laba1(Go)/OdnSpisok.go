package main

import (
	"fmt"
)

// Spisok - односвязный список
type Spisok struct {
	name  string
	place *Spisok
}
// CreateList - создание списка
func CreateList(str string) *Spisok {
	return &Spisok{name: str, place: nil}
}
// AddChelPosle - добавить человека после ptr
func AddChelPosle(ptr *Spisok, str string) {
	element := &Spisok{
		name:  str,
		place: ptr.place,
	}
	ptr.place = element
}
// AddChelDo - добавить человека до ptr
func AddChelDo(head **Spisok, ptr *Spisok, str string) {
	element := &Spisok{
		name: str,
	}
	if *head == ptr {
		element.place = *head
		*head = element
	} else {
		per := *head
		for per != nil && per.place != ptr {
			per = per.place
		}
		if per != nil {
			element.place = ptr
			per.place = element
		}
	}
}
// AddToEnd - добавить в конец списка
func AddToEnd(ptr *Spisok, n string) {
	element := &Spisok{
		name:  n,
		place: nil,
	}
	temp := ptr
	for temp.place != nil {
		temp = temp.place
	}
	temp.place = element
}
// AddToBegin - добавить в начало списка
func AddToBegin(ptr **Spisok, n string) {
	element := &Spisok{
		name:  n,
		place: *ptr,
	}
	*ptr = element
}
// Print - печать списка
func Print(ptr *Spisok) {
	for ptr != nil {
		fmt.Print(ptr.name, " ")
		ptr = ptr.place
	}
	fmt.Println()
}
// SizeList - размер списка
func SizeList(ptr *Spisok) int {
	count := 0
	for ptr != nil {
		count++
		ptr = ptr.place
	}
	return count
}
// Poisk - поиск элемента по значению (есть или нет)
func Poisk(ptr *Spisok, str string) bool {
	element := ptr
	for element != nil {
		if element.name == str {
			return true
		}
		element = element.place
	}
	return false
}
// DeletePoZnach - удаление по значению
func DeletePoZnach(ptr **Spisok, str string) {
	if *ptr == nil {
		return
	}
	if !Poisk(*ptr, str) {
		fmt.Println("Нельзя удалить несуществующий элемент")
		return
	}
	element := *ptr
	var prev *Spisok = nil
	for element != nil && element.name != str {
		prev = element
		element = element.place
	}
	if prev == nil {
		*ptr = element.place
	} else {
		prev.place = element.place
	}
}
// DeleteToBegin - удалить начало списка
func DeleteToBegin(ptr **Spisok) {
	if *ptr == nil {
		return
	}
	if (*ptr).place == nil {
		*ptr = nil
		return
	}
	*ptr = (*ptr).place
}
// DeleteToEnd - удалить конец списка
func DeleteToEnd(ptr **Spisok) {
	if *ptr == nil {
		return
	}
	if (*ptr).place == nil {
		*ptr = nil
		return
	}
	temp := *ptr
	for temp.place.place != nil {
		temp = temp.place
	}
	temp.place = nil
}
// FindElement - поиск элемента (вернуть указатель)
func FindElement(ptr *Spisok, str string) *Spisok {
	element := ptr
	for element != nil {
		if element.name == str {
			return element
		}
		element = element.place
	}
	return nil
}
// DeleteChelDo - удалить до заданного значения
func DeleteChelDo(ptr **Spisok, target *Spisok) {
	if *ptr == nil || target == nil {
		return
	}
	if *ptr == target {
		return
	}
	var prevPrev *Spisok = nil
	prev := *ptr
	element := (*ptr).place
	for element != nil && element != target {
		prevPrev = prev
		prev = element
		element = element.place
	}
	if element != target {
		return
	}
	if prevPrev == nil {
		DeleteToBegin(ptr)
		*ptr = element
	} else {
		prevPrev.place = prev.place
	}
}
// DeleteChelPosle - удаляет после заданного значения
func DeleteChelPosle(ptr *Spisok) {
	if ptr == nil || ptr.place == nil {
		return
	}
	ptr.place = ptr.place.place
}
